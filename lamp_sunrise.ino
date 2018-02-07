int AC_LOAD = D7;    // Output to Opto Triac pin
int dimming = 550;  // 550 is equivalent to ON, this is used for a delayMicroseconds()
int interruptPin = D3;
unsigned char i;
bool triggered = false;
bool alarm = false;

int onOff = 0;

// Timer check what time it is every 50 seconds
Timer minuteCheck(50000,checkTime);

// uncomment this to test triggering
Timer triggerOn(10000, triggOn, true);
Timer triggerOff(15000, triggOff, true);

void setup() {
    // put your setup code here, to run once:
    pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
    pinMode(interruptPin, INPUT);   // Set interruptPin (D3) to input 
    attachInterrupt(interruptPin, zero_crosss_int, RISING);
    digitalWrite(AC_LOAD, LOW);    // triac Off                     //test in high

    minuteCheck.start();
    
    // uncomment this to test triggering
    triggerOn.start();
    triggerOff.start();
    
    Time.zone(-5);
}

// uncomment this to test triggering
void triggOn(){
    dimming = 8000;
    triggered = true;
    onOff = 1;
}

void triggOff(){
    // dimming = 550;
    triggered = true;
    onOff = 0;
}

void checkTime(){
    int hora = Time.hourFormat12();
    int minuto = Time.minute(); 
    
    // Particle.publish("Time is: "+ String(hora) + " hours with "+ String(minuto) +" minutes");

    if(hora == 9 && minuto == 57){
        dimming = 8000;
        triggered = true;
        alarm = true;
    }
}

void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms 
  // Every zerocrossing : (50Hz)-> 10ms (1/2 Cycle) For 60Hz (1/2 Cycle) => 8.33ms 
  // 10ms=10000us
  
    if(triggered){

        int dimtime = dimming;
        delayMicroseconds(dimtime);    // Off cycle
        digitalWrite(AC_LOAD, HIGH);   // triac firing
        delayMicroseconds(10);         // triac On propogation delay (for 60Hz use 8.33)
        digitalWrite(AC_LOAD, LOW);    // triac Off
        
        if(alarm){
            sunrise();                        // call rise() function to change dimming value
        }
        else if(onOff == 1){
            turnOn();
        }
        else if(onOff == 0){
            turnOff();
        }
    }
    
}

void sunrise(){
    if(dimming > 550){
        dimming--;
    }
    if(dimming == 550){
        // this should run once
        triggered = false;
        alarm = false;
        digitalWrite(AC_LOAD, HIGH);    // triac ON
    }
}

void turnOn(){
    if(dimming > 550){
        dimming -= 50 ;
    }
    if(dimming == 550){
        // this should run once
        triggered = false;
        digitalWrite(AC_LOAD, HIGH);    // triac ON
        
    }
}

void turnOff(){
    if(dimming < 8000){
        dimming += 50 ;
    }
    if(dimming == 8000){
        // this should run once
        // dimming = 550;
        digitalWrite(AC_LOAD, LOW);    // triac OFF
        triggered = false;

    }
}

void loop() {}
