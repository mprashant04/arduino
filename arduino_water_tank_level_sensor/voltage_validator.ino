/*#define INPUT_VOLTAGE_SAMPLES_COUNT  10
float inputVoltage = -1;

void validateInputVoltage(){
    //ref:  https://startingelectronics.org/articles/arduino/measuring-voltage-with-arduino/
    
    int sum = 0;                    
    unsigned char sample_count = 0;   
    // take a number of analog samples and add them up
    while (sample_count < INPUT_VOLTAGE_SAMPLES_COUNT) {
        sum += analogRead(A1);
        sample_count++;
        delay(10);
    }
    
    inputVoltage = ((float)sum / (float)INPUT_VOLTAGE_SAMPLES_COUNT * 5) / 1024.0;
    inputVoltage = inputVoltage * 4.60;
    
    print(inputVoltage);
    println (" V");    

    lcdPrint(String(inputVoltage) + "v", 15,3);
}
*/
