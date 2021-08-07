// *************************************************************************
//      make sure to keep timer logic simple, avoid any 
//      complex things like String operations, sounds, delays, etc... 
// *************************************************************************

void timerFunction(){
    timerHandler_waterLevelRead();
    timerHandler_buzzer();
}


void initTimers(){
  ITimer1.init();
  if (!ITimer1.attachInterruptInterval(TIMER_FREQUENCY, timerFunction ))
      haltProgram(F("Timer Error!!")); //some issue in initiating timer, halt program...    

  delay(500);
}
