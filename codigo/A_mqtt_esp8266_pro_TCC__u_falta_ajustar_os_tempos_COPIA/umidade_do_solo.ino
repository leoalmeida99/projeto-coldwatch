

///////////////////////////////////// SENSOR DE UMIDADE DO SOLO /////////////////////////////////////////
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
#define     n     100
#define  pino_led_motor        16 // D0

float val[n];
float resul = 0.0;
int resulFinalU = 0;
int vamorUMI = 0;


void SetupUMI() {
  pinMode(pino_led_motor, OUTPUT);
}

void LoopUMI(){

  if(resultadoU > 0){
    
  for(byte i = 0 ; i <= n - 2 ; i++) {
    val[i] = analogRead(A0);  
    resul +=val[i];
  }
  resul /= 100.0;
  resulFinalU = resul;
  resulFinalU = (resulFinalU*100) / 1024;

  if (resulFinalU <= resultadoU){
    digitalWrite(pino_led_motor, HIGH);
  
  }else{ digitalWrite(pino_led_motor, LOW);}  
  
  estatoAnteriorDaUmi = resul;

  if( estadoAtualDaUmi != estatoAnteriorDaUmi ){
    
    estadoAtualDaUmi = estatoAnteriorDaUmi;

    estadoAtualDaUmi = map(estadoAtualDaUmi, 650, 1024, 100, 0); 
    
    envioUMID ();
    
   Serial.print("UMIDADE DO SOLO: "); Serial.println(estadoAtualDaUmi);
  }
 
  }
}

