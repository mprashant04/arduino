//Calibrate these values as per pot signal value at min & max water level, notes maintained in google drive doc. Keep 5-8 points margin above acrual full/empty pot readings
#define   WATER_SIGNAL_MIN                            270  
#define   WATER_SIGNAL_MAX                            730

#define   WATER_LEVEL_FULL_1_ALERT_PERCENTAGE         75      //main tank about to get full
#define   WATER_LEVEL_FULL_2_ALERT_PERCENTAGE         94      //solar tank full, overfill detected in main tank (todo- increase above 100% after joining solar tank to main tank)
#define   WATER_LEVEL_EMPTY_1_ALERT_PERCENTAGE        29      //main tank about to go empty
#define   WATER_LEVEL_EMPTY_2_ALERT_PERCENTAGE        15      //main tank about to go empty
#define   WATER_LEVEL_ALERT_CROSS_THRESHOLD           1
#define   WATER_LEVEL_ALERT_PLAY_SECONDS              120     //no. of seconds to play alert tone on level cross

#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_SMALL     5
#define   WATER_LEVEL_SIGNAL_JUMP_ALERT_THRESHOLD_LARGE     15

#define   WATER_LEVEL_SAMPLES_COUNT                         20
#define   WATER_LEVEL_READING_INTERVAL_MS                   1000

#define   ALERT_TYPE_NA       "-"
#define   ALERT_TYPE_H1       "H1"
#define   ALERT_TYPE_H2       "H2"
#define   ALERT_TYPE_L1       "L1"
#define   ALERT_TYPE_L2       "L2"

// https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/
// EMA alpha factor, between 0 and 1. Finetune as needed. Lower the value, more samples will be used for averaging, i.e. slower response
#define   EMA_a                         0.05

unsigned long levelAlertStartedOn = UNSIGNED_LONG_MAX;

bool wasBelowFullLevel1 = false;
bool wasBelowFullLevel2 = false;
bool wasAboveEmptyLevel = false;
bool wasAboveEmptyLeve2 = false;

//*****************************************************************************
// Called from timer, ensure to keep simple logic
// avoid String operations, delays, times, etc...
// Update only volatile global variables...
// 
// Using timer to ensure signal reading frequency is not affected by random 
// delays due to wifi, BT, etc.
//*****************************************************************************
void timerHandler_waterLevelRead(){
  static int lastValue = -1;
  int sum = 0;                    
  unsigned char sample_count = 0;

  if (isDebugModeRawSignal()){
    sum = analogRead(A0) * WATER_LEVEL_SAMPLES_COUNT;  //read raw signal, no sampling and averating
  }
  else{
    // take a number of analog samples and add them up
    while (sample_count < WATER_LEVEL_SAMPLES_COUNT) {
        sum += analogRead(A0);     //TODO if value is very different than avg read, ignore it, i.e. noise reduction
        sample_count++;
        delay(10);
    }
  }
    
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
  
  
  checkIfDeltaThresholdJumped(lastValue);
  checkIfSafeRangeCrossed();
  //tankFullEmptyAlert();

  lastValue = waterLevelSignalValue;
  waterLevelReadingCount++;
  logLevels();  

  //TODO: update LCD from here??? to have regular screen updates??
}

void logLevels(){
  print("=", 15);
  print(waterLevelPercentageEMA ); 
  print("/"); 
  print(waterLevelPercentage ); 
  print("%" ); 
  print(" ", 4); 
  print("(" ); 
  print(waterLevelSignalValueEMA); 
  print("/" ); 
  print(waterLevelSignalValue); 
  print(")"); 
  print(" ", 8); 
  print(waterLevelSignalThresholdJumpCount_Large); 
  print("/"); 
  print(waterLevelSignalThresholdJumpCount_Small);   
  println(" ");  
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
void validateAlertLevelDefinitions(){
    bool ok = true;
    if (WATER_LEVEL_FULL_1_ALERT_PERCENTAGE >= WATER_LEVEL_FULL_2_ALERT_PERCENTAGE)     ok = false;
    if (WATER_LEVEL_EMPTY_1_ALERT_PERCENTAGE <= WATER_LEVEL_EMPTY_2_ALERT_PERCENTAGE)   ok = false;
    if (!ok)        
        haltProgram("ERR_12");    
}

void tankFullEmptyAlert(){
  int curLevel = round(waterLevelPercentageEMA);
  String alertType = ALERT_TYPE_NA;

  validateAlertLevelDefinitions();

  if (curLevel <= WATER_LEVEL_FULL_1_ALERT_PERCENTAGE   - WATER_LEVEL_ALERT_CROSS_THRESHOLD)   wasBelowFullLevel1 = true;
  if (curLevel <= WATER_LEVEL_FULL_2_ALERT_PERCENTAGE   - WATER_LEVEL_ALERT_CROSS_THRESHOLD)   wasBelowFullLevel2 = true;
  if (curLevel >= WATER_LEVEL_EMPTY_1_ALERT_PERCENTAGE  + WATER_LEVEL_ALERT_CROSS_THRESHOLD)   wasAboveEmptyLevel = true;
  if (curLevel >= WATER_LEVEL_EMPTY_2_ALERT_PERCENTAGE  + WATER_LEVEL_ALERT_CROSS_THRESHOLD)   wasAboveEmptyLeve2 = true;

  //folliwing if sequence is imp, pay attention
  if (wasBelowFullLevel1 && curLevel >= WATER_LEVEL_FULL_1_ALERT_PERCENTAGE)   alertType = ALERT_TYPE_H1;
  if (wasBelowFullLevel2 && curLevel >= WATER_LEVEL_FULL_2_ALERT_PERCENTAGE)   alertType = ALERT_TYPE_H2;
  if (wasAboveEmptyLevel && curLevel <= WATER_LEVEL_EMPTY_1_ALERT_PERCENTAGE)  alertType = ALERT_TYPE_L1;
  if (wasAboveEmptyLeve2 && curLevel <= WATER_LEVEL_EMPTY_2_ALERT_PERCENTAGE)  alertType = ALERT_TYPE_L2;

  playTankLevelAlert(alertType);
}


void playTankLevelAlert(String type){
  //-------- No alert, reset flags and return --------------------
  if (type == ALERT_TYPE_NA){
      levelAlertStartedOn = 0;
      return;
  }
  //-------------------------------------------------------------

  if (levelAlertStartedOn == 0)  levelAlertStartedOn = millis(); 
  
  if ((millis() - levelAlertStartedOn) > WATER_LEVEL_ALERT_PLAY_SECONDS * 1000) {   //alert played for required time, stop it now
      //TODO stop tones if already playing??
      return;   
  }

  if (type == ALERT_TYPE_H1){
      //playLoudTone(100); 
  }
  else if (type == ALERT_TYPE_H1){
      //playLoudTone(100,50,100); 
  }
  //TODO pending for low
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
