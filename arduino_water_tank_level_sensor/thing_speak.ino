#define  MAX_FAILURES_TO_WARN         60
#define  MAX_FAILURES_TO_RECONNECT    15

String THINGSPEAK_HOST = "api.thingspeak.com";
String THINGSPEAK_PORT = "80";
#define THINGSPEAK_API_KEY  "MJQYJZEIGPK7BOWE"   
#define THINGSPEAK_FIELD_WATER_LEVEL_SIGNAL             "field1"
#define THINGSPEAK_FIELD_WATER_LEVEL_PERCENTAGE         "field2"
#define THINGSPEAK_FIELD_WATER_LEVEL_SIGNAL_EMA         "field3"
#define THINGSPEAK_FIELD_WATER_LEVEL_PERCENTAGE_EMA     "field4"
#define THINGSPEAK_FIELD_UPTIME                         "field5"
#define THINGSPEAK_FIELD_API_CALL_FAILURES              "field6"
#define THINGSPEAK_FIELD_SIGNAL_THRESHOLD_JUMPS_SMALL   "field7"
#define THINGSPEAK_FIELD_SIGNAL_THRESHOLD_JUMPS_LARGE   "field8"


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
   //Not using String data type here, causing memory issues. 
  char tmp[15];
  strcpy(dc, "GET /update?api_key=");
  strcat(dc, THINGSPEAK_API_KEY);
  
  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_WATER_LEVEL_SIGNAL);
  strcat(dc, "=");
  itoa (waterLevelSignalValue, tmp, 10);
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_WATER_LEVEL_PERCENTAGE);
  strcat(dc, "=");
  itoa (waterLevelPercentage, tmp, 10);
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_API_CALL_FAILURES);
  strcat(dc, "=");
  itoa (thingSpeakConsecutiveFailureCount, tmp, 10);
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_SIGNAL_THRESHOLD_JUMPS_SMALL);
  strcat(dc, "=");
  itoa (waterLevelSignalThresholdJumpCount_Small, tmp, 10);
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_SIGNAL_THRESHOLD_JUMPS_LARGE);
  strcat(dc, "=");
  itoa (waterLevelSignalThresholdJumpCount_Large, tmp, 10);
  strcat(dc, tmp);  

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_WATER_LEVEL_SIGNAL_EMA);
  strcat(dc, "=");
  dtostrf(waterLevelSignalValueEMA, 1, 1, tmp);  
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_WATER_LEVEL_PERCENTAGE_EMA);
  strcat(dc, "=");
  dtostrf(waterLevelPercentageEMA, 1, 1, tmp);  
  strcat(dc, tmp);

  strcat(dc, "&");
  strcat(dc, THINGSPEAK_FIELD_UPTIME);
  strcat(dc, "=");
  dtostrf(getUptimeInMinutes(), 1, 1, tmp);  
  strcat(dc, tmp); 

  //println(dc);
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
