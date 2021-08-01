
// make 1 to enable, 0 = disable
char debug_mode_raw_signal = '0';
char serialDebugMessagingEnabled = '0';

void debugInit(){  
}

 void print(String msg, int numberOfTimesToPrint){
    if (!isSerialDebugMessagingEnabled()) return;
    while (numberOfTimesToPrint-- > 0)
       print(msg);
 }
 void print(int msg){
    if (!isSerialDebugMessagingEnabled()) return;
    print(String(msg));
 }
 void print(unsigned long  msg){
    if (!isSerialDebugMessagingEnabled()) return;
    print(String(msg));
 }
 void println(int msg){
    if (!isSerialDebugMessagingEnabled()) return;
    println(String(msg));
 }
 void print(float msg){
    if (!isSerialDebugMessagingEnabled()) return;
    print(String(msg));
 }
 void println(float msg){
    if (!isSerialDebugMessagingEnabled()) return;
    println(String(msg));
 }
 void print(String msg){
    if (!isSerialDebugMessagingEnabled()) return;
    if (BT_CONNECTED_TO_SERIAL_PINS == false)      Serial.print(msg);        
    BTserial.print(msg);           
 } 
 void println(String msg){
    if (!isSerialDebugMessagingEnabled()) return;
    if (BT_CONNECTED_TO_SERIAL_PINS == false)      Serial.println(msg);    
    BTserial.println(msg);           
 }

 void toggleDebugModeRawSignal(){
    if (isDebugModeRawSignal())
      debug_mode_raw_signal = '0';
    else
        debug_mode_raw_signal = '1';

    //------not printing to save some memory of string literals--------------------
    //String msg = (isDebugModeRawSignal() ? "Signal Debug ON" : "Signal Debug OFF");
    //lcdTransientMessage(msg, 2000);
    //println(msg);    
}

 boolean isDebugModeRawSignal(){    
    return debug_mode_raw_signal == '1';
 }

 boolean isSerialDebugMessagingEnabled(){
    return serialDebugMessagingEnabled == '1'; 
 }

 void toggleSerialDebugMessagingEnabled(){    
    serialDebugMessagingEnabled = isSerialDebugMessagingEnabled() ? '0' : '1';             
    //println( isSerialDebugMessagingEnabled() ? "serial debug messages  ENABLED" : "serial debug messages  DISABLED", true);  //not printing to save some memory of string literals
 }
 
