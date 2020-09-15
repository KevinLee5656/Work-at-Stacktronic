#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "MPU6050.h"
#include "I2Cdev.h"

File myFile;
MPU6050 accelgyro;

const int MPU = 0x68; 
const int ledPin = 13;
const int accelRange [4] = {2,4,8,16};
const int gyroRange [4] = {250,500,1000,2000};

float gyroScale = 1000/pow(2,15); //Must change here and in setup
float accScale = 8/pow(2,15);    //Must change here and in setup

int16_t AccX,AccY,AccZ,GyroX,GyroY,GyroZ;
float faX,faY,faZ,fgX,fgY,fgZ;
unsigned long int time;

//Update after calibration
const int16_t accXOffset = -4052;
const int16_t accYOffset = -1416;
const int16_t accZOffset = 917;
const int16_t gyroXOffset = 142;
const int16_t gyroYOffset = 51;
const int16_t gyroZOffset = -21;

void setup() {
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.endTransmission(true);
    Serial.begin(9600); 
    pinMode(ledPin, OUTPUT);

    accelgyro.initialize();

    //~~ Set accelerometer and gyroscope offsets ~~
    accelgyro.setXAccelOffset(accXOffset);
    accelgyro.setYAccelOffset(accYOffset);
    accelgyro.setZAccelOffset(accZOffset);
 
    accelgyro.setXGyroOffset(gyroXOffset);
    accelgyro.setYGyroOffset(gyroYOffset);
    accelgyro.setZGyroOffset(gyroZOffset);
    
    //~~ Set accelerometer and gyroscope ranges ~~
    //Also possible to set XYZ ranges individually. See MPU6050 class functions.
    accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_1000); //MPU6050_GYRO_FS_250 defined in MPU6050.h, options: 250, 500, 1000, 2000
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8); //MPU6050_ACCEL_FS_2 defined in MPU6050.h, options: 2, 4, 8, 16

    while (!Serial) {
        ; 
    }
    Serial.print("Initializing SD card...");
    if (!SD.begin(10)) {
        Serial.println("initialization failed!");
        while (1);
    }
    Serial.println("Initialization complete.");

    myFile = SD.open("fileName.txt", FILE_WRITE);
    if (myFile) {
        Serial.print("Writing to test.txt...");
        
        myFile.print("Accelerometer and Gryoscope Range"); 
        myFile.print(accelRange[accelgyro.getFullScaleAccelRange()]); myFile.print(",");
        myFile.println(gyroRange[accelgyro.getFullScaleGyroRange()]);
        myFile.println("");

        myFile.close(); 

        Serial.println("File set-up complete");
    } else {
        Serial.println("Error opening file.");
    }

    //~~ Indicate setup is complete ~~
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(100);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(100);
}

void loop() {
    digitalWrite(ledPin, HIGH);

    //~~ Read milliseconds passed since program began running ~~
    time = millis();

    //~~ Read accelerometer data ~~
    accelgyro.getMotion6(&AccX,&AccY,&AccZ,&GyroX,&GyroY,&GyroZ); 

    if ( myFile = SD.open("fileName.txt", FILE_WRITE) ){  
        myFile.print(time); myFile.print(",");
        myFile.print(AccX); myFile.print(",");
        myFile.print(AccY); myFile.print(",");
        myFile.print(AccZ); myFile.print(",");
        myFile.print(GyroX); myFile.print(",");
        myFile.print(GyroY); myFile.print(",");
        myFile.println(GyroZ);
    
        myFile.close(); 
    } else {
        digitalWrite(ledPin, LOW);
    }

    if ( ((AccX == AccY == AccZ == 0)||(AccX == AccY == AccZ == -1)) 
    || ((GryoX == GyroY == GyroZ == 0)||(GyroX == GyroY == GyroZ == -1)) ){
        if (counter >= 100) digitalWrite(ledPin, LOW);
        else counter++;
    } else {
        counter = 0;
    }

    /*
    faX = float(AccX)* accScale;
    faY = float(AccY)* accScale;
    faZ = float(AccZ)* accScale;
    fgX = float(GyroX)* gyroScale;
    fgY = float(GyroY)* gyroScale;
    fgZ = float(GyroZ)* gyroScale; 

    // ~~~ for CSV ~~~
    Serial.print(time); Serial.print(",");
    Serial.print(AccX); Serial.print(",");
    Serial.print(AccY); Serial.print(",");
    Serial.print(AccZ); Serial.print(",");
    Serial.print(GyroX); Serial.print(",");
    Serial.print(GyroY); Serial.print(",");
    Serial.print(GyroZ); Serial.print(",");
    Serial.println("");

    // ~~~ for serial plotter ~~~
    Serial.print(faX); Serial.print(" ");
    Serial.print(faY); Serial.print(" ");
    Serial.print(faZ);  Serial.println(" ");
    Serial.print(fgX); Serial.print(" ");
    Serial.print(fgY); Serial.print(" ");
    Serial.print(fgZ); Serial.println(" ");    
    */
}

/*
    The output scale for any setting is [-32768, +32767] for each of the six axes

    Resources: 
        - MPU6050 Class Functions
            - https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html#abd8fc6c18adf158011118fbccc7e7054
*/
