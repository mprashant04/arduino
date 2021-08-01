// *************************************************************************
//      make sure to keep timer logic simple, avoid any 
//      complex things like String operations, sounds, delays, etc... 
// *************************************************************************

void timerHandler()
{
  static bool toggle1 = false;
  static bool started = false;
  
  waterLevelRead();  

//  if (!started)
//  {
//    started = true;
//    pinMode(outputPin, OUTPUT);
//  }
//  
//  digitalWrite(outputPin, toggle1);
//  toggle1 = !toggle1;

}

void initTimers(){
  ITimer1.init();
  if (!ITimer1.attachInterruptInterval(1000, timerHandler))      
      haltProgram("ERR_TIMER1"); //some issue in initiating timer, halt program...  
  
}
