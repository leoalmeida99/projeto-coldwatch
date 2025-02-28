
///////////////////////////////////// ENVIA AS MENSAGENS PRO SERVIDOR /////////////////////////////////////////

void envio(){

    //#define MSG_BUFFER_SIZE  (50)
    //char R[MSG_BUFFER_SIZE];
    //int temp = 0;
    //temp = analogRead(A0)/10;
    
    //delay(200);
}

void envioTEMP (){
  
    #define MSG_BUFFER_SIZE  (50)
    char R[MSG_BUFFER_SIZE];
    snprintf (R, MSG_BUFFER_SIZE, "%ld", resultadoT,"%ld");
    client.publish("temp", R);
}


void envioTEMP1 (){
  
    #define MSG_BUFFER_SIZE  (50)
    char T[MSG_BUFFER_SIZE];
//    snprintf (T, MSG_BUFFER_SIZE, "%ld", numeroT,"%ld");
    client.publish("tempDHT", T);
}

void envioUMID (){
    
    #define MSG_BUFFER_SIZE1  (50)
    char R2[MSG_BUFFER_SIZE1];
    snprintf (R2, MSG_BUFFER_SIZE1, "%ld", estadoAtualDaUmi ,"%ld"); 
    client.publish("umi", R2);
}
/*
void envioALERT (){
        
    #define MSG_BUFFER_SIZE2  (50)
    char R3[MSG_BUFFER_SIZE2];
    snprintf (R3, MSG_BUFFER_SIZE2, "%ld", estadoAtualDoZoiudo,"%ld");
    client.publish("alerta", R3);
}*/

void envioRGB (){  //ESSE TOPICO É APENAS PARA TESTE DOS VALORES FINAIS DO CÓDIGO RGB 
        
    #define MSG_BUFFER_SIZE3  (50)
    char R4[MSG_BUFFER_SIZE3];
    snprintf (R4, MSG_BUFFER_SIZE3, "%ld", resultadoR,"%ld");
    client.publish("corR", R4);
        
    #define MSG_BUFFER_SIZE4  (50)
    char R5[MSG_BUFFER_SIZE4];
    snprintf (R5, MSG_BUFFER_SIZE4, "%ld", resultadoG,"%ld");
    client.publish("corG", R5);
        
    #define MSG_BUFFER_SIZE5  (50)
    char R6[MSG_BUFFER_SIZE5];
    snprintf (R6, MSG_BUFFER_SIZE5, "%ld", resultadoB,"%ld");
    client.publish("corB", R6);
}

