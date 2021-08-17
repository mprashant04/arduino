/*
 * Install all required libraries first:
 *    - Sketch -> include library -> add .zip library -> select D:\_my_cloud_synced\softwares\_Development\Arduino\_Development libraries\xxx.zip
 * 
 * ============================
 *      P  I  N  S
 * ============================
 * Pinout diagram: https://upload.wikimedia.org/wikipedia/commons/e/e4/Arduino-nano-pinout.png
 * 00 RXD - BT HC 05 TX
 * 01 TXD - BT HC 05 RX
 * 02 D2  - Wifi RX
 * 03 D3  - Wifi TX
 * 04 D4  - Wifi reset (via opto-coupler)
 * 05 D5  - 
 * 06 D6  - 
 * 07 D7  - 
 * 08 D8  - 
 * 09 D9  -  
 * 10 D10 - 
 * 11 D11 - 
 * 12 D12 - 
 * 13     - 
 * 14 A0  - Buzzer
 * 15 A1  - 
 * 16 A2  - POT signal
 * 17 A3  - 
 * 18 A4  - LCD i2c SDA
 * 19 A5  - LCD i2c SLC
 * 20 A6  - Do NOT use, something different with this pin in pinout than other pins, misbehaves
 * 21 A7  - Do NOT use, something different with this pin in pinout than other pins, misbehaves
 *
 *
 */


/*  =============================================================
 *     Development Notes
 *  =============================================================
 *  Generic dev notes recorded here - https://github.com/mprashant04/arduino
 *  
 * Wifi References / Notes
 *    - https://www.youtube.com/watch?v=nMWwqcn7ofw
 *    - more ref examples?? - https://github.com/mathworks/thingspeak-arduino 
 *    - esp8266 cannot be connected to arduino 3.3v pin, arduino cannot provide required current for it
 *    - for serial mode AT commands, tx/rx needs to connected to tx/rx0 of arduino, for regular run they can be on data pins
 * 
 * LCD notes
 *    - Ref: https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/
 * IR remote and tone() conflict   
 *    - Both use same timer 2, hence IR remote did not work after using tone(), so manually changed timer to 1 for IR remote library
 *        (file: IRTimer.cpp.h, line 76, commented IR_USE_AVR_TIMER2 and uncommented IR_USE_AVR_TIMER1)
 * 
 *  Jumping signal issue forum discussion  - https://forum.arduino.cc/t/unstable-readings-from-potentiometer-with-long-wire/887784
 *  
 *  String constatants also consumes memory, use carefully ,e.g. println("hello"); will consume 5 global bytes memory
 *      also avoid duplicate strings, use #define for strings multiple uses
 *  
 *  
 *  
 *  
 * 
 * *********************************************************************
 *             OTA Sketch Upload Using BT 
 * *********************************************************************
 *    - references
 *        - https://www.mischianti.org/2019/01/31/arduino-remote-wireless-programming/
 *        - http://www.martyncurrey.com/arduino-with-hc-05-bluetooth-module-at-mode/
 *    - Put HC05 in AT command mode & configure  (one time activity) 
 *        - somehow high signal on EN pin / pressing hc05 button did not work. 
 *        - finally used pin 34 approach
 *            - cut the protective plastic cover on pin-34 of hc05 (top right pin) 
 *            - upload this code to arduino
 *            - open serial monitor, set baud rate to 57600 (or the baud rate HC05 was configured last time) and select "Both NL & CR"
 *            - connect 
 *                pin34 of hc05 -> 3.3v of arduino (just connected a jumper wire pin manually to pin-34, manually and carefully)
 *                GND of hc05   -> GND of arduino
 *                VCC of hc05   -> kept disconnected
 *                TX of hc05    -> TX of arduino
 *                RX of hc05    -> RX of ardun0
 *            - uplaod blank sketch to arduno & power on arduino
 *            - connect VCC pin of HC05 to arduino 5v pin
 *            - now hc05 should to in command mode (somehow slow led blinking did not happen on my hc05, but got OK response to AT command in serial monitor. That means hc05 in command mode)
 *            - run followign commands in serial monitor
 *                  AT                    -> OK                    
 *                  AT+ORGL               -> OK                    
 *                  AT+POLAR=1,0          -> OK                    
 *                  AT+NAME=arduino_hc05  -> OK                    
 *                  AT+UART=57600,0,0     -> OK     (for nano 57600 is required baud rate)                                                         
 *                  AT+INIT               -> did not receive any response, but should be ok as per                                         
 *                  
 *    - how to upload OTA
 *        - In windows bluetooth devices, pair to "arduini_bt" device
 *        - then in windows "device manager" you will see 2 new com ports added
 *        - in Arduino ide, use one of theese 2 new ports (usually first one) and just upload  (ensure no other device is connected to this BT device)
 *        - to use ide serial monitor, just select same port and choose baud rate 57600
 *    
 *    - cannot use tone() fuction now, causes compilation error due to conflict with TimerInterrupt library
 *        in short looks like cannot use anything based on hardware timer, like IR led timer based library, etc.
 *             
 * 
 */






#include <SoftwareSerial.h>
#include <Wire.h>                   // Library for I2C communication
#include <LiquidCrystal_I2C.h>      // Library for LCD
#include "timers.h"
#include "debug.h"
#include "buzzer.h"
#include "bluetooth.h"
#include "water_level_sensor.h"


#define LCD_TRANSIENT_MESSAGE_COL     0
#define LCD_TRANSIENT_MESSAGE_ROW     2
#define REBOOT_INTERVAL_HOURS         12
#define AT_OK                         "OK"
#define UNSIGNED_LONG_MAX             4294967290



LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 20, 4); 


// was not able to use 2 software serials (BT and wifi) at once, hence connected BT to tx/rx pins and using as inbuilt serial. Can flip between tx/rx ping and 16-17 pins by using jumpers on board
// after changing jumper comment/uncomment folloing #define appropriately
// NOTE: Power off BT using on board toggle switch while uplaoding sketch using usb, else upload fails
#define BTserial  Serial  
#if defined(BTserial)
    #define BT_CONNECTED_TO_SERIAL_PINS  true
#else
    #define BT_CONNECTED_TO_SERIAL_PINS  false
    SoftwareSerial  BTserial(17, 16); // RX | TX
#endif


SoftwareSerial esp8266(3,2);


int wifi_CommandFailureCount;
int wifi_CommandSuccessCount;


void setup() {    
  if (BT_CONNECTED_TO_SERIAL_PINS == false)   Serial.begin(9600);  

  // have changed baud rate of HC05 module i am using now on board, to 57600 using AT commands (e.g. ref- https://www.instructables.com/Changing-Baud-Rate-of-HC-05-Bluetooth/)
  // As per this page, baud rate required for nano is 57600 in order to upload sketch using BT:: https://www.electroniclinic.com/arduino-ota-over-the-air-programming-using-bluetooth/
  // So if using ide serial monitor, choose 57600 baud rate
  // **Update** after changing BT baud rate and connecting to serial tx/rx pins, wifi api call communciation became more stable, looks like earlier both BT and 
  //            wifi having high rate was causing some data loss for wifi. So win-win case
  BTserial.begin(57600); //(38400); 
  esp8266.begin(115200);      

  initTimers();  //tones will work only after this...

  //println(F("\n********* BOOT **********  v1.4"), true); 
  println (F("******** BOOT ******** (Built on " __DATE__ " at " __TIME__")"), true); 
    
  lcdInit();
  wifiInit();
  startupValidations();    
  lcdWelcomeMessage();

  startWaterReading = true;
  waitTillFirstWaterSignalRead();  
}

void loop() {
  unsigned long startedOn = millis();
  
  handleBluetoothCommands();

  if (isWaterReadingUpdated(true)){
      lcdUpdateWaterStatus();
      playWaterLevelAlertIfAny();
      playWaterTankFillingStartedAlert();
  }
  
  if (isDebugModeRawSignal())
    delay (500);
  else
    thingSpeakSendData();
  
  scheduledReboot(); 
}

void scheduledReboot(){
  //if (getUptimeInHours() >= REBOOT_INTERVAL_HOURS)
  //    reboot();
}


void waitTillFirstWaterSignalRead(){
  lcdTransientMessage(F("Init Timers.."));  
  while (!isWaterReadingUpdated(false)){}
  lcdTransientMessageClear();
}


void startupValidations(){    
  validateWaterAlertLevelDefinitions();
}
