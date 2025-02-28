////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arquivo:   MQTT_WiFiManager_Rele.ino
// Tipo:      Exemplo de uso das bibliotecas WiFiManager e PubSubClient para ESP8266 na IDE do Arduino
// Autor:     Marco Rabelo para o canal Infortronica Para Zumbis
// Descricao: Curso de ESP8266 - Utilizando MQTT e gerenciador de WiFi para controlar um relé.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <DHT.h>

#define DEBUG

    int resultadoR = 0;
    int resultadoG = 0;
    int resultadoB = 0;
    
    int resultadoA = 0;
    int resultadoT = 0;
    int resultadoU = 0;

    byte estatoAnteriorDaUmi = 0;
    byte estadoAtualDaUmi = 0;

// Definições de variáveis corrigidas
char servidor_mqtt[50] = "hurricanecloud384.cloud.shiftr.io";
char servidor_mqtt_usuario[50] = "hurricanecloud384";
char servidor_mqtt_senha[50] = "umaSenhaSimples";
char mqtt_topico_sub[50] = "esp8266/pincmd";

// Porta MQTT (precisa ser string para WiFiManagerParameter)
const int servidor_mqtt_porta = 1883;
char servidor_mqtt_porta_str[6];  // String para armazenar a porta

// Definição do pino e alocação de memória
const int pino = 2;
const int memoria_alocada = 4;

// Instâncias do WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

uint8_t statusAnt = 0;
bool precisaSalvar = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Função de configuração
void setup() {
#ifdef DEBUG
    Serial.begin(115200);
#endif
    imprimirSerial(true, "...");

    pinMode(pino, OUTPUT);

    // Formata a string da porta MQTT
    sprintf(servidor_mqtt_porta_str, "%d", servidor_mqtt_porta);

    // Inicia o SPIFFS
    imprimirSerial(true, "Iniciando o SPIFSS (SPI Flash File System)");
    if (SPIFFS.begin()) {
        imprimirSerial(true, "Sistema de arquivos SPIFSS montado!");
        if (SPIFFS.exists("/config.json")) {
            imprimirSerial(true, "Abrindo o arquivo de configuracao...");
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                imprimirSerial(true, "Arquivo de configuracao aberto.");
                size_t size = configFile.size();
                
                // Alocando buffer para armazenar o conteúdo do arquivo
                std::unique_ptr<char[]> buf(new char[size]);
                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                json.printTo(Serial);

                if (json.success()) {
                    imprimirSerial(true, "Arquivo json analisado.");
                    strcpy(servidor_mqtt, json["servidor_mqtt"]);
                    strcpy(servidor_mqtt_porta_str, json["servidor_mqtt_porta"]);
                    strcpy(servidor_mqtt_usuario, json["servidor_mqtt_usuario"]);
                    strcpy(servidor_mqtt_senha, json["servidor_mqtt_senha"]);
                    strcpy(mqtt_topico_sub, json["mqtt_topico_sub"]);
                } else {
                    imprimirSerial(true, "Falha ao ler as configuracoes do arquivo json.");
                }
            }
        }
    } else {
        imprimirSerial(true, "Falha ao montar o sistema de arquivos SPIFSS.");
    }

    // Parâmetros extras para WiFiManager
    WiFiManagerParameter custom_mqtt_server("server", "Servidor MQTT", servidor_mqtt, 40);
    WiFiManagerParameter custom_mqtt_port("port", "Porta", servidor_mqtt_porta_str, 6);
    WiFiManagerParameter custom_mqtt_user("user", "Usuario", servidor_mqtt_usuario, 20);
    WiFiManagerParameter custom_mqtt_pass("pass", "Senha", servidor_mqtt_senha, 20);
    WiFiManagerParameter custom_mqtt_topic_sub("topic_sub", "Topico para subscrever", mqtt_topico_sub, 30);

    WiFiManager wifiManager;
    wifiManager.setSaveConfigCallback(precisaSalvarCallback);

    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_pass);
    wifiManager.addParameter(&custom_mqtt_topic_sub);

    if (!wifiManager.autoConnect("Incubadora", "plantatec")) {
        imprimirSerial(true, "Falha ao conectar. Excedeu o tempo limite para conexao.");
        delay(3000);
        ESP.reset();
        delay(5000);
    }

    imprimirSerial(true, "Conectado!! :)");

    // Atualiza os parâmetros
    strcpy(servidor_mqtt, custom_mqtt_server.getValue());
    strcpy(servidor_mqtt_porta_str, custom_mqtt_port.getValue());
    strcpy(servidor_mqtt_usuario, custom_mqtt_user.getValue());
    strcpy(servidor_mqtt_senha, custom_mqtt_pass.getValue());
    strcpy(mqtt_topico_sub, custom_mqtt_topic_sub.getValue());

    if (precisaSalvar) {
        imprimirSerial(true, "Salvando as configuracoes");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["servidor_mqtt"] = servidor_mqtt;
        json["servidor_mqtt_porta"] = servidor_mqtt_porta_str;
        json["servidor_mqtt_usuario"] = servidor_mqtt_usuario;
        json["servidor_mqtt_senha"] = servidor_mqtt_senha;
        json["mqtt_topico_sub"] = mqtt_topico_sub;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
            imprimirSerial(true, "Houve uma falha ao abrir o arquivo de configuracao para incluir/alterar as configuracoes.");
        }

        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();
    }

    imprimirSerial(false, "IP: ");
    imprimirSerial(true, WiFi.localIP().toString());

    // Converte porta MQTT de string para inteiro
    int portaInt = atoi(servidor_mqtt_porta_str);
    client.setServer(servidor_mqtt, portaInt);
    client.setCallback(retorno);

    // Obtém status do pino
    lerStatusAnteriorPino();

    SetupDHT();
    SetupUMI();
    SetupRGB();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loop principal
void loop() {
    if (!client.connected()) {
        reconectar();
    }
    client.loop();

    LoopDHT();
    LoopUMI();
    LoopRGB();
}

