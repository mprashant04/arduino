// *************************************************************************
//      make sure to keep timer logic simple, avoid any 
//      complex things like String operations, sounds, delays, etc... 
// *************************************************************************

void initTimers(){
  ITimer1.init();
  if (!ITimer1.attachInterruptInterval(1000, timerHandler_waterLevelRead))      
      haltProgram("ERR_TIMER1"); //some issue in initiating timer, halt program...  

  ITimer2.init();
  if (!ITimer2.attachInterruptInterval(10, timerHandler_buzzer))
      haltProgram("ERR_TIMER2"); //some issue in initiating timer, halt program...  
  
}
