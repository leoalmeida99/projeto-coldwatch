 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Arquivo:   MQTT_WiFiManager_Rele.ino
//Tipo:      Exemplo de uso das bibliotecas WiFiManager e PubSubClient para ESP8266 na IDE do Arduino
//Autor:     Marco Rabelo para o canal Infortronica Para Zumbis (www.youtube.com/c/InfortronicaParaZumbis)
//Descricao: Curso de ESP8266 - Utilizando MQTT e gerenciador de wifi para controlar um relé.
//Video:     https://youtu.be/oX4ttJEULmA
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Equivalencia das saidas Digitais entre NodeMCU e ESP8266 (na IDE do Arduino)
NodeMCU - ESP8266
D0 = 16;
D1 = 5;
D2 = 4;
D3 = 0;
D4 = 2;
D5 = 14;
D6 = 12;
D7 = 13;
D8 = 15;
D9 = 3;
D10 = 1;
*/

    int resultadoR = 0;
    int resultadoG = 0;
    int resultadoB = 0;
    
    int resultadoA = 0;
    int resultadoT = 0;
    int resultadoU = 0;

    byte estatoAnteriorDaUmi = 0;
    byte estadoAtualDaUmi = 0;

#include <FS.h>                 //Esta precisa ser a primeira referência, ou nada dará certo e sua vida será arruinada. kkk
#include <ESP8266WiFi.h>        //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>        //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>
#include <EEPROM.h>
#include <DHT.h>            

const char* DEBUG                   //Se descomentar esta linha vai habilitar a 'impressão' na porta serial

//Coloque os valores padrões aqui, porém na interface eles poderão ser substituídos.
const char* servidor_mqtt             "hurricanecloud384.cloud.shiftr.io"  //URL do servidor MQTT
const char* servidor_mqtt_porta       "1883"  //Porta do servidor (a mesma deve ser informada na variável abaixo)
const char* servidor_mqtt_usuario     "hurricanecloud384"  //Usuário
const char* servidor_mqtt_senha       "umaSenhaSimples"  //Senha
const char* mqtt_topico_sub           "esp8266/pincmd"    //Tópico para subscrever o comando a ser dado no pino declarado abaixo

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         //Declaração do pino que será utilizado e a memória alocada para armazenar o status deste pino na EEPROM
         
const char* pino             2                   //Pino que executara a acao dado no topico "esp8266/pincmd" e terá seu status informado no tópico "esp8266/pinstatus"
const char* memoria_alocada  4                   //Define o quanto sera alocado na EEPROM (valores entre 4 e 4096 bytes)

WiFiClient espClient;                        //Instância do WiFiClient
PubSubClient client(espClient);              //Passando a instância do WiFiClient para a instância do PubSubClient

uint8_t statusAnt   =    0;                  //Variável que armazenará o status do pino que foi gravado anteriormente na EEPROM
bool precisaSalvar  =    false;              //Flag para salvar os dados



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                            //Função inicial (será executado SOMENTE quando ligar o ESP)

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  imprimirSerial(true, "...");



  //Fazendo o pino ser de saída, pois ele irá "controlar" algo.
  pinMode(pino, OUTPUT);



  
  //Formatando a memória interna
  //(descomente a linha abaixo enquanto estiver testando e comente ou apague quando estiver pronto)
  //SPIFFS.format();

  //Iniciando o SPIFSS (SPI Flash File System)
  imprimirSerial(true, "Iniciando o SPIFSS (SPI Flash File System)");
  if (SPIFFS.begin()) {
    imprimirSerial(true, "Sistema de arquivos SPIFSS montado!");
    if (SPIFFS.exists("/config.json")) {
      //Arquivo de configuração existe e será lido.
      imprimirSerial(true, "Abrindo o arquivo de configuracao...");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        imprimirSerial(true, "Arquivo de configuracao aberto.");
        size_t size = configFile.size();
        
        //Alocando um buffer para armazenar o conteúdo do arquivo.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
            //Copiando as variáveis salvas previamente no aquivo json para a memória do ESP.
            imprimirSerial(true, "arquivo json analisado.");
            strcpy(servidor_mqtt, json["servidor_mqtt"]);
            strcpy(servidor_mqtt_porta, json["servidor_mqtt_porta"]);
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
  //Fim da leitura do sistema de arquivos SPIFSS

  //Parâmetros extras para configuração
  //Depois de conectar, parameter.getValue() vai pegar o valor configurado.
  //Os campos do WiFiManagerParameter são: id do parâmetro, nome, valor padrão, comprimento
  WiFiManagerParameter custom_mqtt_server("server", "Servidor MQTT", servidor_mqtt, 40);
  WiFiManagerParameter custom_mqtt_port("port", "Porta", servidor_mqtt_porta, 6);
  WiFiManagerParameter custom_mqtt_user("user", "Usuario", servidor_mqtt_usuario, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "Senha", servidor_mqtt_senha, 20);
  WiFiManagerParameter custom_mqtt_topic_sub("topic_sub", "Topico para subscrever", mqtt_topico_sub, 30);

  //Inicialização do WiFiManager. Uma vez iniciado não é necessário mantê-lo em memória.
  WiFiManager wifiManager;

  //Definindo a função que informará a necessidade de salvar as configurações
  wifiManager.setSaveConfigCallback(precisaSalvarCallback);
  
  //Adicionando os parâmetros para conectar ao servidor MQTT
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_topic_sub);

  //Busca o ID e senha da rede wifi e tenta conectar.
  //Caso não consiga conectar ou não exista ID e senha,
  //cria um access point com o nome "AutoConnectAP" e a senha "senha123"
  //E entra em loop aguardando a configuração de uma rede WiFi válida.
  if (!wifiManager.autoConnect("Incubadora", "plantatec")) { //<--------------------------------------------------- Senha da incubadora 
    imprimirSerial(true, "Falha ao conectar. Excedeu o tempo limite para conexao.");
    delay(3000);
    //Reinicia o ESP e tenta novamente ou entra em sono profundo (DeepSleep)
    ESP.reset();
    delay(5000);
  }

  //Se chegou até aqui é porque conectou na WiFi!
  imprimirSerial(true, "Conectado!! :)");

  //Lendo os parâmetros atualizados
  strcpy(servidor_mqtt, custom_mqtt_server.getValue());
  strcpy(servidor_mqtt_porta, custom_mqtt_port.getValue());
  strcpy(servidor_mqtt_usuario, custom_mqtt_user.getValue());
  strcpy(servidor_mqtt_senha, custom_mqtt_pass.getValue());
  strcpy(mqtt_topico_sub, custom_mqtt_topic_sub.getValue());

  //Salvando os parâmetros informados na tela web do WiFiManager
  if (precisaSalvar) {
    imprimirSerial(true, "Salvando as configuracoes");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["servidor_mqtt"] = servidor_mqtt;
    json["servidor_mqtt_porta"] = servidor_mqtt_porta;
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

  //Informando ao client do PubSub a url do servidor e a porta.
  int portaInt = atoi(servidor_mqtt_porta);
  client.setServer(servidor_mqtt, portaInt);
  client.setCallback(retorno);
  
  //Obtendo o status do pino antes do ESP ser desligado
  lerStatusAnteriorPino();

  SetupDHT();
  SetupUMI();
  SetupRGB();
  //SetupALERTA();
  
}


//Função de repetição (será executado INFINITAMENTE até o ESP ser desligado)
void loop() {
  if (!client.connected()) {
    reconectar();
  }
  client.loop();

  LoopDHT();
  LoopUMI();
  LoopRGB();
  //LoopALERTA();
  
}








