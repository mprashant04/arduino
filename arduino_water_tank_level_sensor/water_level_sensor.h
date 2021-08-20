//updated from timer functions, hence declaring volatile
volatile float    waterLevelPercentage      = 0;
volatile int      waterLevelSignalValue     = 0;
volatile float    waterLevelPercentageEMA   = -1;
volatile float    waterLevelSignalValueEMA  = -1;
volatile int      waterLevelSignalThresholdJumpCount_Small = 0;
volatile int      waterLevelSignalThresholdJumpCount_Large = 0;
volatile char     waterLevelReadingCount = 0;  //just used for checking in main thread if reading is taken, does not matter if value rollovers after data type overflow
volatile boolean  waterTankFillingInProgress = false;
volatile char     waterTankFillCounter = 0;


volatile char waterLevelAlertType = 0;

boolean startWaterReading = false;
