

///////////////////////////////////////// VOID LED RGB /////////////////////////////////////////
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

#define ledRGB_R 5 //  D1
#define ledRGB_G 4 //  D2
#define ledRGB_B 0 //  D3

void SetupRGB(){
  
  pinMode(ledRGB_R,OUTPUT);
  pinMode(ledRGB_G,OUTPUT);
  pinMode(ledRGB_B,OUTPUT);
}

void LoopRGB(){
  
  analogWrite(ledRGB_R, resultadoR);
  analogWrite(ledRGB_G, resultadoG);
  analogWrite(ledRGB_B, resultadoB);
}
