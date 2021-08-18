#define WIFI_RESET_PIN      4
#define AT_CMD_RST          "AT+RST"
#define AT_CMD_CWMODE       "AT+CWMODE=1"
#define AT_CMD_CWGAP        "AT+CWJAP=\"home06_3\",\"trGsdu457SWydgbdyjYUfj758UF\""   //add here wifi SSID and password


#define MSG_WIFI_RESET      "  Wifi Reset.."

boolean wifiConnected = false;

void wifiInit(){
  pinMode(WIFI_RESET_PIN, OUTPUT);
}

void connectWifi(){  
  if (wifiConnected == false){
    wifiConnected = true; 

    //Many times on startup wifi does not connect till module is hard reset. Using ocrocoupler for reset
    hardResetWifi();    
    delay(1000);
    
    sendWifiCommand(AT_CMD_RST,    5,  AT_OK);
    sendWifiCommand(AT_CMD_CWMODE, 5,  AT_OK);
    sendWifiCommand(AT_CMD_CWGAP,  20, AT_OK);  
    delay(500);
  }
}

void disconnectWifi(){
  wifiConnected = false;  //reset status, so that next time connectWifi() will re-attempt to connect
}


void hardResetWifi(){
  lcdTransientMessage(MSG_WIFI_RESET);
  println(MSG_WIFI_RESET);
  
  //Single reset is enough i think, but resetting multiple times just to be safe. 
  //After each reset, blue led blinks on wifi module
  for (int i = 0; i < 3; i++){  
    digitalWrite(WIFI_RESET_PIN, HIGH);
    delay(500);
    digitalWrite(WIFI_RESET_PIN, LOW);
    delay(500);
  }    
  
  lcdTransientMessageClear();  
}


void resetWifiCounters(){
   wifi_CommandSuccessCount=0;   
   wifi_CommandFailureCount=0;
}

void sendWifiCommand(String command, int maxRetries, char readReplay[]) {
  boolean found = false; 
  
  //print(thingSpeak_CountTrueCommand);
  print(F(". "));
  print(command);  
  
  while(maxRetries > 0 && !found)
  {
    esp8266.println(command);
    if(esp8266.find(readReplay))   found = true;     
    maxRetries--;
  }
  
  if(found)  
    wifi_CommandSuccessCount++;    
  else
    wifi_CommandFailureCount++;      


  println( found ? F(" OK")  : F("        **** Fail **** "));
}
