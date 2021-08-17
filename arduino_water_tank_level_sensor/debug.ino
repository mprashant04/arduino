
// make 1 to enable, 0 = disable
char debug_mode_raw_signal = '0';
char serialDebugMessagingEnabled = '0';

 
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
 void println(unsigned int msg){
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
    print(msg, false);
 }
 void print(String msg, boolean force){
    if (!isSerialDebugMessagingEnabled() && !force) return;
    if (BT_CONNECTED_TO_SERIAL_PINS == false)      Serial.print(msg);        
    BTserial.print(msg);           
 }
 void println(String msg){
    println(msg, false);
 }
 void println(String msg, boolean force){
    if (!isSerialDebugMessagingEnabled() && !force) return;
    if (BT_CONNECTED_TO_SERIAL_PINS == false)      Serial.println(msg);    
    BTserial.println(msg);           
 }

 void toggleDebugModeRawSignal(){
    if (isDebugModeRawSignal())
      debug_mode_raw_signal = '0';
    else
        debug_mode_raw_signal = '1';
    
    String msg = (isDebugModeRawSignal() ? F("Signal Debug ON") : F("Signal Debug OFF"));
    lcdTransientMessage(msg, 2000);
    println(msg);    
}

 boolean isDebugModeRawSignal(){    
    return debug_mode_raw_signal == '1';
 }

 boolean isSerialDebugMessagingEnabled(){
    return serialDebugMessagingEnabled == '1'; 
 }

 void toggleSerialDebugMessagingEnabled(){    
    serialDebugMessagingEnabled = isSerialDebugMessagingEnabled() ? '0' : '1';             

    String msg = isSerialDebugMessagingEnabled() ? F("BT Logs ENABLED") : F("BT Logs DISABLED");
    println(msg);  
    lcdTransientMessage(msg, 2000);  
 }
 
