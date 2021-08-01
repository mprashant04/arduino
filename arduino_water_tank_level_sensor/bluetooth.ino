#define BT_CMD_TOGGLE_RAW_SIGNAL_DEBUG        "<toggle_debug_raw_signal>"
#define BT_CMD_TOGGLE_SERIAL_LOGGING          "<toggle_logs>"
#define BT_CMD_TONE_TEST                      "<tone_test>"

#define MSG_INVALID_BT_COMAND                 "Invalid BT Command  "

void handleBluetoothCommands(){        
    String cmd = readBluetoothCommand();            

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
    else if (cmd.length() > 0){        
        playTone(TONE_SINGLE, 0, 700, TONE_ARG_EOL); 
        lcdTransientMessage(MSG_INVALID_BT_COMAND, 2000);
        println(MSG_INVALID_BT_COMAND + cmd);     
    }
}

void playCommandTone(){  
    playTone(TONE_SINGLE, 0, 100,100,250, TONE_ARG_EOL); 
}

String readBluetoothCommand(){         
  String s = "";
  if (BTserial.available()){
    s = BTserial.readStringUntil('\n');    
  } 
  s.trim();    
  return s;
}
