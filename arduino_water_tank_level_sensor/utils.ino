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
      playLoudTone(100,100,200);            
      delay(1000);
  }
}
