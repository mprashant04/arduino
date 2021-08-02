//Calibrate these values as per pot signal value at min & max water level, notes maintained in google drive doc. Keep 5-8 points margin above acrual full/empty pot readings
#define   WATER_SIGNAL_MIN                            270  
#define   WATER_SIGNAL_MAX                            730

#define   WATER_ALERT_LEVEL_H1                        80      //main tank about to get full
#define   WATER_ALERT_LEVEL_H2                        95      //solar tank full, overfill detected in main tank (todo- increase above 100% after joining solar tank to main tank)
#define   WATER_ALERT_LEVEL_L1                        35      //main tank about to go empty
#define   WATER_ALERT_LEVEL_L2                        20      //main tank about to go empty
#define   WATER_ALERT_LEVEL_CROSS_THRESHOLD           1
#define   WATER_ALERT_TONE_COUNT                      180

#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_SMALL     5
#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_LARGE     15

#define   WATER_LEVEL_SAMPLES_COUNT                         20
#define   WATER_LEVEL_READING_INTERVAL_MS                   1000

#define   READING_FREQUENCY                      1000   //in milli sec
#define   READING_FREQUENCY_SIGNAL_DEBUG         300    //in milli sec


// https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
// EMA alpha factor, between 0 and 1. Finetune as needed. Lower the value, more samples will be used for averaging, i.e. slower response
#define   EMA_a                         0.05

unsigned long levelAlertStartedOn = UNSIGNED_LONG_MAX;


//*****************************************************************************
// Called from timer, ensure to keep simple logic
// avoid String operations, delays, times, etc...
// Update only volatile global variables...  delay() does not work here
// 
// Using timer to ensure signal reading frequency is not affected by random 
// delays due to wifi, BT, etc.
//*****************************************************************************
void timerHandler_waterLevelRead(){
  static int lastValue = -1;
  static unsigned long sum = 0;
  static int sample_count = 0;

  if (!startWaterReading)  return;  //device still initiating, wait....

  
  
  //------ reading sampling stage -----------------------------------------------  
  if (sample_count < WATER_LEVEL_SAMPLES_COUNT){            
      if (sample_count >= 0){
          if (isDebugModeRawSignal())
              sum = analogRead(A0) * WATER_LEVEL_SAMPLES_COUNT;  //read raw signal, no sampling and averating
          else
              sum += analogRead(A0);
      }
      else{
          //wait state...
      }
          
      //delay (10);  // delay does not work in this timer function, hence using state machine like logic
      
      sample_count++;
      return;
  }

  //------ reading sampling done  -----------------------------------------------
  waterLevelSignalValue = sum / WATER_LEVEL_SAMPLES_COUNT;
  if (waterLevelSignalValueEMA == -1)
    waterLevelSignalValueEMA = waterLevelSignalValue;  //first time initiation
  else
    waterLevelSignalValueEMA = (EMA_a * waterLevelSignalValue) + ((1.0 - EMA_a) * waterLevelSignalValueEMA);  

      
  waterLevelPercentage    = 100.0 * (waterLevelSignalValue    - WATER_SIGNAL_MIN) / (WATER_SIGNAL_MAX - WATER_SIGNAL_MIN);
  waterLevelPercentageEMA = 100.0 * (waterLevelSignalValueEMA - WATER_SIGNAL_MIN) / ((WATER_SIGNAL_MAX - WATER_SIGNAL_MIN) * 1.0);


  //round to single decimal place
  waterLevelSignalValueEMA = round (waterLevelSignalValueEMA * 10.0) / 10.0;
  waterLevelPercentageEMA  = round (waterLevelPercentageEMA  * 10.0) / 10.0;

  sum = 0;
  sample_count = 0 - (( (isDebugModeRawSignal() ? READING_FREQUENCY_SIGNAL_DEBUG : READING_FREQUENCY) / TIMER_FREQUENCY) - WATER_LEVEL_SAMPLES_COUNT);
      
  
  checkIfDeltaThresholdJumped(lastValue);
  checkIfSafeRangeCrossed();
  checkTankLevelAlerts();

  lastValue = waterLevelSignalValue;
  waterLevelReadingCount++;
  logLevels();  
}

void logLevels(){
  if (!isSerialDebugMessagingEnabled()) return;
  
  print(F("\n=============== "));
  print(waterLevelPercentageEMA ); 
  print(F("/")); 
  print(waterLevelPercentage ); 
  print(F("%")); 
  print(F("    ")); 
  print(F("(")); 
  print(waterLevelSignalValueEMA); 
  print(F("/")); 
  print(waterLevelSignalValue); 
  print(F(")")); 
  print(F("        ")); 
  print(waterLevelSignalThresholdJumpCount_Large); 
  print(F("/")); 
  print(waterLevelSignalThresholdJumpCount_Small);   
  println(F(" "));  
}

void checkIfDeltaThresholdJumped(int lastValue){
  if (lastValue >= 0){    
      if (abs(lastValue - waterLevelSignalValue) >= WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_LARGE){    
          waterLevelSignalThresholdJumpCount_Large ++;
          if (isDebugModeRawSignal())
            playTone(TONE_SINGLE, 0, 150, TONE_ARG_EOL);      //todo move tones to main looop() ??
          else
            playTone(TONE_REPEAT, 4, 400, 150, TONE_ARG_EOL);
      }
      else if (abs(lastValue - waterLevelSignalValue) >= WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_SMALL){    
          waterLevelSignalThresholdJumpCount_Small ++;          
      }      
  }  
}

void checkIfSafeRangeCrossed(){
  //TODO - alert when pot value jumps beyong min/max range with risk of physically stressing pot
  //       pot should not be stressed with weight on either end
  //  in addition to audio alert, create java alert in FNO service??
}

//check if any manual config errors
void validateWaterAlertLevelDefinitions(){
    bool ok = true;
    if (WATER_ALERT_LEVEL_H2 - WATER_ALERT_LEVEL_CROSS_THRESHOLD <= WATER_ALERT_LEVEL_H1)     ok = false;
    if (WATER_ALERT_LEVEL_L2 + WATER_ALERT_LEVEL_CROSS_THRESHOLD >= WATER_ALERT_LEVEL_L1)     ok = false;
    if (!ok)        
        haltProgram(F("Conf Error!!"));    
}


void checkTankLevelAlerts(){
  static bool wasBelowH1 = false;
  static bool wasBelowH2 = false;
  static bool wasAboveL1 = false;
  static bool wasAboveL2 = false;
  static char alertType  = 0;

  //TODO - skip alert logic for 1 min after startup??
  
  float curLevel = waterLevelPercentageEMA;   //todo remove variable, durectly use level value to save memory  

  if (!wasBelowH1 && curLevel < WATER_ALERT_LEVEL_H1 - WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasBelowH1 = true;  alertType = 0;  }
  if (!wasBelowH2 && curLevel < WATER_ALERT_LEVEL_H2 - WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasBelowH2 = true;  alertType = 0;  }
  if (!wasAboveL1 && curLevel > WATER_ALERT_LEVEL_L1 + WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasAboveL1 = true;  alertType = 0;  }
  if (!wasAboveL2 && curLevel > WATER_ALERT_LEVEL_L2 + WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasAboveL2 = true;  alertType = 0;  }

  //folliwing if sequence is imp
  if (wasBelowH1 && curLevel > WATER_ALERT_LEVEL_H1) { wasBelowH1 = false;  alertType = 1;  }
  if (wasBelowH2 && curLevel > WATER_ALERT_LEVEL_H2) { wasBelowH2 = false;  alertType = 2;  }
  if (wasAboveL1 && curLevel < WATER_ALERT_LEVEL_L1) { wasAboveL1 = false;  alertType = -1; }
  if (wasAboveL2 && curLevel < WATER_ALERT_LEVEL_L1) { wasAboveL2 = false;  alertType = -2; }

  waterLevelAlertType = alertType;
}

void playWaterLevelAlertIfAny(){
  static char lastAlertType = -99;

  if (lastAlertType != waterLevelAlertType){
      lastAlertType = waterLevelAlertType;
      
      switch (lastAlertType){
          case 0:     //do nothing
              break;
              
          case  1:    //H1 alert              
          case -1:    //L1 alert              
              playTone(TONE_REPEAT_BLOCKING, WATER_ALERT_TONE_COUNT, 100, 900, TONE_ARG_EOL);
              break;
              
          case  2:    //H2 alert              
          case -2:    //L2 alert              
              playTone(TONE_REPEAT_BLOCKING, WATER_ALERT_TONE_COUNT, 500, 500, TONE_ARG_EOL);
              break;        
      }    
  }    
}



boolean isWaterReadingUpdated(boolean resetStatusIfChanged){
  static char lastWaterLevelReadingCount = waterLevelReadingCount;
  if (lastWaterLevelReadingCount != waterLevelReadingCount){
      if (resetStatusIfChanged)
          lastWaterLevelReadingCount = waterLevelReadingCount;
      return true;
  }
  return false;
}
