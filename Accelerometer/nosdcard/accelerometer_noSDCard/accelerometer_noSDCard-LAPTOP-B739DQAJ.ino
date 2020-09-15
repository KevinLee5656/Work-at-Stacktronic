#include <Wire.h>
#include "MPU6050.h"
#include "I2Cdev.h"

const int MPU = 0x68; 
const int ledPin = 13;
int16_t AccX,AccY,AccZ,GyroX,GyroY,GyroZ;
float faX,faY,faZ,fgX,fgY,fgZ;
unsigned long int time; 
MPU6050 accelgyro;

float gyroScale = 1000/pow(2,15); //Must change here and in setup
float accScale = 8/pow(2,15);    //Must change here and in setup

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

    accelgyro.initialize();

    //Set accelerometer and gyroscope offsets
    accelgyro.setXAccelOffset(accXOffset);
    accelgyro.setYAccelOffset(accYOffset);
    accelgyro.setZAccelOffset(accZOffset);
 
    accelgyro.setXGyroOffset(gyroXOffset);
    accelgyro.setYGyroOffset(gyroYOffset);
    accelgyro.setZGyroOffset(gyroZOffset);
    
    //Set accelerometer and gyroscope ranges
    //Also possible to set XYZ ranges individually. See MPU6050 class functions.
    accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_1000); //MPU6050_GYRO_FS_250 defined in MPU6050.h, options: 250, 500, 1000, 2000
    accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_8); //MPU6050_ACCEL_FS_2 defined in MPU6050.h, options: 2, 4, 8, 16


    //Indicate setup is complete
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

    // ~~~ for CSV ~~~
//    Serial.println("Time, Xacc, Yacc, Zacc, Xgyro, Ygyro, Zgyro");
}

void loop() {
    digitalWrite(ledPin, HIGH);

    //Read milliseconds passed since program began running
    time = millis();

    //Read accelerometer data
    accelgyro.getMotion6(&AccX,&AccY,&AccZ,&GyroX,&GyroY,&GyroZ); 
    faX = float(AccX)* accScale;
    faY = float(AccY)* accScale;
    faZ = float(AccZ)* accScale;
    fgX = float(GyroX)* gyroScale;
    fgY = float(GyroY)* gyroScale;
    fgZ = float(GyroZ)* gyroScale;  
      
    // ~~~ for CSV ~~~
//    Serial.print(time); Serial.print(",");
//    Serial.print(AccX); Serial.print(",");
//    Serial.print(AccY); Serial.print(",");
//    Serial.print(AccZ); Serial.print(",");
//    Serial.print(GyroX); Serial.print(",");
//    Serial.print(GyroY); Serial.print(",");
//    Serial.print(GyroZ); Serial.print(",");
//    Serial.println("");


// ~~~ for serial plotter ~~~
    Serial.print(faX); Serial.print(" ");
    Serial.print(faY); Serial.print(" ");
    Serial.print(faZ);  Serial.println(" ");
//    Serial.print(fgX); Serial.print(" ");
//    Serial.print(fgY); Serial.print(" ");
//    Serial.print(fgZ); Serial.println(" ");    
}

/*
    The output scale for any setting is [-32768, +32767] for each of the six axes

    Resources: 
        - MPU6050 Class Functions
            - https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html#abd8fc6c18adf158011118fbccc7e7054
*/
