//make sure to keep command length < 32 chars, don't use whitespaces
#define BT_CMD_TOGGLE_RAW_SIGNAL_DEBUG        "<toggle_debug_raw_signal>"
#define BT_CMD_TOGGLE_SERIAL_LOGGING          "<toggle_logs>"
#define BT_CMD_TONE_TEST                      "<tone_test>"
#define BT_CMD_REBOOT                         "<reboot>"

#define MAX_COMMAND_LENGTH                    32


void handleBluetoothCommands(){          
  
    char* cmd = readBluetoothCommand();  

    if (strcmp(cmd, BT_CMD_TOGGLE_RAW_SIGNAL_DEBUG) == 0){
        playCommandTone();
        toggleDebugModeRawSignal();
    }
    else if (strcmp(cmd, BT_CMD_TOGGLE_SERIAL_LOGGING) == 0){
        playCommandTone();
        toggleSerialDebugMessagingEnabled();
    }
    else if (strcmp(cmd, BT_CMD_TONE_TEST) == 0){
        playCommandTone();
        playToneTest();
    }
    else if (strcmp(cmd, BT_CMD_REBOOT) == 0){
        playCommandTone();
        reboot();
    }    
    else if (strlen(cmd) > 1){
        playTone(TONE_SINGLE, 0, 700, TONE_ARG_EOL); 
        lcdTransientMessage(F("Invalid BT Command"), 2000);
        print(F("Invalid BT Command: "));     
        println(cmd);          
    }
}

void playCommandTone(){  
    playTone(TONE_SINGLE, 0, 100,100,250, TONE_ARG_EOL); 
    delay(800);
}


char* readBluetoothCommand(){
  static char cmd[MAX_COMMAND_LENGTH];
  char byteRead;

  cmd[0] = '\0';
  
  int idx = 0;
  while (BTserial.available() && idx < MAX_COMMAND_LENGTH - 1)  {
      byteRead = BTserial.read();      
      if (byteRead != ' ' && byteRead != '\t' && byteRead != '\n' && byteRead != '\r') {          
        cmd[idx] = byteRead;
        cmd[idx+1] = '\0';  
        idx++;
      }
  }
  
  return cmd;  
}
