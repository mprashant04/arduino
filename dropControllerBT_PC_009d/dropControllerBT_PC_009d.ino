/*
*
* ==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
* [PM] My Customizations
*
* - Changed BT baud rate to make program working with HC-05
* - Disabled solenoid 4,5,6 to make these pins free for other customizations
* - Added sound trigged flash functionality
* - Draining of valve-3 disabled, since this valve is used for second flash
* - many more.......
* ==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
*
*
*
*
*
*
*
*      _                      _____               _                _  _              ____  _______             _____    _____
*     | |                    / ____|             | |              | || |            |  _ \|__   __|   ___     |  __ \  / ____|
*   __| | _ __  ___   _ __  | |      ___   _ __  | |_  _ __  ___  | || |  ___  _ __ | |_) |  | |     ( _ )    | |__) || |
*  / _` || '__|/ _ \ | '_ \ | |     / _ \ | '_ \ | __|| '__|/ _ \ | || | / _ \| '__||  _ <   | |     / _ \/\  |  ___/ | |
* | (_| || |  | (_) || |_) || |____| (_) || | | || |_ | |  | (_) || || ||  __/| |   | |_) |  | |    | (_>  <  | |     | |____
*  \__,_||_|   \___/ | .__/  \_____|\___/ |_| |_| \__||_|   \___/ |_||_| \___||_|   |____/   |_|     \___/\/  |_|      \_____|
*                    | |
*                    |_|
*
* 10.09.2016. Version 009d
* Now works with dropControllerBT and dropControllerPC
* You need to set the device type. See below
*
* dropControllerBT & dropControllerPC
* By Martyn Currey.
* www.dropController.com
*
* Developed in Arduino IDE version 1.6.3
*
* dropcontroller. A device for controlling solenoid valves for creating water drop collisions
* 1 x camera connection
* 1 x flash connection
* 6 x solenoid valve connections
*
* Create up to 9 drops
* Can use up to 6 solenoid valves
* Receives the controls and times from an Android app or a Windows program
*
*
*
*   _____   _
*  |  __ \ (_)
*  | |__) | _  _ __   ___
*  |  ___/ | || '_ \ / __|
*  | |     | || | | |\__ \
*  |_|     |_||_| |_||___/
*
* 02 - Flash-3 (Solenoid-5)               // [OLD]  status LED - waiting
* 03 - Flash-2 (Solenoid-4)               // [OLD]  status LED - active
* 04 - Flash-1                            // [OLD]  solenoid 4 .............. [PM] - Now 'flash-2'
* 05 - Focus                              // [OLD]  solenoid 5 .............. [PM] - Now 'sound trigger'
* 06 - Shutter                            // [OLD]  solenoid 6 .............. [PM] - Now 'unused'
* 07 - Solenoid-1                         // [OLD]  solenoid 1
* 08 - Solenoid-2                         // [OLD]  solenoid 2
* 09 - Solenoid-3                         // [OLD]  solenoid 3
* 10 - **Free** (Sound trigger)           // [OLD]  camera shutter trigger
* 11 - **Free**                           // [OLD]  camera focus trigger
* 12 - **Free**                           // [OLD]  flash trigger
* 13 - Status LED
* 14 A0 - 
* 15 A1 - 
* 16 A2
* 17 A3
* 18 A4 software serial RX (only if using the BT device)
* 19 A5 software serial TX (only if using the BT device)
* 20 A6 - Do NOT use, something different with this pin in pinout than other pins, misbehaves
* 21 A7 - Do NOT use, something different with this pin in pinout than other pins, misbehaves
*
*
*
*    _____                                                _
*   / ____|                                              | |
*  | |      ___   _ __ ___   _ __ ___    __ _  _ __    __| | ___
*  | |     / _ \ | '_ ` _ \ | '_ ` _ \  / _` || '_ \  / _` |/ __|
*  | |____| (_) || | | | | || | | | | || (_| || | | || (_| |\__ \
*   \_____|\___/ |_| |_| |_||_| |_| |_| \__,_||_| |_| \__,_||___/
*
* expects the following data
* !                  - ! = start of data marker. Only used for debugging
* NDx                - x = number of drops. Used to check we have data for all drops.
* D1,1,0100,0030     - D = Drop Data. Drop 1, Solenoid 1, start at 100ms, size 30ms
* D2,2,0210,0025     - D = Drop Data. Drop 2, Solenoid 2, start at 210ms, size 25ms
* D3,3,0235,0020     - D = Drop Data. Drop 3, Solenoid 3, start at 235ms, size 20ms
* FT0540,050         - FT = Flash Trigger Time. Trigger the flash at 540ms. Keep the trigger active for 50ms.
* CT0000,050         - CT = Camera Trigger Time. Trigger the shutter at 0ms. Keep the trigger active for 50ms. Trigger time of 0ms means bulb mode.
* MN1000             - M = Mirror Lock Up. N= NO.  1000 = time in ms to wait after the mirror lock up trigger. when NO the time is not used.
* MY1000             - M = Mirror Lock Up. Y= YES. 1000 = time in ms to wait after the mirror lock up trigger
* EOD                - EOD = End of Drop Data
*
* V11                - Valve 1 open
* V10                - Valve 1 close
* V21                - Valve 2 open
* V20                - Valve 2 close
* V31                - Valve 3 open
* V30                - Valve 3 close
* ....
* HELLO              - Connection request from the host computer
* BYE                - connection closed by the comtrol app
* VERSION            - request for firmware version. Not implemented in the control apps. Used for debugging through a serial monitor.
*
*
*
* To Do
* Redo the abort function.
* Add hardware switch to select device type.
*
*
*
*
*
*   _____        _              _          _               _
*  / ____|      | |            | |        | |             (_)
* | (___    ___ | |  ___   ___ | |_     __| |  ___ __   __ _   ___  ___
*  \___ \  / _ \| | / _ \ / __|| __|   / _` | / _ \\ \ / /| | / __|/ _ \
*  ____) ||  __/| ||  __/| (__ | |_   | (_| ||  __/ \ V / | || (__|  __/
* |_____/  \___||_| \___| \___| \__|   \__,_| \___|  \_/  |_| \___|\___|
*
* NOTES
* The dropControllerPC app can now be used with the dropControllerBT device (as long as your PC has Bluetooth of course).
* If using  a usb connection      - set DEVICE_TYPE = 1
* If using a Bluetooth connection - set DEVICE_TYPE = 2
*/

const byte DEVICE_TYPE = 2;




#include <SoftwareSerial.h>
SoftwareSerial BTserial(19, 18); // RX, TX


#include <digitalIOPerformance.h>
// digitalIOPerformance
// https://github.com/projectgus/digitalIOPerformance
// This library is slightly outdated and does not work with the new Arduino boards. However, it works for all the boards based aroung the 328P and similar.
// The Arduino is fairly slow at switching digital pins. The digitalIOperformance library speeds this up (as long as the pin numbers are stored in constants).





// CONSTANTS *************************************************

// Version
const char verNum[] = "009d";
const char fileName[] = __FILE__;
const char compileDate[] = __DATE__;


// Devices
const byte CAMERA  = 1;
const byte FOCUS   = 2;
const byte FLASH   = 3;


// Pins
//const byte LED_GREEN           = 2;  // removed this led, now using only single status LED
const byte LED_STATUS            = 13;  // No external LED connected here, using LED on arduino for status visuals

const byte sol1_TriggerPin       =  7;
const byte sol2_TriggerPin       =  8;
const byte sol3_TriggerPin       =  9;
const byte sol4_TriggerPin       =  3;
const byte sol5_TriggerPin       =  2;

// [PM] disabled, will use these pins for my own customization. New program will handle only 5 solenoids now
// ---- const byte sol6_TriggerPin       =  6;

const byte soundTriggerPin         =  10;           // actual sound trigger signal

const byte cam_TrigShutterPin    = 6;
const byte cam_TrigFocusPin      = 5;
const byte flash_TrigPin         = 4;


boolean soundTriggerModeEnabled           = false;  //this mode will be enabled with bluetooth command
boolean soundTestModeEnabled              = false;  //mode to test sound module sensitivity


// drop data variables *************************************************

// mirrorLockup. When set will issue a pre-sequence shutter trigger to activate the mirror lockup on the camera.
// The delay is how long to wait after the lockup.
// The trigger pulse length is set by the camera trigger pulse value
boolean mirrorLockup           = false;
unsigned int mirrorLockupDelay = 0;

// Trigger duration
unsigned int  camTriggerPulseDuration      = 50;        // How long the camera trigger signal is active
unsigned int  flashTriggerPulseDuration    = 50;        // Time the flash trigger is active.


// Arrays to hold the drop times
// I don't use element [0]. This means drop 1 is at position [1]
byte sol[10]                        = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // the solenoid to use for the drop; 1 to 6
unsigned int dropStartTime[10]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // start time in millisecond
unsigned int dropStopTime[10]       = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // stop time in millisecond

// If cameraTriggerTimeStart == 0 means we are in bulb mode and should open the shutter at the start and close the shutter at the end
unsigned int flashTriggerTime_Start  = 0;
unsigned int flashTriggerTime_Stop   = 0;
unsigned int cameraTriggerTime_Start = 0;
unsigned int cameraTriggerTime_Stop  = 0;


byte numDrops = 0;
byte numDropsCheck = 0;




// Variables used for receiving serial data  ************************************************
const byte numChars = 20;
char receivedChars[numChars];
boolean haveNewData = false;
boolean haveNewDrop = false;

// variables to hold received commands        ************************************************
char flashCommand[12];
char cameraCommand[12];
char mirrLockupCommand[7];
char dropCommand[10][16] = {"0-1234567890", "1-1234567890", "2-1234567890" , "3-1234567890" , "4-1234567890" , "5-1234567890" , "6-1234567890" , "7-1234567890" , "8-1234567890" , "9-1234567890"    };



//  variables used when checking received data ************************************************
boolean haveFT = false;
boolean haveCT = false;
boolean haveML = false;
boolean haveAllDrops = false;





void setup()
{

  // I open both hardware serial and software serial regardless of which device is used.
  // This does not effect the sketch and makes the programming easier. (I couldn't get proprocessor commands to work correctly!)
  Serial.begin(9600);
  BTserial.begin(38400);  // [PM] fixed baud rate for HC-05

  //pinMode(13, OUTPUT); digitalWrite(13, LOW);

  // status LEDs
  pinMode(LED_STATUS, OUTPUT);  digitalWrite(LED_STATUS, LOW);

  // define the trigger pins and set the pins to low
  pinMode(sol1_TriggerPin, OUTPUT);    digitalWrite(sol1_TriggerPin, LOW);
  pinMode(sol2_TriggerPin, OUTPUT);    digitalWrite(sol2_TriggerPin, LOW);
  pinMode(sol3_TriggerPin, OUTPUT);    digitalWrite(sol3_TriggerPin, LOW);
  pinMode(sol4_TriggerPin, OUTPUT);    digitalWrite(sol4_TriggerPin, LOW);
  pinMode(sol5_TriggerPin, OUTPUT);    digitalWrite(sol5_TriggerPin, LOW);

  // [PM] solenoids disabled for my customization
  // pinMode(sol6_TriggerPin, OUTPUT);    digitalWrite(sol6_TriggerPin, LOW);

  pinMode(soundTriggerPin, INPUT);


  pinMode(cam_TrigShutterPin, OUTPUT); digitalWrite(cam_TrigShutterPin, LOW);
  pinMode(cam_TrigFocusPin, OUTPUT);   digitalWrite(cam_TrigFocusPin, LOW);
  pinMode(flash_TrigPin, OUTPUT);      digitalWrite(flash_TrigPin, LOW);


  // flash the LED to show version number - verNum
  // Now that I am at version 9 this is no longer a good solution and I may remove it

  //int versionNumber = atoi(verNum);
  //byte loopCount = 0;
  //while (loopCount < 4)  //while (loopCount < versionNumber)
  //{
  //  digitalWrite(LED_STATUS, HIGH); delay (250); digitalWrite(LED_STATUS, LOW); delay (250);
  //  loopCount++;
  //}
  blinkLED (LED_STATUS, 4, 250, 250);  //blink 4 times on startup

  // LED_GREEN is turned on when the HELLO message is received from the control app.

  initialise();
}


void loop()
{
  if (soundTestModeEnabled) {

    //*********************** sound test mode *****************************************************************************************************
    digitalWrite(LED_STATUS, HIGH);  //indicate with yellow led that board is listening sound
    //digitalWrite(LED_GREEN, LOW);

    boolean soundDetected = waitTillSoundTriggerDetected(20000);	 //argument is timeout delay
    if (!soundDetected) {
      blinkLED (LED_STATUS, 4, 200, 100);			//blink led to notify timeout
    }
    else {
      digitalWrite(LED_STATUS, LOW);
      //digitalWrite(LED_GREEN, HIGH);
      delay (2000);
    }

  }
  else {

    //*********************** default workflow *****************************************************************************************************

    if      (DEVICE_TYPE == 1) {
      recvWithStartEndMarkersUSB();
    }
    else if (DEVICE_TYPE == 2) {
      recvWithStartEndMarkersBT();
    }

    if (haveNewData) {
      parseNewData();  // this doesn't actually parse data. It copies it to temp variables
    }
    // Also sets haveNewDrop to TRUE when it gets an "EOD" command. "EOD" is the End Of Data flag.

    if (haveNewDrop)
    {
      parseDropData();                   // copy the temp received data variables to the drop data arrays.

      if ( dropDataIsOK() )              // check we have all the drop data. This is a basic check only.
      {
        //digitalWrite(LED_GREEN, LOW);

        // if (soundTriggerModeEnabled){
        //  //flash led, indicating sound-trigger mode
        //   blinkLED (LED_STATUS, 2, 60, 60);
        // }

        digitalWrite(LED_STATUS, HIGH);

        makeDrops();
        delay(100);
        sendFinishedFlag();
      }
      else
      {
        showError();
        reset();
      }

      //digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_STATUS, LOW);


      initialise();
    }
  }

}



/*
****************************************
* Function initialise
*
* Passed:
*
* Global:
*    numDrops
*    numDropsCheck
*    haveNewDrop
*    haveFT
*    haveCT
*    haveML
*    haveAllDrops
*
* Returns:
*
* Sets:
*    numDrops
*    numDropsCheck
*    haveNewDrop
*    haveFT
*    haveCT
*    haveML
*    haveAllDrops
*
*/
void initialise()
{
  numDrops = 0;
  numDropsCheck = 0;
  haveNewDrop = false;
  for (int i = 1; i < 10; i++)   {
    dropCommand[i][0] = ' ' ;
    dropCommand[i][1] = 0 ;
  }
  haveFT = false;
  haveCT = false;
  haveML = false;
  haveAllDrops = false;
}




/*
****************************************
* Function reset
*
* Passed:
*
* Global:
*
* Returns:
*    True or False
*
* Sets:
*
*/
void reset()
{
  // the control app waits for the finished flag. If this is not sent the program freezes.
  // Ideally I need to add a time out to the program
  sendFinishedFlag();
}




/*
****************************************
* Function dropDataIsOK
* checks the received drop data. Basic checking only.
*
* Passed:
*
* Global:
*    numDrops
*    numDropsCheck
*    haveFT
*    haveCT
*    haveML
*
* Returns:
*    dTrue or False
*
* Sets:
*
*/
boolean dropDataIsOK()
{
  // need to expand this
  // check each field        FT, CT, ML
  // check to see if any blank drop data
  boolean dataOK = true;
  for (int i = 1; i < numDrops + 1; i++)
  {
    if ( dropCommand[i][0] != 'D')   {
      dataOK = false;
    }
  }

  if (numDrops != numDropsCheck) {
    dataOK = false;
  }
  if (!haveFT) {
    dataOK = false; ;
  }
  if (!haveCT) {
    dataOK = false; ;
  }
  if (!haveML) {
    dataOK = false; ;
  }

  return dataOK;
}





/*
****************************************
* Function showError
* Flashes an LED to show there is an error.
*
* Passed:
*
* Global:
*
* Returns:
*
* Sets:
*
*/
void showError()
{
  for (int i = 1; i < 9; i++)
  {
    digitalWrite(LED_STATUS, HIGH); delay(100);
    digitalWrite(LED_STATUS, LOW);  delay(75);
  }
}







/*
****************************************
* Function sendFinishedFlag
* Emties any remaining serial data and sends a <FTS> signal
*
* Passed:
*
* Global:
*
* Returns:
*
* Sets:
*
*/
void sendFinishedFlag()
{
  char rc;
  if (DEVICE_TYPE == 1)
  {
    while (Serial.available() > 0)  {
      rc = Serial.read();    // not really required but makes me feel better.
    }
    Serial.print("<FTS>");
    //digitalWrite(13, HIGH);
  }

  if (DEVICE_TYPE == 2)
  {
    while (BTserial.available() > 0)  {
      rc = BTserial.read();    // not really required but makes me feel better.
    }
    BTserial.print("<FTS>");
  }
}






















