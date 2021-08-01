#define BUZZER_PIN                        12
#define BUZZER_LARGE_PIN                  9


#define TONE_SINGLE             's'    
#define TONE_SINGLE_BLOCKING    'S'    //other tone requests will be discarded till this one is complete
#define TONE_REPEAT             'r'
#define TONE_REPEAT_BLOCKING    'R'    //other tone requests will be discarded till this one is complete


#define TONE_ARG_EOL      -1
#define TONES_ARR_SIZE    20
volatile int      tones[TONES_ARR_SIZE];
volatile char     toneType;
volatile boolean  tonePlayInProgress = false;
volatile char     tonesUpdateCount = 0;
volatile int      tonesRepeatCount = 0;
