#define BUZZER_PIN                  9
#define TONES_ARR_SIZE              20

volatile int      tones[TONES_ARR_SIZE];
volatile char     toneType;
volatile boolean  tonePlayInProgress = false;
volatile char     tonesUpdateCount = 0;
volatile int      tonesRepeatCount = 0;

void playToneTest(){      
  
    delay(4000);    
      
    playTone(TONE_REPEAT_BLOCKING, 180, 100,800, TONE_ARG_EOL);      
    delay(5000);
    playTone(TONE_REPEAT, 3, 2000,800, TONE_ARG_EOL); //this will get discarded
  
}


void playTone(char type, int repeatCount, int num, ...){  

  //---------------------------
  // if other blocking tone play in progress, discard current tone request 
  if (   isToneTypeBlocking(toneType)       // last type was blocking type
      && isTimerTonePlayInProgress()        // and it's still being played
      && !isToneTypeBlocking(type)          // and new type not blocking type
     ){      
      return;
  }
  //---------------------------
  
  
  va_list arguments;
  va_start ( arguments, repeatCount );  

  toneType = type;
  tonesRepeatCount = repeatCount;
  for (int i=0; i < TONES_ARR_SIZE; i++) tones[i] = 0;  //clear array
  
  int idx = 0;   
  
  tones[idx++] = 1;   
  while (num > 0){      
      if (idx >= TONES_ARR_SIZE)  haltProgram("ERR_TONE");          
  
      tones[idx] = tones[idx-1] + num;
      idx++;
      num = va_arg(arguments, int); 
  }  

  va_end(arguments); 
  tonesUpdateCount++;  //this is signal to timer to start tone...  
  while(!isTimerTonePlayInProgress()){}  //return after tone play has started
}


//*****************************************************************************
// Called from timer, ensure to keep simple logic
// avoid String operations, delays, times, etc...
// Update only volatile global variables...  delay() does not work here
// 
// Using timer allow non-blocking custom audio alerts
//*****************************************************************************
void timerHandler_buzzer(){    
    static unsigned long toneStartedOn = millis();
    static int idx = 0;
    static int repeatCount = 0;
    static boolean started = false;

    if (!started){
        started = true;
        pinMode(BUZZER_PIN, OUTPUT);        
    }
    
    if (isNewTimerBuzzerRequestReceived()){      
        toneStartedOn = millis();
        idx = 0;
        repeatCount = tonesRepeatCount-1;                
        tonePlayInProgress = true;
    }
    else if (idx > 0 && tones[idx] <= 0 ){
        if (isToneTypeRepeating(toneType) && repeatCount > 0){
            toneStartedOn = millis();
            idx = 0;
            repeatCount--;
        }
        else if (tonePlayInProgress){    
            tonePlayInProgress = false;                          
        }
    }


    if (tonePlayInProgress && tones[idx] > 0 && (tones[idx] + toneStartedOn) < millis()){
        if (tones[idx+1] <= 0 && getBuzzerStatus(idx) == HIGH){
          //skip. Unnecessary last element sent by calling function, preventing continuous ON signal after tone sequence finished                 
        }
        else{
          digitalWrite(BUZZER_PIN, getBuzzerStatus(idx));
        }
        idx++;
    }  
}

boolean isToneTypeBlocking(char type){
  return type == TONE_SINGLE_BLOCKING || type == TONE_REPEAT_BLOCKING;
}

boolean isToneTypeRepeating(char type){
  return type == TONE_REPEAT || type == TONE_REPEAT_BLOCKING;
}

boolean getBuzzerStatus(int idx){
  return idx%2==0 ? HIGH : LOW;
}

boolean isTimerTonePlayInProgress(){
  return tonePlayInProgress;
}

boolean isNewTimerBuzzerRequestReceived(){
    static char lastTonesUpdateCount = tonesUpdateCount;    
        
    if (lastTonesUpdateCount != tonesUpdateCount){
        lastTonesUpdateCount = tonesUpdateCount; 
        return true;
    }
    return false;
}
