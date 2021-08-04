#define BT_CMD_TOGGLE_RAW_SIGNAL_DEBUG        "<toggle_debug_raw_signal>"
#define BT_CMD_TOGGLE_SERIAL_LOGGING          "<toggle_logs>"
#define BT_CMD_TONE_TEST                      "<tone_test>"
#define BT_CMD_REBOOT                         "<reboot>"


void handleBluetoothCommands(){        
  //TODO disabled to diagnore arduino reboot issue, avoid using String type
/*    String cmd = readBluetoothCommand();            

    if (cmd == BT_CMD_TOGGLE_RAW_SIGNAL_DEBUG){
        playCommandTone();
        toggleDebugModeRawSignal();
    }
    else if (cmd == BT_CMD_TOGGLE_SERIAL_LOGGING){
        playCommandTone();
        toggleSerialDebugMessagingEnabled();
    }
    else if (cmd == BT_CMD_TONE_TEST){
        playCommandTone();
        playToneTest();
    }
    else if (cmd == BT_CMD_REBOOT){
        playCommandTone();
        reboot();
    }
    else if (cmd.length() > 0){        
        playTone(TONE_SINGLE, 0, 700, TONE_ARG_EOL); 
        lcdTransientMessage(F("Invalid BT Command"), 2000);
        print(F("Invalid BT Command: "));     
        println(cmd);     
    }
*/
}

void playCommandTone(){  
    playTone(TONE_SINGLE, 0, 100,100,250, TONE_ARG_EOL); 
    delay(800);
}

/*
String readBluetoothCommand(){         
  String s = "";
  if (BTserial.available()){
    s = BTserial.readStringUntil('\n');    
  } 
  s.trim();    
  return s;
}
*/
