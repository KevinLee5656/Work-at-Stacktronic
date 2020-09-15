const int numofCells = 10;
const int period = 3600000; //1 hour
const float dutyPercent = 0.1;
const int dutyCycle = int(period*dutyPercent);

int counter;
unsigned long int time; 
unsigned long int dischargeTimer; 
unsigned long int periodTimer;  
unsigned int startTime;
bool startCycle;

//Cell data variables
float current; 
bool doneDischarging [numofCells] = {false}; 
float cellVoltage [numofCells];
float cellTemp [numofCells];

const int ledPin = 13;
const int pushbutton = 51;
const int buzzer = 53;

//Master mosfet for controling cell discharging circuit (DC load circuit) 
const int master = 2;

//Mosfets for individual cell discharging
const int r1 = 3; 
const int r2 = 4; 
const int r3 = 5; 
const int r4 = 6; 
const int r5 = 7; 
const int r6 = 8; 
const int r7 = 9; 
const int r8 = 10; 
const int r9 = 11;
const int r10 = 12;

//Mosfets for cell temperature (thermistor ciruit)
const int t1 = 22; 
const int t2 = 24; 
const int t3 = 26; 
const int t4 = 28; 
const int t5 = 30;


//Analog input pins for reading cell voltage
const int cell_aPin_1 = A0;
const int cell_aPin_2 = A1;
const int cell_aPin_3 = A2;
const int cell_aPin_4 = A3;
const int cell_aPin_5 = A4;
const int cell_aPin_6 = A5;
const int cell_aPin_7 = A6;
const int cell_aPin_8 = A7;
const int cell_aPin_9 = A8;
const int cell_aPin_10 = A9;

//Analog input pins for reading thermistor circuit/temperature
const int temp_aPin_1 = A10; 
const int temp_aPin_2 = A11; 
const int temp_aPin_3 = A12; 
const int temp_aPin_4 = A13;
const int temp_aPin_5 = A14;

//Analog input pin for Hall effect sensor
const int current_aPin = A15;


//Coded with normally open relay
void changeRelayConfig(int n){
    switch (n){
        case 1: {
            digitalWrite(r1,HIGH);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 2:{
            digitalWrite(r1,LOW);
            digitalWrite(r2,HIGH);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 3: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,HIGH);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 4: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,HIGH);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 5: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,HIGH);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 6: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,HIGH);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 7: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,HIGH);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 8: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,HIGH);
            digitalWrite(r9,LOW);
            digitalWrite(r10,LOW);
            break;
        }
        case 9: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,HIGH);
            digitalWrite(r10,LOW);
            break;
        }
        case 10: {
            digitalWrite(r1,LOW);
            digitalWrite(r2,LOW);
            digitalWrite(r3,LOW);
            digitalWrite(r4,LOW);
            digitalWrite(r5,LOW);
            digitalWrite(r6,LOW);
            digitalWrite(r7,LOW);
            digitalWrite(r8,LOW);
            digitalWrite(r9,LOW);
            digitalWrite(r10,HIGH);
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
    float temp = (1 / (float(1/298.15) + float(1/3933) * log(5/readVoltage(sensorValue) - 1)));
    if (temp >= 323.15) {
        digitalWrite(master,LOW);
        indicateFault();
    }
    return temp;
}

//Might need to calibrate offset
void readCurrent(){
    //current = (voltage - zero-current output voltage) * sensitivity(66mV/A)
    current = (analogRead(current_aPin) * float(5/1023) - 2.5) * float(66/1000);
}

void readVoltandTemp (){
    cellVoltage[0] = getVoltage(analogRead(cell_aPin_1));
    cellVoltage[1] = getVoltage(analogRead(cell_aPin_2));
    cellVoltage[2] = getVoltage(analogRead(cell_aPin_3));
    cellVoltage[3] = getVoltage(analogRead(cell_aPin_4));
    cellVoltage[4] = getVoltage(analogRead(cell_aPin_5));
    cellVoltage[5] = getVoltage(analogRead(cell_aPin_6));
    cellVoltage[6] = getVoltage(analogRead(cell_aPin_7));
    cellVoltage[7] = getVoltage(analogRead(cell_aPin_8));
    cellVoltage[8] = getVoltage(analogRead(cell_aPin_9));
    cellVoltage[9] = getVoltage(analogRead(cell_aPin_10));
    cellTemp[0] = getTemp(analogRead(temp_aPin_1));
    cellTemp[2] = getTemp(analogRead(temp_aPin_2));
    cellTemp[4] = getTemp(analogRead(temp_aPin_3));
    cellTemp[6] = getTemp(analogRead(temp_aPin_4));
    cellTemp[8] = getTemp(analogRead(temp_aPin_5));

    digitalWrite(t1,HIGH);
    digitalWrite(t2,HIGH);
    digitalWrite(t3,HIGH);
    digitalWrite(t4,HIGH);
    digitalWrite(t5,HIGH);
    delay(250);

    cellTemp[1] = getTemp(analogRead(temp_aPin_1));
    cellTemp[3] = getTemp(analogRead(temp_aPin_2));
    cellTemp[5] = getTemp(analogRead(temp_aPin_3));
    cellTemp[7] = getTemp(analogRead(temp_aPin_4));
    cellTemp[9] = getTemp(analogRead(temp_aPin_5));

    digitalWrite(t1,LOW);
    digitalWrite(t2,LOW);
    digitalWrite(t3,LOW);
    digitalWrite(t4,LOW);
    digitalWrite(t5,LOW);
}

void writeData(int dischargingCell){
    Serial.print(millis()/1000.0); Serial.print(",");

    for (int i = 0; i < numofCells; i++){
        if (i == dischargingCell) {
            Serial.print(current); Serial.print(",");
        } else {
            Serial.print(0); Serial.print(",");
        }        

        
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
        tone(buzzer,850,750);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
    }
}


void setup(){
    if (dutyCycle*numofCells > period) indicateFault();
    
    counter = 0; 
    startCycle = false;
    
    pinMode(ledPin,OUTPUT);
    pinMode(master,OUTPUT);
    pinMode(pushbutton,INPUT);

    pinMode(r1,OUTPUT);
    pinMode(r2,OUTPUT);
    pinMode(r3,OUTPUT);
    pinMode(r4,OUTPUT);
    pinMode(r5,OUTPUT);
    pinMode(r6,OUTPUT);
    pinMode(r7,OUTPUT);
    pinMode(r8,OUTPUT);
    pinMode(r9,OUTPUT);
    pinMode(r10,OUTPUT);
    
    pinMode(t1,OUTPUT);
    pinMode(t2,OUTPUT);
    pinMode(t3,OUTPUT);
    pinMode(t4,OUTPUT);
    pinMode(t5,OUTPUT);


    Serial.begin(9600);
    Serial.println("LABEL,time,C1,V1,T1,C2,V2,T2,C3,V3,T3,C4,V4,T4,C5,V5,T5,C6,V6,T6,C7,V7,T7,C8,V8,T8,C9,V9,T9,C10,V10,T10");

}

//loop = one period
void loop(){
    //~ Check if all cells are done discharging 
    if ( (counter < numofCells) && startCycle ){
        digitalWrite(ledPin, HIGH);
        periodTimer = millis() + period; 

        //each loop = one duty cycle
        for (int n = 1; n <= numofCells; n++){
            digitalWrite(master,LOW);
            changeRelayConfig(n); 
        
            if (!doneDischarging[n-1]) digitalWrite(master,HIGH);

            time = millis(); 
            dischargeTimer = time + dutyCycle; 

            while (time + 500 < dischargeTimer){
                //~ Read voltage and temperature of all cells
                readCurrent();
                readVoltandTemp();
                writeData(n-1);

                //~ Check if voltage of discharging cell <= 2.7 V
                if ( cellVoltage[n-1] <= 2.7 ){
                    digitalWrite(master,LOW);
                    if (doneDischarging[n-1] == false){
                        counter++;
                    }
                    doneDischarging[n-1] = true;
                }

                delay(650);
                time = millis();         
            } 
        }
        while (millis() < periodTimer){
            digitalWrite(master,LOW);
            readCurrent();
            readVoltandTemp();
            writeData(-1);
            delay(650);
        }
    } 
    
   else if ( (counter < numofCells) && !startCycle ){
        digitalWrite(master,LOW);
        digitalWrite(ledPin, LOW);
        if (digitalRead(pushbutton) == HIGH) startCycle = true; 
    }

    // Turn off LED to indicate done modelling
    else {
        digitalWrite(master,LOW);
        digitalWrite(ledPin, LOW);

        delay(10000000000);
    }
}

/*
    Writing Arduino data to Excel Options
        - PLX-DAQ network: http://www.filipposfactory.com/index.php/8-arduino/25-arduino-and-excel
        - Data Streamer in Excel: https://create.arduino.cc/projecthub/HackingSTEM/stream-data-from-arduino-into-excel-f1bede

*/
