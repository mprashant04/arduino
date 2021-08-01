//max 8 charactes can be defined, 0-7
#define CHAR_BLOCK_FILLED               0
#define CHAR_BLOCK_BORDER_TOP           1
#define CHAR_BLOCK_BORDER_BOTTOM        2
#define CHAR_BLOCK_BORDER_TOP_LEFT      3
#define CHAR_BLOCK_BORDER_TOP_RIGHT     4
#define CHAR_BLOCK_BORDER_BOTTOM_LEFT   5
#define CHAR_BLOCK_BORDER_BOTTOM_RIGHT  6
#define CHAR_API_CALL_NOT_OK            7

#define LCD_WIDTH                       20

boolean lastDisplayStatusFlag = true;


void lcdInit(){
  //declaring byte arrays in function and not globally to save memory
  byte SpecialCharData_Block_Filled[] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111  
  };
  byte SpecialCharData_Border_Top[] = {
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000, 
  };
  byte SpecialCharData_Border_Bottom[] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111, 
  };
  byte SpecialCharData_Border_Top_Left[] = {
    B11111,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000, 
  };
  byte SpecialCharData_Border_Top_Right[] = {
    B11111,
    B00001,
    B00001,
    B00001,
    B00001,
    B00001,
    B00001,
    B00001, 
  };
  byte SpecialCharData_Border_Bottom_Left[] = {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B11111, 
  };
  byte SpecialCharData_Border_Bottom_Right[] = {
    B00001,
    B00001,
    B00001,
    B00001,
    B00001,
    B00001,
    B00001,
    B11111, 
  };
  byte SpecialCharData_ApiCallNotOk[] = {
    B00100,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
  };

  lcd.init();
  lcd.backlight();  

  lcd.createChar(CHAR_BLOCK_FILLED, SpecialCharData_Block_Filled);
  lcd.createChar(CHAR_BLOCK_BORDER_TOP, SpecialCharData_Border_Top);
  lcd.createChar(CHAR_BLOCK_BORDER_BOTTOM, SpecialCharData_Border_Bottom);
  lcd.createChar(CHAR_BLOCK_BORDER_TOP_LEFT, SpecialCharData_Border_Top_Left);
  lcd.createChar(CHAR_BLOCK_BORDER_TOP_RIGHT, SpecialCharData_Border_Top_Right);
  lcd.createChar(CHAR_BLOCK_BORDER_BOTTOM_LEFT, SpecialCharData_Border_Bottom_Left);
  lcd.createChar(CHAR_BLOCK_BORDER_BOTTOM_RIGHT, SpecialCharData_Border_Bottom_Right);
  lcd.createChar(CHAR_API_CALL_NOT_OK, SpecialCharData_ApiCallNotOk);
}


void lcdTransientMessage(String msg){  
  lcdTransientMessage(msg, 0);
}


//if millSeconds = 0, message is not erased
void lcdTransientMessage(String msg, int millSeconds){  
  lcdTransientMessageClear();
  lcdPrint(centerAlign(msg, LCD_WIDTH), LCD_TRANSIENT_MESSAGE_COL, LCD_TRANSIENT_MESSAGE_ROW);
   if (millSeconds > 0){
      delay (millSeconds);
      lcdTransientMessageClear();
   }
}


void lcdTransientMessageClear(){  
  lcdPrint("                    ", LCD_TRANSIENT_MESSAGE_COL, LCD_TRANSIENT_MESSAGE_ROW);
}

void lcdWelcomeMessage(){    
  lcdPrint("INITIATING...", 4, 1);
  //playPassed();
  playLoudTone(150);  
  delay (2000);
  
  //lcdPrintSpecialChar (CHAR_POWER, 8, 3);
  
  for (int i = 15; i > 0 ; i--){
    lcdPrint(String (i) + "     ", 9, 3);
    delay (1000);
  }
  
  lcdClear();
}

void lcdPrintSpecialChar(int character, int col, int row){
  lcd.setCursor(col, row);
  lcd.write(character);
}

void lcdPrint(String msg, int col, int row){
  lcd.setCursor(col, row); 
  lcd.print(msg);
}


void lcdClear(){
  lcd.clear();
}

void lcdUpdateWaterStatus(){

  float levelPcIncrement = 2.5; // 40 blocks divided by 100
  float levelPc = 0.0;
    
  for (int col = 0; col <= 19; col++){
      for (int row = 1; row >= 0; row--){          
          if (waterLevelPercentageEMA >= levelPc){            
              lcdPrintSpecialChar(CHAR_BLOCK_FILLED, col, row);           
          }
          else{
              if (row==0 && col==0)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_TOP_LEFT, col, row);
              else if (row==1 && col==0)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_BOTTOM_LEFT, col, row);
              else if (row==0 && col==19)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_TOP_RIGHT, col, row);
              else if (row==1 && col==19)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_BOTTOM_RIGHT, col, row);
              else if (row==0)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_TOP, col, row);
              else if (row==1)
                  lcdPrintSpecialChar(CHAR_BLOCK_BORDER_BOTTOM, col, row);                  
          }
          levelPc += levelPcIncrement;
      }
  }
  
  lastDisplayStatusFlag =  !lastDisplayStatusFlag;   
  
  lcdPrint(centerAlign(String(round(waterLevelPercentageEMA))+ "%", LCD_WIDTH), 0, 3);  
  lcdPrint(getBlinkStatusChar(), 18,3);    
}

String getBlinkStatusChar(){
  if (lastDisplayStatusFlag){
      if (isDebugModeRawSignal() && isSerialDebugMessagingEnabled())          return "ds";
      else if (isSerialDebugMessagingEnabled())                               return " d";
      else if (isDebugModeRawSignal())                                        return " s";

      return " .";
  }
  return "  ";
}


void lcdShowApiCallStatus(char stat[]){
    for (int i = 0; i < 20; i++){
        if (stat[i] == '0')
            lcdPrintSpecialChar(CHAR_API_CALL_NOT_OK,  i, LCD_TRANSIENT_MESSAGE_ROW);
        else if (stat[i] == '1')
            lcdPrintSpecialChar(CHAR_BLOCK_BORDER_TOP, i, LCD_TRANSIENT_MESSAGE_ROW);
    }
}
