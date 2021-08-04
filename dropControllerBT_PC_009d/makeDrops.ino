
/*
****************************************
* Function makeDrops
* controls the solenoid values to make the drops
* 
* passed:
*  
* global: 
*         sol[]
*         dropStartTime[]
*         dropStopTime[]
*         flashTriggerTime_Start
*         flashTriggerTime_Stop
*         cameraTriggerTime_Start
*         cameraTriggerTime_Stop
*         numDrops
*
* Returns:
*          
* Sets:
*
*/


void makeDrops()
{

    closeSolenoid(1); 
    closeSolenoid(2);
    closeSolenoid(3);
  
    boolean done    = false;
    boolean aborted = false;
    boolean done2   = false;
    
    long unsigned seqStartTime = 0;  
    int unsigned loopCount = 0;

    boolean CamTriggered   = false;
    boolean camDone = false;
    boolean flashTriggered = false;
    boolean flashDone = false;
    boolean allDropsDone = false;
    
    byte currentDrop = 0;
    boolean dropped[10] = { false, false, false, false, false, false, false, false, false, false} ;
    boolean solOpen[10] = { false, false, false, false, false, false, false, false, false, false} ;
    
     
    

    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    startPos:
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

    //if (checkButtonPress(false)==BUTTON_SELECT ) { aborted = true; } 
    // sometimes a goto is just what you need.
    if (aborted) {goto endPos; }
           



    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    // start the drop sequence

 
    // activate mirror lockup
    if (mirrorLockup && !aborted) 
    { 
         digitalWrite( cam_TrigFocusPin, HIGH );  
         digitalWrite( cam_TrigShutterPin, HIGH );  
         delay( cameraTriggerTime_Stop - cameraTriggerTime_Start );  
         digitalWrite( cam_TrigFocusPin, LOW );  
         digitalWrite( cam_TrigShutterPin, LOW );
         
         done = false;
         long unsigned startTime = millis();
         while (startTime == millis())  // This makes sure we are starting at the beginning of a millisecond
         {;}
         startTime = millis();
         
         while (!done)
         {
            if (millis() - startTime >= mirrorLockupDelay ) { done = true;  }
         }

    }

    if (aborted)  {  goto endPos ;  }  //  sometimes gotos are the easiest way to do something.
	

	


    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-   
    // FROM THIS POINT KEEP EVERYTHING TO ESSENTIAL OPERATIONS ONLY
	
	
 

    CamTriggered = false;
    camDone = false;
    flashTriggered = false; 
    flashDone = false;
    allDropsDone = true;
    
 
     // if cameraTriggerTime_Start == 0 then use bulb mode. Open the shutter at the beginning and close at the end.
     // Close the shutter 250 milliseconds after the flash is triggered. 
    if ( cameraTriggerTime_Start == 0)  
    {    
         cameraTriggerTime_Stop = flashTriggerTime_Stop + 100;     // close the camera 100 ms after the flash. 
         triggerStart(FOCUS);
         triggerStart(CAMERA);
         CamTriggered = true;
    }      
	
	
	// set the drops as not dropped
    // set the solenoid valve for each drop as not open
    for (int i=1; i<9; i++)   { dropped[i] = false;  solOpen[i] = false; }
	
	

	
	// **********  [PM] - Customization done for handling sound trigger ****************************************************
	if (soundTriggerModeEnabled) {				
		blinkLED (LED_STATUS, 2, 60, 60);  //blink green led indicating start of sound trigger detection		
		boolean soundDetected = waitTillSoundTriggerDetected(10000);	 		
		if (!soundDetected) {
			blinkLED (LED_STATUS, 4, 200, 100);			//blink green led to notify timeout			
			goto endPos ;    //timeout, abort further execution
		}
	}


   
    

    done=false;
    seqStartTime = millis(); 
    while (seqStartTime == millis())  // Make sure we are starting at the beginning of a millisecond
    {}
    seqStartTime = millis();
    
    while (!done)
    {
        if (!CamTriggered && !camDone)     { if (millis() - seqStartTime >= cameraTriggerTime_Start   )  { triggerStart(FOCUS); triggerStart(CAMERA);  CamTriggered = true;    }     }
        if (!flashTriggered && !flashDone) { if (millis() - seqStartTime >= flashTriggerTime_Start    )  { triggerStart(FLASH);   flashTriggered = true;  }     }
        
        // assume all drops are done  then check to see if any are not.
        allDropsDone = true;
        
        // loop through the drops and see if we need to start or stop the solenoids
        for ( currentDrop=1; currentDrop <= numDrops; currentDrop++)
        {
             if ( !dropped[currentDrop] && !solOpen[currentDrop] )  
             { 
                  if ( millis() - seqStartTime >= dropStartTime[currentDrop] )   { openSolenoid(  sol[currentDrop]);  solOpen[currentDrop] = true;     } 
             }  
             
             if (solOpen[currentDrop]) 
             { 
                  if ( millis() - seqStartTime >= dropStopTime[currentDrop]  )   
                  { 
                       closeSolenoid( sol[currentDrop] );  
                       solOpen[currentDrop] = false;   
                       dropped[currentDrop] = true; 
                  }   
             }
              
             if ( !dropped[currentDrop] ) { allDropsDone = false;}              
              
        } // for ( currentDrop=1; currentDrop < tmpNumDrops; currentDrop++)
        
        
        // check to see if need to stop the flash trigger or the camera triggers
        if (!flashDone) { if (flashTriggered && ( millis() - seqStartTime >= flashTriggerTime_Stop  ))  { triggerStop(FLASH);   flashDone = true;  }     }
        if (!camDone)   { if (CamTriggered   && ( millis() - seqStartTime >= cameraTriggerTime_Stop ))  { triggerStop(FOCUS);   triggerStop(CAMERA);  camDone = true;  }    }
        
        if (allDropsDone && flashDone && camDone) {done = true;}
        
    } // while (!done)


    // at this point all drops and triggers should have been activated.


    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-  
    
    

     
    ;
    endPos:
    ;
     // close all 
     // left in as part of the abort. May reimplement an abort function later
     triggerStop(CAMERA);
     triggerStop(FOCUS);
     triggerStop(FLASH);
     closeSolenoid(1);
     closeSolenoid(2);
     closeSolenoid(3);


     
     
}   // void makeDrops()


