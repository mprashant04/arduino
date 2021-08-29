#define   WATER_SENSOR_PIN                                  A2

//Calibrate these values as per pot signal value at min & max water level, notes maintained in google drive doc. Keep 5-8 points margin above acrual full/empty pot readings
// Signal value set ~750 on extreme top when rope touch tank inside top and max out. I.e. value won't go beyond that. (29 aug 2021)
#define   WATER_SIGNAL_MIN                                  260
#define   WATER_SIGNAL_MAX                                  720

#define   WATER_ALERT_LEVEL_H1                              90      //main tank about to get full
#define   WATER_ALERT_LEVEL_H2                              102     //solar tank full, overfill detected in main tank
#define   WATER_ALERT_LEVEL_L1                              30      //main tank about to go empty
#define   WATER_ALERT_LEVEL_L2                              15      //main tank about to go empty
#define   WATER_ALERT_LEVEL_CROSS_THRESHOLD                 1
#define   WATER_ALERT_TONE_COUNT                            300     //5 minutes, assuming each tone duration = 1 sec in code below

#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_SMALL     5
#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_LARGE     15

#define   WATER_LEVEL_SAMPLES_COUNT                         20
#define   WATER_LEVEL_READING_INTERVAL_MS                   1000

#define   READING_FREQUENCY                                 1000   //in milli sec
#define   READING_FREQUENCY_SIGNAL_DEBUG                    300    //in milli sec

#define   TANK_FILL_CHECK_READING_FREQUENCY                 15     //check every Xth reading taken, i.e. every 10 sec if main reading frequency is 1 sec and this var value is 10
#define   TANK_FILL_CHECKS_UP             					        5
#define   TANK_FILL_CHECKS_DOWN           					        12


// https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
// EMA alpha factor, between 0 and 1. Finetune as needed. Lower the value, more samples will be used for averaging, i.e. slower response
#define   EMA_A_WHILE_FILLING                             0.099  //use faster sampling while tank is being filled since filling singla change rate is faster than no-filling signal change rate, and we need to give on time alert when tank about to get full
#define   EMA_A_WHILE_NOT_FILLING                         0.010

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
  static unsigned long sum = 0;
  static int sample_count = 0;

  if (!startWaterReading)  return;  //device still initiating, wait....

  
  
  //------ reading sampling stage -----------------------------------------------  
  if (sample_count < WATER_LEVEL_SAMPLES_COUNT){            
      if (sample_count >= 0){
          if (isDebugModeRawSignal())
              sum = analogRead(WATER_SENSOR_PIN) * WATER_LEVEL_SAMPLES_COUNT;  //read raw signal, no sampling and averating
          else
              sum += analogRead(WATER_SENSOR_PIN);
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
  else{
    float EMA_a = roundNumber(waterTankFillingInProgress ? EMA_A_WHILE_FILLING : EMA_A_WHILE_NOT_FILLING, 3);
    waterLevelSignalValueEMA = (EMA_a * waterLevelSignalValue) + ((1.0 - EMA_a) * waterLevelSignalValueEMA);      
    waterLevelSignalValueEMA = roundNumber (waterLevelSignalValueEMA, 1);
  }
      
  waterLevelPercentage    = 100.0 * (waterLevelSignalValue    - WATER_SIGNAL_MIN) / ((WATER_SIGNAL_MAX - WATER_SIGNAL_MIN) * 1.0);
  waterLevelPercentageEMA = 100.0 * (waterLevelSignalValueEMA - WATER_SIGNAL_MIN) / ((WATER_SIGNAL_MAX - WATER_SIGNAL_MIN) * 1.0);

  waterLevelPercentage     = roundNumber (waterLevelPercentage , 1);
  waterLevelPercentageEMA  = roundNumber (waterLevelPercentageEMA , 1);
  

  sum = 0;
  sample_count = 0 - (( (isDebugModeRawSignal() ? READING_FREQUENCY_SIGNAL_DEBUG : READING_FREQUENCY) / TIMER_FREQUENCY) - WATER_LEVEL_SAMPLES_COUNT); //making negative to ensure reading sampling at every defined interval only
      
  
  checkIfDeltaThresholdJumped();
  checkIfSafeRangeCrossed();
  checkTankLevelAlerts();
  checkIfWaterFillingStarted();

  
  waterLevelReadingCount++;
  logLevels();  
}

void checkIfWaterFillingStarted(){
    static float lastValue = -1;    
    
    if (waterLevelReadingCount % TANK_FILL_CHECK_READING_FREQUENCY != 0) return;
    if (getUptimeInMinutes() < 3.0) return;  //let signal stabilize in first few minutes after boot

    // when waterTankFillingInProgress=true, ema is made faster, so using rounded value to timely detect filling stop
    float signal = waterTankFillingInProgress ? round(waterLevelSignalValueEMA) : waterLevelSignalValueEMA;  
    
    if (lastValue < 0) lastValue = signal;   //first time initiation        

    if (signal > lastValue + 0){
        waterTankFillCounter++;
        if (waterTankFillCounter >= TANK_FILL_CHECKS_UP && !waterTankFillingInProgress){  //water tank filling has started....
            waterTankFillingInProgress = true;
            waterTankFillCounter = TANK_FILL_CHECKS_DOWN;
        }        
    }
    else{
        waterTankFillCounter--;
        if (waterTankFillCounter <= 0){  //water tank filling stopped
            waterTankFillingInProgress = false;            
        }
    }

    if (waterTankFillCounter > TANK_FILL_CHECKS_DOWN)  waterTankFillCounter = TANK_FILL_CHECKS_DOWN;
    if (waterTankFillCounter < 0)                      waterTankFillCounter = 0;
        

    print (F("*** rc="));
    print (waterLevelReadingCount);
    print (F(", last="));
    print (lastValue);
    print (F(", sig="));
    print (signal);
    print (F(", fc="));
    print (waterTankFillCounter);
    print (F(", filling="));
    print (waterTankFillingInProgress);    
    println (F(" "));
    
    lastValue = signal;
}



void checkIfDeltaThresholdJumped(){
  static int lastValue = -1;
  
  if (lastValue >= 0){
      if (abs(lastValue - waterLevelSignalValue) >= WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_LARGE){    
          waterLevelSignalThresholdJumpCount_Large ++;
          
          // can NOT play here in timer, will  hang arduino, move tones play task to main looop().
          /*if (isDebugModeRawSignal())
            playTone(TONE_SINGLE, 0, 150, TONE_ARG_EOL);      
          else
            playTone(TONE_REPEAT, 4, 400, 150, TONE_ARG_EOL);
          */
          
      }
      else if (abs(lastValue - waterLevelSignalValue) >= WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_SMALL){    
          waterLevelSignalThresholdJumpCount_Small ++;          
      }      
  }

  lastValue = waterLevelSignalValue;
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

  if (!wasBelowH1 && waterLevelPercentageEMA < WATER_ALERT_LEVEL_H1 - WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasBelowH1 = true;  alertType = 0;  }
  if (!wasBelowH2 && waterLevelPercentageEMA < WATER_ALERT_LEVEL_H2 - WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasBelowH2 = true;  alertType = 0;  }
  if (!wasAboveL1 && waterLevelPercentageEMA > WATER_ALERT_LEVEL_L1 + WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasAboveL1 = true;  alertType = 0;  }
  if (!wasAboveL2 && waterLevelPercentageEMA > WATER_ALERT_LEVEL_L2 + WATER_ALERT_LEVEL_CROSS_THRESHOLD) {  wasAboveL2 = true;  alertType = 0;  }

  //folliwing if sequence is imp
  if (wasBelowH1 && waterLevelPercentageEMA > WATER_ALERT_LEVEL_H1) { wasBelowH1 = false;  alertType = 1;  }
  if (wasBelowH2 && waterLevelPercentageEMA > WATER_ALERT_LEVEL_H2) { wasBelowH2 = false;  alertType = 2;  }
  if (wasAboveL1 && waterLevelPercentageEMA < WATER_ALERT_LEVEL_L1) { wasAboveL1 = false;  alertType = -1; }
  if (wasAboveL2 && waterLevelPercentageEMA < WATER_ALERT_LEVEL_L2) { wasAboveL2 = false;  alertType = -2; }

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

void playWaterTankFillingStartedAlert(){
    static boolean lastStatus = false;

    if (lastStatus != waterTankFillingInProgress){
        lastStatus = waterTankFillingInProgress;
        
        if (waterTankFillingInProgress) playTone(TONE_REPEAT, 10, 300,150,300,150,80,80,80,9000, TONE_ARG_EOL);
        else  playTone(TONE_REPEAT, 2, 80,80,250,500,80,80,250,5000, TONE_ARG_EOL);
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



void logLevels(){
  if (!isSerialDebugMessagingEnabled()) return;
  
  print(F("======= "));
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
  print(F("    ")); 
  print(waterLevelSignalThresholdJumpCount_Large); 
  print(F("/")); 
  print(waterLevelSignalThresholdJumpCount_Small);   
  print(F("/")); 
  print(waterTankFillingInProgress);  
  print(F(", ")); 
  print(getUptimeInMinutes());   
  println(F("min"));  
}
