const int numofCells = 4;
const int period = 20000; //18 minutes 
const float dutyPercent = 0.20;
const int dutyCycle = int(period*dutyPercent);

const int ledPin = 13;
const int master = 2;
const int pushButton = 10;
const int buzzer = 11;

const int cell_aPin_1 = A4;
const int cell_aPin_2 = A5;

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

bool startCycle; 
bool doneDischarging [numofCells] = {false}; 
float cellVoltage [numofCells];
double cellTemp [numofCells];

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

float getVoltage (int sensorValue){
    float voltage = (float(sensorValue) * (5.0/1023.0));
    if (voltage <= 2.5) {
        digitalWrite(0,LOW);
        indicateFault();
    }
    return voltage;
}

double getTemp (int sensorValue){
    //  1/T = 1/T0 + 1/B * ln( ( adcMax / adcVal ) – 1 )
    return (1 / (double(1/298.15) + double(1/4282) * log(5/(float(sensorValue) * (5.0/1023.0)) - 1)));
}


void readVoltandTemp (){
    cellTemp[0] = getTemp(analogRead(cell_aPin_1));
    cellTemp[2] = getTemp(analogRead(cell_aPin_2));
  
    digitalWrite(t1,HIGH);
    digitalWrite(t2,HIGH);
    delay(250);
    
    cellTemp[1] = getTemp(analogRead(cell_aPin_1));
    cellTemp[3] = getTemp(analogRead(cell_aPin_2));
    
    digitalWrite(t1,LOW);
    digitalWrite(t2,LOW);
}

void writeVoltandTemp(){
    Serial.print(millis()/1000.0); Serial.print(",");
    for (int i = 0; i < numofCells; i++){
        if (i == numofCells - 1){
            Serial.println(cellTemp[i]);
        } else {
            Serial.print(cellTemp[i]); Serial.print(",");
        }
    }
}

void indicateFault(){
    while(true){
        digitalWrite(master,LOW);
        tone(buzzer,850,400);
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
    
    pinMode(ledPin, OUTPUT);
    pinMode(master,OUTPUT);
    pinMode(pushButton,INPUT);
    
    pinMode(r1,OUTPUT);
    pinMode(r2,OUTPUT);
    pinMode(r3,OUTPUT);
    pinMode(r4,OUTPUT);
    
    pinMode(t1,OUTPUT);
    pinMode(t2,OUTPUT);

    Serial.begin(9600);
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
                readVoltandTemp();
                writeVoltandTemp();

                //~ Check if voltage of discharging cell <= 2.7 V
                if (doneDischarging[n-1] == false){
                    counter++;
                    doneDischarging[n-1] = true;
                }

                delay(750);
                time = millis();         
            } 
        }
        while (millis() < periodTimer){
            digitalWrite(master,LOW);
            readVoltandTemp();
            writeVoltandTemp();
            delay(750);
        }
    } 
    
    else if ( (counter < numofCells) && !startCycle ){
        digitalWrite(master,LOW);
        digitalWrite(ledPin, LOW);
        if (digitalRead(pushButton) == HIGH) startCycle = true;
    }  
  
    // Turn off LED to indicate done modelling
    else {
        digitalWrite(master,LOW);
        digitalWrite(ledPin, LOW);

        delay(600000);
    }
}
