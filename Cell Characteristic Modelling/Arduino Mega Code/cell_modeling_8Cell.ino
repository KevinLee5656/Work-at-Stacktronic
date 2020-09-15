//---NOT UPDATED---//

const int numofCells = 8;
const int period = 2880000; //48 minutes 
const float dutyPercent = 0.125;
const int dutyCycle = int(period*dutyPercent);

const int ledPin = 13;

int counter;
unsigned long int time; 
unsigned long int dischargeTimer; 
unsigned long int periodTimer;  

bool doneDischarging [numofCells] = {false}; 
float cellVoltage [numofCells];
float cellTemp [numofCells];

//Coded with normally open relay
void changeRelayConfig(int n){
    switch (n){
        case 1: {
            digitalWrite(1,HIGH);
            digitalWrite(2,HIGH);
            digitalWrite(3,LOW);
            digitalWrite(4,HIGH);
        }
        case 2:{
            digitalWrite(1,HIGH);
            digitalWrite(2,HIGH);
            digitalWrite(3,LOW);
            digitalWrite(4,LOW);
        }
        case 3: {
            digitalWrite(1,HIGH);
            digitalWrite(2,LOW);
            digitalWrite(3,LOW);
            digitalWrite(4,HIGH);
        }
        case 4: {
            digitalWrite(1,HIGH);
            digitalWrite(2,LOW);
            digitalWrite(3,LOW);
            digitalWrite(4,LOW);
        }
        case 5: {
            digitalWrite(1,LOW);
            digitalWrite(2,HIGH);
            digitalWrite(3,HIGH);
            digitalWrite(4,LOW);
        }
        case 6: {
            digitalWrite(1,LOW);
            digitalWrite(2,HIGH);
            digitalWrite(3,LOW);
            digitalWrite(4,LOW);
        }
        case 7: {
            digitalWrite(1,LOW);
            digitalWrite(2,LOW);
            digitalWrite(3,HIGH);
            digitalWrite(4,LOW);
        }
        case 8: {
            digitalWrite(1,LOW);
            digitalWrite(2,LOW);
            digitalWrite(3,LOW);
            digitalWrite(4,LOW);
        }
    }
}

float getVoltage (int sensorValue){
    float voltage = (float(sensorValue) * (5.0/1023.0));
    if (voltage < 2.5) {
        digitalWrite(0,LOW);
        indicateFault();
    }
    return voltage;
}

float getTemp (int sensorValue){
    //  1/T = 1/T0 + 1/B * ln( ( adcMax / adcVal ) – 1 )
    return (1 / (float(1/298.15) + float(1/4282) * std::log(5/getVoltage(sensorValue) - 1)));
}

void readVoltandTemp (){
    cellVoltage[0] = getVoltage(analogRead(A0));
    cellTemp[0] = getTemp(analogRead(A1));
    cellVoltage[1] = getVoltage(analogRead(A2));
    cellTemp[1] = getTemp(analogRead(A3));
    cellVoltage[2] = getVoltage(analogRead(A4));
    cellTemp[2] = getTemp(analogRead(A5));
    cellVoltage[3] = getVoltage(analogRead(A6));
    cellTemp[3] = getTemp(analogRead(A7));
    cellVoltage[4] = getVoltage(analogRead(A8));
    cellTemp[4] = getTemp(analogRead(A9));
    cellVoltage[5] = getVoltage(analogRead(A10));
    cellTemp[5] = getTemp(analogRead(A11));
    cellVoltage[6] = getVoltage(analogRead(A12));
    cellTemp[6] = getTemp(analogRead(A13));
    cellVoltage[7] = getVoltage(analogRead(A14));
    cellTemp[7] = getTemp(analogRead(A15));
}

void writeVoltandTemp(){
    Serial.print(time/1000); Serial.print(",");
    for (int i = 0; i < numofCells; i++){
        if (i == numofCells - 1){
            Serial.print(cellVoltage[i]); Serial.print(",");
            Serial.println(cellTemp[i]);
        } else {
            Serial.print(cellVoltage[i]); Serial.print(","); 
            Serial.print(cellTemp[i]); Serial.print(",");
        }
    }
}

void indicateFault(){
    while(true){
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
    }
}


void setup(){
    if (dutyCycle*numofCells > period) indicateFault();
    analogReference(EXTERNAL);

    counter = 0; 
    
    pinMode(0,OUTPUT);
    pinMode(1,OUTPUT);
    pinMode(2,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);

    Serial.begin(9600);
    Serial.println("LABEL,time,V1,T1,V2,T2,V3,T3,V4,T4,V5,T5,V6,T6,V7,T7,V8,T8");

}

//loop = one period
void loop(){
    //~ Check if all cells are done discharging 
    if (counter < numofCells){
        digitalWrite(ledPin, HIGH);
        periodTimer = millis() + period; 

        //each loop = one duty cycle
        for (int n = 1; n <= numofCells; n++){
            digitalWrite(0,LOW);
            changeRelayConfig(n); 
        
            if (!doneDischarging[n-1]) digitalWrite(0,HIGH);

            time = millis(); 
            dischargeTimer = time + dutyCycle; 

            while (time < dischargeTimer){
                //~ Read voltage and temperature of all cells
                readVoltandTemp();
                writeVoltandTemp();

                //~ Check if voltage of discharging cell <= 2.7 V
                if ( cellVoltage[n-1] <= 2.7 ){
                    digitalWrite(0,LOW);
                    doneDischarging[n-1] = true;
                    counter++;
                }

                delay(1000);
                time = millis();         
            } 
        }
        while (millis() < periodTimer){
            digitalWrite(0,LOW);
            readVoltandTemp();
            writeVoltandTemp();
        }
    } 
    
    // Turn off LED to indicate done modelling
    else {
        digitalWrite(0,LOW);
        digitalWrite(ledPin, LOW);

        delay(1000000);
    }
}

/*
    Writing Arduino data to Excel 
        - PLX-DAQ network: http://www.filipposfactory.com/index.php/8-arduino/25-arduino-and-excel
        - Data Streamer in Excel: https://create.arduino.cc/projecthub/HackingSTEM/stream-data-from-arduino-into-excel-f1bede

*/