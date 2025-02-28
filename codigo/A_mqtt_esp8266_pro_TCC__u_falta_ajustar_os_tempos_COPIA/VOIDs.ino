
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                         
                                         //Função para imprimir na porta serial
                                         
void imprimirSerial(bool linha, String mensagem){
  #ifdef DEBUG
    if(linha){
      Serial.println(mensagem);
    }else{
      Serial.print(mensagem);
    }
  #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                 //Função de retorno para notificar sobre a necessidade de salvar as configurações
                 
void precisaSalvarCallback() {
  imprimirSerial(true, "As configuracoes tem que ser salvas.");
  precisaSalvar = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                           //Função que reconecta ao servidor MQTT
                                            
void reconectar() {
  //Repete até conectar
  while (!client.connected()) {
    imprimirSerial(false, "Tentando conectar ao servidor MQTT...");
    
    //Tentativa de conectar. Se o MQTT precisa de autenticação, será chamada a função com autenticação, caso contrário, chama a sem autenticação. 
    bool conectado = strlen(servidor_mqtt_usuario) > 0 ?
                     client.connect("ESP8266Client", servidor_mqtt_usuario, servidor_mqtt_senha) :
                     client.connect("ESP8266Client");

    if(conectado) {
      imprimirSerial(true, "Conectado!");
      //Subscreve para monitorar os comandos recebidos

      //NESSA PARTE NÓS VAMOS ESCREVER TODOS OS TOPICOS A SEREM RECEBIDOS PELO APLICATIVO/SERVIDOR 
      client.subscribe("TEMPERATURA");//<------------------------------------------------ comando em que ele lê o topico "TEMPERATURA" 
      client.subscribe("UMIDADE");//<---------------------------------------------------- comando em que ele lê o topico "UMIDADE" 
      client.subscribe("ALERTA");//<----------------------------------------------------- comando em que ele lê o topico "ALERTA" 
      client.subscribe("COR_R");//<------------------------------------------------------ comando em que ele lê o topico "COR R" 
      client.subscribe("COR_G");//<------------------------------------------------------ comando em que ele lê o topico "COR G" 
      client.subscribe("COR_B");//<------------------------------------------------------ comando em que ele lê o topico "COR B" 
      
    } else {
      imprimirSerial(false, "Falhou ao tentar conectar. Codigo: ");
      imprimirSerial(false, String(client.state()).c_str());
      imprimirSerial(true, " tentando novamente em 5 segundos");
      //Aguarda 5 segundos para tentar novamente
      delay(5000);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                      //Função que verifica qual foi o último status do pino antes do ESP ser desligado

void lerStatusAnteriorPino(){
  EEPROM.begin(memoria_alocada);  //Aloca o espaco definido na memoria
  statusAnt = EEPROM.read(0);     //Le o valor armazenado na EEPROM e passa para a variável "statusAnt"
  if(statusAnt > 1){
    statusAnt = 0;                //Provavelmente é a primeira leitura da EEPROM, passando o valor padrão para o pino.
    EEPROM.write(0,statusAnt);
  }
  digitalWrite(pino, statusAnt);
  EEPROM.end();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                         //Função que grava status do pino na EEPROM
                                         
void gravarStatusPino(uint8_t statusPino){
  EEPROM.begin(memoria_alocada);
  EEPROM.write(0, statusPino);
  EEPROM.end();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                 //Função que será chamada ao receber mensagem do servidor MQTT
/*
void retorno(char* topico, byte* mensagem, unsigned int tamanho) {
  //Convertendo a mensagem recebida para string
  mensagem[tamanho] = '\0';
  String strMensagem = String((char*)mensagem);
  strMensagem.toLowerCase();
  //float f = s.toFloat();
  
  imprimirSerial(false, "Mensagem recebida! Topico: ");
  imprimirSerial(false, topico);
  imprimirSerial(false, ". Tamanho: ");
  imprimirSerial(false, String(tamanho).c_str());
  imprimirSerial(false, ". Mensagem: ");
  imprimirSerial(true, strMensagem);

  //Executando o comando solicitado
  imprimirSerial(false, "Status do pino antes de processar o comando: ");
  imprimirSerial(true, String(digitalRead(pino)).c_str());
  
  if(strMensagem == "liga"){
    imprimirSerial(true, "Colocando o pino em stado ALTO...");
    digitalWrite(pino, HIGH);
    gravarStatusPino(HIGH);
  }else if(strMensagem == "desliga"){
    imprimirSerial(true, "Colocando o pino em stado BAIXO...");
    digitalWrite(pino, LOW);
    gravarStatusPino(LOW);
  }else{
    imprimirSerial(true, "Trocando o estado do pino...");
    digitalWrite(pino, !digitalRead(pino));
    gravarStatusPino(digitalRead(pino));
  }
  
  imprimirSerial(false, "Status do pino depois de processar o comando: ");
  imprimirSerial(true, String(digitalRead(pino)).c_str());
}
*/
