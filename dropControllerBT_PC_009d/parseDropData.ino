
/*
****************************************
* Function parseDropData
* Converts ascii text to numeric values for the drop data, the flash trigger time, the camera shutter trigger time and the mirror lock up time 
* 
* passed:
*  
* global: 
*         flashCommand[]
*         cameraCommand[]
*         mirrLockupCommand[]
*
* Returns:
*          
* Sets:
*      flashTriggerTime_Start
*      flashTriggerTime_Stop
*      cameraTriggerTime_Start
*      cameraTriggerTime_Stop
*      mirrorLockup
*      mirrorLockupDelay
*/


void parseDropData()
{
     unsigned int temp = 0;

     // flash trigger time
     temp = (flashCommand[2] - 48) * 1000;
     temp = temp + (flashCommand[3] - 48) * 100;
     temp = temp + (flashCommand[4] - 48) * 10;
     temp = temp + flashCommand[5] - 48;
     flashTriggerTime_Start = temp;
     
     temp = (flashCommand[7] - 48) * 100;
     temp = temp + (flashCommand[8] - 48) * 10;
     temp = temp + flashCommand[9] - 48;
     flashTriggerTime_Stop = flashTriggerTime_Start + temp;
     
     
     // camera trigger time
     // CT0000,050
     temp = (cameraCommand[2] - 48) * 1000;
     temp = temp + (cameraCommand[3] - 48) * 100;
     temp = temp + (cameraCommand[4] - 48) * 10;
     temp = temp + cameraCommand[5] - 48;
     cameraTriggerTime_Start = temp;
     
     temp = (cameraCommand[7] - 48) * 100;
     temp = temp + (cameraCommand[8] - 48) * 10;
     temp = temp + cameraCommand[9] - 48;
     cameraTriggerTime_Stop = cameraTriggerTime_Start + temp;
     
     
     // mirror Lock Up
     // MN1000
     if (mirrLockupCommand[1] == 'Y') {  mirrorLockup = true;  } else {  mirrorLockup = false;  }
     temp = (mirrLockupCommand[2] - 48) * 1000;
     temp = temp + (mirrLockupCommand[3] - 48) * 100;
     temp = temp + (mirrLockupCommand[4] - 48) * 10;
     temp = temp + mirrLockupCommand[5] - 48; 
     mirrorLockupDelay = temp; 



     //drop data
     //
     // 01234567890123
     // D1,1,0100,0030

     // dropCommand[numDrops]-[1] is the drop number (1 to 9)
     // dropCommand[numDrops]-[2] is a ','
     // dropCommand[numDrops]-[3] is the valve
     // dropCommand[numDrops]-[4] is a ','
     // dropCommand[numDrops]-[5][6][7][8] is the start time in milliseconds
     // dropCommand[numDrops]-[9] is a ','
     // dropCommand[numDrops]-[10][11][12][13] is the size time in milliseconds
     //
     // The received data is in ascii. To convert an ascii "1" to the value value 1 simply subtract 48
     //

     byte dropNumber = 0;
     
     for (int i = 1; i < numDrops+1; i++)   
     {   
     
       dropNumber      = dropCommand[i][1] - 48;
       sol[dropNumber] = dropCommand[i][3] - 48;

       temp = (dropCommand[i][5] - 48) * 1000;
       temp = temp + (dropCommand[i][6] - 48) * 100;
       temp = temp + (dropCommand[i][7] - 48) * 10;
       temp = temp +  dropCommand[i][8] - 48;
       dropStartTime[dropNumber] = temp;
       
       temp = (dropCommand[i][10] - 48) * 1000;
       temp = temp + (dropCommand[i][11] - 48) * 100;
       temp = temp + (dropCommand[i][12] - 48) * 10;
       temp = temp + dropCommand[i][13] - 48;              
       dropStopTime[dropNumber] = dropStartTime[dropNumber] + temp;
	   
	   //[PM] Ignore valve-4 & 5 drop size & override it to flash-trigger-pulse-time. This valves used as secondary flashes
	   if (sol[dropNumber] == 4 || sol[dropNumber] == 5){
			dropStopTime[dropNumber] = dropStartTime[dropNumber] + (flashTriggerTime_Stop - flashTriggerTime_Start) ;
	   }

    }









}
