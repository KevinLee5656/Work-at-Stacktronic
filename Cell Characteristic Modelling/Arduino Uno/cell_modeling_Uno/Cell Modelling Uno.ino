const int numofCells = 4;
const int period = 1080000; //18 minutes 
const float dutyPercent = 0.25;
const int dutyCycle = int(period*dutyPercent);

const int ledPin = 13;
const int master = 2;

const int r1 = 3; 
const int r2 = 4; 
const int r3 = 5; 
const int r4 = 6; 

const int t1 = 7; 
const int t2 = 8;

int counter;
unsigned long int time; 
unsigned long int dischargeTimer; 
unsigned long int periodTimer; 

bool doneDischarging [numofCells] = {false}; 
float current; 
float cellVoltage [numofCells];
float cellTemp [numofCells];

//Coded with normally open relay
void changeRelayConfig(int n){
    switch (n){
case 1: {
            digitalWrite(r1,HIGH);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            break;
        }
        case 2:{
            digitalWrite(r1,LOW);
            digitalWrite(r2,HIGH);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            break;
        }
        case 3: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,HIGH);
            digitalWrite(r4,LOW);
            break;
        }
        case 4: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,HIGH);
            break;
        }
    }
}

float readVoltage(int sensorValue){
    return float(sensorValue) * (5.0/1023.0);
}

float getVoltage (int sensorValue){
    float voltage = readVoltage(sensorValue);
    if (voltage <= 2.5) {
        digitalWrite(master,LOW);
        indicateFault();
    }
    return voltage;
}

float getTemp (int sensorValue){
    //  1/T = 1/T0 + 1/B * ln( ( adcMax / adcVal ) – 1 )
    float temp = (1 / (float(1/298.15) + float(1/4282) * log(5/readVoltage(sensorValue) - 1)));
    if (temp >= 323.15) {
        digitalWrite(master,LOW);
        indicateFault();
    }
    return temp;
}

void readVoltandTemp (){
    cellVoltage[0] = getVoltage(analogRead(A0));
    cellVoltage[1] = getVoltage(analogRead(A1));
    cellVoltage[2] = getVoltage(analogRead(A2));
    cellVoltage[3] = getVoltage(analogRead(A3));
    
    cellTemp[0] = getTemp(analogRead(A4));
    cellTemp[2] = getTemp(analogRead(A5));

    digitalWrite(t1,HIGH);
    digitalWrite(t2,HIGH);
    delay(250);

    cellTemp[1] = getTemp(analogRead(A4));
    cellTemp[3] = getTemp(analogRead(A5));

    digitalWrite(t1,LOW);
    digitalWrite(t2,LOW);
}

//Might need to calibrate an offset
void readCurrent(){
    //current = (voltage - zero-current output voltage) * sensitivity(=66mV/A) 
    //current = (analogRead(A15) * float(5/1023) - 2.5) * float(66/1000);
}

void writeData(){
    Serial.print(millis()/1000.0); Serial.print(",");
    Serial.print(current); Serial.print(",");
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
        digitalWrite(master,LOW);
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
        delay(1000);
    }
}

void setup(){
    if (dutyCycle*numofCells > period) indicateFault();

    counter = 0;
    
    pinMode(ledPin, OUTPUT);
    pinMode(master,OUTPUT);
    
    pinMode(r1,OUTPUT);
    pinMode(r2,OUTPUT);
    pinMode(r3,OUTPUT);
    pinMode(r4,OUTPUT);
    
    pinMode(t1,OUTPUT);
    pinMode(t2,OUTPUT);

    Serial.begin(9600);
    Serial.println("LABEL,time,V1,T1,V2,T2,V3,T3"); 
}

//loop = one period
void loop(){
    //~ Check if all cells are done discharging 
    if (counter < numofCells){
        digitalWrite(ledPin, HIGH);
        periodTimer = millis() + period; 

        //each loop = one duty cycle
        for (int n = 1; n <= numofCells; n++){
            digitalWrite(master,LOW);
            changeRelayConfig(n); 
        
            if (!doneDischarging[n-1]) digitalWrite(master,HIGH);

            time = millis(); 
            dischargeTimer = time + dutyCycle; 

            while ( (time + 500) < dischargeTimer){
                //~ Read voltage and temperature of all cells
                readCurrent();
                readVoltandTemp();
                writeData();

                //~ Check if voltage of discharging cell <= 2.7 V
                if ( cellVoltage[n-1] <= 2.7 ){
                    digitalWrite(master,LOW);
                    if (doneDischarging[n-1] == false){
                        counter++;
                    }
                    doneDischarging[n-1] = true;
                }

                delay(750);
                time = millis();         
            } 
        }
        while ( (millis() + 500) < periodTimer){
            digitalWrite(master,LOW);
            readCurrent();
            readVoltandTemp();
            writeData();
            delay(750);
        }
    } 
    
    // Turn off LED to indicate done modelling
    else {
        digitalWrite(master,LOW);
        digitalWrite(ledPin, LOW);

        delay(10000000000);
    }
}

/*
    Writing Arduino data to Excel 
        - PLX-DAQ network: http://www.filipposfactory.com/index.php/8-arduino/25-arduino-and-excel
        - Data Streamer in Excel: https://create.arduino.cc/projecthub/HackingSTEM/stream-data-from-arduino-into-excel-f1bede

*/