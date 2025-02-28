

///////////////////////////////////////// DHT E DIMMER /////////////////////////////////////////
/*
/////// PINAGEM DO ESP PARA ARDUINO ///////
-------------------------------------------
NodeMCU / ESP8266   |  NodeMCU / ESP8266  |
D0 = 16             |  D6 = 12            |
D1 = 5              |  D7 = 13            |
D2 = 4              |  D8 = 15            |
D3 = 0              |  D9 = 3             |
D4 = 2              |  D10 = 1            |
D5 = 14             |                     |
-------------------------------------------
*/


#define     nDHT     10

float valDHT[nDHT];
float resulDHT = 0.0;
float resulFinalDht = 0;

int numeroT = 0;
int estatoAnteriorDoDht = 0;
int estadoAtualDoDht = 0;

//int resultadoT =0;

// DEFINIÇÕES DE PINOS
#define pinDht 2  // D4
#define pinZC  13 // D7
#define pinDIM 15 // D8


// DEFINIÇÕES
#define intervaloLeitura 300

#define periodo 8333 // MICROSSEGUNDOS

//#define temperaturaIdeal 32.5

#define iMin 20 // INTENSIDADE MINIMA 10
#define iMax 90 // INTENSIDADE MÁX 100
#define difMin 0.5 // DIFERENÇA MIN EM RELAÇÃO A TEMP IDEAL
#define difMax 5.0 // DIFERENÇA MÁX DISPARA iMax

// INSTANCIANDO OBJETOS
DHT dht(pinDht, DHT22);

// DECLARAÇÃO DE VARIÁVEIS
volatile int intensidade = 0; //CARREGAR VARIÁVEL DA MEMORIA RAM EM VEZ DO REGISTRADOR

unsigned long controleTempo;

void SetupDHT() {
  Serial.begin(115200);

  pinMode(pinDIM, OUTPUT);
  pinMode(pinZC, INPUT);
  //attachInterrupt( digitalPinToInterrupt(pinZC), sinalZC, RISING);
  sinalZC();
  dht.begin();
  controleTempo = millis();

  Serial.println("Fim do Setup()");
}

void LoopDHT() {

  if (millis() - controleTempo > intervaloLeitura) {
    
   // Serial.println("Medindo temperatura...");
    float temperatura = dht.readTemperature();
    
    //float difTemperatura = temperaturaIdeal - temperatura;z
    float difTemperatura = resultadoT - temperatura; //<------------------------------------------ Adaptação
    
   // Serial.print("Temperatura: "); Serial.println(temperatura,2);

    numeroT = temperatura;

   // Serial.println("Calculando intensidade...");
    if      (difTemperatura > 5.0) intensidade = iMax;
    else if (difTemperatura < 0.0) intensidade = 0;
    else if (difTemperatura < 1.5) intensidade = iMin;
    else    {
      // MAP FUNCIONA COM INT ENTÃO MULTIPLICAMOS POR 10 PARA "GANHAR" PRECISÃO
      intensidade = map( int(difTemperatura * 10), (difMin * 10), (difMax * 10), iMin, iMax);
    }
   // Serial.print("Intensidade: "); Serial.println(intensidade);
    
    controleTempo += intervaloLeitura;

//***************************************************************************************************

  //estadoAtualDoDht = temperatura;
  

    for(byte i = 0 ; i <= nDHT - 2 ; i++) {
       valDHT[i] = numeroT;  
       resulDHT +=valDHT[i];
    }
    resulDHT /= 10.0;
  
    estatoAnteriorDoDht = resulDHT;
  
    if( estadoAtualDoDht != estatoAnteriorDoDht ){
    
    estadoAtualDoDht = estatoAnteriorDoDht;
    
    envioTEMP1 ();
    
    }
    
//***************************************************************************************************

  }else{sinalZC();
  }
  sinalZC();
    //envioTEMP ();  // <-------------------------------------------------------------------------------------------------------- AQUI Ó 
}

// IMPLEMENTO DE FUNÇÕES
void sinalZC() {
  if ( intensidade < iMin) return;
  if ( intensidade > iMax) intensidade = iMax;

  int delayInt = 8333 - (intensidade * (8333 / 100) );

  delayMicroseconds(delayInt);
  digitalWrite(15, HIGH);
  delayMicroseconds(6);
  digitalWrite(15, LOW);
}
