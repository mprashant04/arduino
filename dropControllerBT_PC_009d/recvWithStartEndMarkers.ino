
     
// function recvWithStartEndMarkers by Robin2 of the Arduino forums
// See  http://forum.arduino.cc/index.php?topic=288234.0
/*
****************************************
* Function recvWithStartEndMarkers
* reads serial data and returns the content between a start marker and an end marker.
* 
* passed:
*  
* global: 
*       receivedChars[]
*       newData
*
* Returns:
*          
* Sets:
*       newData
*       receivedChars
*
*
*  Lazy solution. I created 2 different versions. One to handle hardware serial and one to handle software serial
*/


void recvWithStartEndMarkersBT() 
{
     static boolean recvInProgressBT = false;
     static byte ndx = 0;
     char startMarker = '<';
     char endMarker = '>';
     char rc;
     
     if (BTserial.available() > 0) 
     { 
          rc = BTserial.read();
          if (recvInProgressBT == true) 
          {
               if (rc != endMarker) 
               {
                    receivedChars[ndx] = rc;
                    ndx++;
                    if (ndx >= numChars) { ndx = numChars - 1; }
               }
               else 
               {
                     receivedChars[ndx] = '\0'; // terminate the string
                     recvInProgressBT = false;
                     ndx = 0;
                     haveNewData = true;
               }
          }
          else if (rc == startMarker) { recvInProgressBT = true; }
     }
}




void recvWithStartEndMarkersUSB() 
{
     static boolean recvInProgress = false;
     static byte ndx = 0;
     char startMarker = '<';
     char endMarker = '>';
     char rc;

     if (Serial.available() > 0) 
     {
          rc = Serial.read();
          if (recvInProgress == true) 
          {
               if (rc != endMarker) 
               {
                    receivedChars[ndx] = rc;
                    ndx++;
                    if (ndx >= numChars) { ndx = numChars - 1; }
               }
               else 
               {
                     receivedChars[ndx] = '\0'; // terminate the string
                     recvInProgress = false;
                     ndx = 0;
                     haveNewData = true;
               }
          }
          else if (rc == startMarker) { recvInProgress = true; }
     }
}




