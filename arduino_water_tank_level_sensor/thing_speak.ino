#define  MAX_FAILURES_TO_WARN         60
#define  MAX_FAILURES_TO_RECONNECT    15

String THINGSPEAK_HOST = "api.thingspeak.com";
String THINGSPEAK_PORT = "80";
#define THINGSPEAK_WRITE_API_KEY                        "RJ99UK0DE5N5WRD9"   
#define THINGSPEAK_FIELD_STRING_DATA                    "field1"
#define THINGSPEAK_FIELD_SEPARATOR                      "_"

#define TS_START            'a'
#define TS_AT_CIPMUX        'b'
#define TS_AT_CPSTART       'c'
#define TS_AT_CPSEND        'd'
#define TS_AT_DATA          'e'
#define TS_AT_DATA_DELAY    'f'
#define TS_AT_CLOSE         'g'
#define TS_AT_FINISHED      'h'
#define TS_DONE             'i'
char state = TS_START;
unsigned long delayStartedOn = 0;

unsigned char apiStatusIndex = 0;
char apiStatus[20];

unsigned int thingSpeakConsecutiveFailureCount = 0;

char dc[200];  //keep size 15-20 bytes more than size found in AT+CIPSEND command logs

void thingSpeakSendData(){  

  //state machine....
  switch (state){
      case TS_START:
            connectWifi();
            buildDataToSend();              
            resetWifiCounters();
            state = TS_AT_CIPMUX;
            break;
      case TS_AT_CIPMUX:
            sendWifiCommand(F("AT+CIPMUX=1"), 1, AT_OK);
            state = TS_AT_CPSTART;
            break;
      case TS_AT_CPSTART:
            sendWifiCommand("AT+CIPSTART=0,\"TCP\",\"" + THINGSPEAK_HOST +"\"," + THINGSPEAK_PORT, 1, AT_OK);
            state = TS_AT_CPSEND;
            break;
      case TS_AT_CPSEND:
            sendWifiCommand("AT+CIPSEND=0," + String(strlen(dc)+4), 1, ">");
            state = TS_AT_DATA;
            break;
      case TS_AT_DATA:
            esp8266.println(dc);            
            state = TS_AT_DATA_DELAY;  delayStartedOn = millis();
            break;
      case TS_AT_DATA_DELAY:
            //delay(1500);
            if (millis() - delayStartedOn > 1500){            
              state = TS_AT_CLOSE;
            }
            break;
      case TS_AT_CLOSE:
            sendWifiCommand(F("AT+CIPCLOSE=0"), 1, AT_OK);
            state = TS_AT_FINISHED;
            break;
      case TS_AT_FINISHED:
            handleFailures();
            state = TS_DONE;
            break;
      case TS_DONE:
            print(wifi_CommandSuccessCount);
            print(F("/"));
            print(wifi_CommandFailureCount);  
            print(F("  :  "));
            println(thingSpeakConsecutiveFailureCount);            
            updateApiCallStatusOnLCD();
            state = TS_START;
            break;
      default:
            playTone(TONE_SINGLE, 0, 400, TONE_ARG_EOL); 
    
  }
}  

void handleFailures(){
  if (wifi_CommandFailureCount > 0)
    thingSpeakConsecutiveFailureCount ++;  
  else
    thingSpeakConsecutiveFailureCount  = 0;
  

  if (thingSpeakConsecutiveFailureCount > 0){
      if (thingSpeakConsecutiveFailureCount % MAX_FAILURES_TO_WARN == 0 )          playTone(TONE_REPEAT, 4, 180,80, TONE_ARG_EOL);
      if (thingSpeakConsecutiveFailureCount % MAX_FAILURES_TO_RECONNECT == 0 )     disconnectWifi();
  }
}

void buildDataToSend(){  
  char tmp[15];
  char data[100];  //ensure that array size is enough to fit all fields data
  data[0] = '\0';
  
  itoa (waterLevelSignalValue, tmp, 10);                        strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  dtostrf(waterLevelSignalValueEMA, 1, 1, tmp);                 strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  dtostrf(waterLevelPercentage, 1, 1, tmp);                     strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  dtostrf(waterLevelPercentageEMA, 1, 1, tmp);                  strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  itoa (thingSpeakConsecutiveFailureCount, tmp, 10);            strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  itoa (waterLevelSignalThresholdJumpCount_Small, tmp, 10);     strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  itoa (waterLevelSignalThresholdJumpCount_Large, tmp, 10);     strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  dtostrf(getUptimeInMinutes(), 1, 1, tmp);                     strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  strcpy(tmp, waterTankFillingInProgress ? "1" : "0");          strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  itoa (waterTankFillCounter, tmp, 10);                         strcat (data, tmp);   strcat (data, THINGSPEAK_FIELD_SEPARATOR);
  

  strcpy(dc, "GET /update?api_key=");
  strcat(dc, THINGSPEAK_WRITE_API_KEY);
  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_STRING_DATA);
  strcat(dc, "=");  
  strcat(dc, data);      

  //println(dc, true);
}


void updateApiCallStatusOnLCD(){
  if (apiStatusIndex > 19){
      apiStatusIndex = 19;
      //shift array elements to left by 1      
      for (int i = 1; i < 20 ; i++)  apiStatus[i-1] = apiStatus[i];          
  }
  
  //if (tmpMsgWifiStatus.length() > 19){    
  //  tmpMsgWifiStatus.remove (0,1);
  //}
  
  if (thingSpeakConsecutiveFailureCount > 0){        
    apiStatus[apiStatusIndex] = '0';
  }
  else{        
    apiStatus[apiStatusIndex] = '1';
  }
  apiStatusIndex ++;  

  lcdShowApiCallStatus(apiStatus);
}
