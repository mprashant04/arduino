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
  lcdPrint(F("                    "), LCD_TRANSIENT_MESSAGE_COL, LCD_TRANSIENT_MESSAGE_ROW);
}

void lcdWelcomeMessage(){
  lcdPrint(F("--------------------"), 0, 0);
  lcdPrint(F(__DATE__ " " __TIME__),  0, 1);    //sketch build date and time
  lcdPrint(F("--------------------"), 0, 2); 
  
  //delay (2000);
  //lcdClear();  
      
  //lcdPrint(F("INITIATING..."), 4, 1);  
  playTone(TONE_SINGLE, 0, 150, TONE_ARG_EOL); 

  for (int i = 20; i > 0 ; i--){
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
  static boolean lastDisplayStatusFlag = true;

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
  
  lcdPrint(centerAlign(String(waterLevelPercentageEMA,1)+ "%", LCD_WIDTH), 0, 3);  
  lcdPrint(getBlinkStatusChar(lastDisplayStatusFlag), 18,3); 

//  switch (waterLevelAlertType){
//      case 0:    lcdPrint(F("  "), 0,3);   break;
//      case 1:    lcdPrint(F("H1"), 0,3);   break;
//      case 2:    lcdPrint(F("H2"), 0,3);   break;
//      case -1:   lcdPrint(F("L1"), 0,3);   break;
//      case -2:   lcdPrint(F("L2"), 0,3);   break;
//      default:   lcdPrint(F("??"), 0,3);   break;
//  }

  switch (waterTankFillingInProgress){
      case true:  lcdPrint(F("^"), 0,3);   break;
      case false: lcdPrint(F(" "), 0,3);   break;
      default:    lcdPrint(F("?"), 0,3);   break;      
  }
}

String getBlinkStatusChar(boolean lastDisplayStatusFlag){
  if (lastDisplayStatusFlag){
      if (isDebugModeRawSignal() && isSerialDebugMessagingEnabled())          return F("ds");
      else if (isSerialDebugMessagingEnabled())                               return F(" d");
      else if (isDebugModeRawSignal())                                        return F(" s");

      return F(" .");
  }
  return F("  ");
}


void lcdShowApiCallStatus(char stat[]){
    for (int i = 0; i < 20; i++){
        if (stat[i] == '0')
            lcdPrintSpecialChar(CHAR_API_CALL_NOT_OK,  i, LCD_TRANSIENT_MESSAGE_ROW);
        else if (stat[i] == '1')
            lcdPrintSpecialChar(CHAR_BLOCK_BORDER_TOP, i, LCD_TRANSIENT_MESSAGE_ROW);
    }
}
