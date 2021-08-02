float getUptimeInMinutes(){
  return millis() / 1000.0 / 60.0;
}

float getUptimeInHours(){
  return getUptimeInMinutes() / 60.0;
}

void reboot(){
  //dirty trick to cause stack overflow to reboot arduino
  //UPDATE: did not work, makes arduino unstable many times, think alternate way  
  //    String data  = "some garbage";
  //    reboot();  

  //option-2
  //update - this does not reset chip, just moves program execution to start. Have seen some 
  // wierd behavior like repeated restarts, etc with this approach. So not using it
  //resetFunc();
}

String centerAlign(String msg, int width){
    int spaces = width - msg.length();
    if (spaces > 0) {
        for (int i = 0; i < spaces / 2; i++)  msg = " " + msg;
        while (msg.length() < width)  msg = msg + " ";        
    }
    return msg;    
}

void haltProgram(String lcdMessage){  
  lcdTransientMessage(lcdMessage);
  while (true){
      playTone(TONE_SINGLE, 0, 100,100,200, TONE_ARG_EOL);       
      delay(1000);
  }
}
