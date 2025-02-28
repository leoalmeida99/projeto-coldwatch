

///////////////////////////////////////// RECEBE AS MENSAGENS /////////////////////////////////////////

//void recebePacote(char* topic, byte* mensagem, unsigned int length) {
  
void retorno(char* topico, byte* mensagem, unsigned int tamanho) {

    String msg;

    for (int i = 0; i < tamanho; i++) {
       char c = (char)mensagem[i];
       msg += c;
      }

  //////////////////////////////////////////////// RGB ////////////////////////////////////////////////

   //ESSES 3 BLOCOS SÃO RESPONSAVEIS PELAS CORES DOS LED'S RGB 
   
  if ((char)mensagem[0] == 'R') {
    
    int PriR =  (char)mensagem[5] - '0';
    int SegR = ((char)mensagem[4] - '0')*10;
    int TerR = ((char)mensagem[3] - '0')*100;

    resultadoR = TerR + SegR + PriR;
  } 
  
  if ((char)mensagem[0] == 'G') {
    
    int PriG =  (char)mensagem[5] - '0';
    int SegG = ((char)mensagem[4] - '0')*10;
    int TerG = ((char)mensagem[3] - '0')*100;

    resultadoG = TerG + SegG + PriG;
  } 
  
  if ((char)mensagem[0] == 'B') {
    
    int PriB =  (char)mensagem[5] - '0';
    int SegB = ((char)mensagem[4] - '0')*10;
    int TerB = ((char)mensagem[3] - '0')*100;

    resultadoB = TerB + SegB + PriB;
    envioRGB();
  } 
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  
  if ((char)mensagem[0] == 'T') {         //<---------------------------------------------- TEMPERATURA 
    
    int PriT =  (char)mensagem[5] - '0';
    int SegT = ((char)mensagem[4] - '0')*10;
    int TerT = ((char)mensagem[3] - '0')*100;

    resultadoT = TerT + SegT + PriT;
//  envioTEMP();
  } 
  
  if ((char)mensagem[0] == 'U') {         //<---------------------------------------------- UMIDADE 
    
    int PriU =  (char)mensagem[5] - '0';
    int SegU = ((char)mensagem[4] - '0')*10;
    int TerU = ((char)mensagem[3] - '0')*100;

    resultadoU = TerU + SegU + PriU;
//  envioUMID();
  } 
  
  if ((char)mensagem[0] == 'A') {         //<---------------------------------------------- ALERTA
    
    int PriA =  (char)mensagem[5] - '0';
    int SegA = ((char)mensagem[4] - '0')*10;
    int TerA = ((char)mensagem[3] - '0')*100;

    resultadoA = TerA + SegA + PriA;
//  envioALERT();
  } 
  delay(100);
} 


