
void playToneTest(){
    int j = 15;
    delay(4000);
    while (j-- > 0){

      int del = 200;

      playTone(TONE_REPEAT, 8, del,del, TONE_ARG_EOL);      
      delay (7000);

      for (int i = 0; i<8; i++){
        digitalWrite(BUZZER_PIN, HIGH);
        delay (del);
        digitalWrite(BUZZER_PIN, LOW);
        delay (del);
      }
      delay (4000);
  }  
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
    
  //toneStartedOn = millis();
  int idx = 0;   
  
  tones[idx++] = 1; //millis();
  //int num = va_arg(arguments, int);
  while (num > 0){      
      if (idx >= TONES_ARR_SIZE){
          //TODO halt("ERR_TMR_9");
      }      
  
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
        pinMode(BUZZER_LARGE_PIN, OUTPUT);        
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
          digitalWrite(BUZZER_LARGE_PIN, getBuzzerStatus(idx));
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
