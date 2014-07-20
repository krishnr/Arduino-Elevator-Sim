int currentFloor = 0;
int destinationFloor = 0;
const int floor0Pin = 2;
const int floor1Pin = 3;
const int floor2Pin = 4;
const int floor3Pin = 5;
const int floor0LED = 10;
const int floor1LED = 11;
const int floor2LED = 12;
const int floor3LED = 13;
int prescaler = 64;
float counter_freq;
volatile int overflowCount=0;
volatile int millisCount=0;
volatile float fraction = 0;
volatile unsigned long m = 0;

void setup() {
  pinMode(floor0Pin, INPUT_PULLUP);
  pinMode(floor1Pin, INPUT_PULLUP);
  pinMode(floor2Pin, INPUT_PULLUP);
  pinMode(floor3Pin, INPUT_PULLUP);
  pinMode(floor0LED, OUTPUT);
  pinMode(floor1LED, OUTPUT);
  pinMode(floor2LED, OUTPUT);
  pinMode(floor3LED, OUTPUT);
  counter_freq = 16e6/64;


  TIMSK1=0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1=0x0BDC; // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x04; // start timer/ set clock
  
  Serial.begin(9600); 
  
}

void loop() {
  
  digitalWrite(currentFloor+10, HIGH);
  
  
  while(digitalRead(floor0Pin)==HIGH && digitalRead(floor1Pin)==HIGH && digitalRead(floor2Pin)==HIGH && digitalRead(floor3Pin)==HIGH) {
    if(digitalRead(floor0Pin)==LOW){
      destinationFloor=0;
      Serial.println("Going to floor 0");
    }else if(digitalRead(floor1Pin)==LOW){
      destinationFloor=1;
      Serial.println("Going to floor 1");
    }else if(digitalRead(floor2Pin)==LOW){
      destinationFloor=2;
      Serial.println("Going to floor 2");
    }else if(digitalRead(floor3Pin)==LOW){
      destinationFloor=3;
      Serial.println("Going to floor 3");
    }
  }
  
  if(destinationFloor != currentFloor) {
    //if going up
    if(destinationFloor-1 > currentFloor) {
      
      for(int i = currentFloor; i < destinationFloor; i++) {
        //wait for travel time
        digitalWrite(i+9, LOW);
        pause(1000);
        digitalWrite(i+10, HIGH);
        Serial.print("On floor ");
        Serial.print(i);
        Serial.println();
        pause(1000);
      }
    }  
    
    //if going down
    if(destinationFloor-1 < currentFloor) {
      
      for(int i = currentFloor; i > destinationFloor; i--) {
        //wait for travel time
        digitalWrite(i+11, LOW);
        pause(1000);
        digitalWrite(i+10, HIGH);
        Serial.print("On floor ");
        Serial.println(i);
        pause(1000);
      }
    } 
    
    digitalWrite(destinationFloor+9, LOW);
    digitalWrite(destinationFloor+11, LOW);
    pause(1000);
    Serial.print("Arrived at ");
    Serial.println(destinationFloor);
    Serial.println(time());
    
    //open and close door
    
    currentFloor = destinationFloor;
  }
}

ISR(TIMER1_OVF_vect)
{
  
    TCNT1=0x0BDC; 
    overflowCount++;
}
    
unsigned long time()
{
    unsigned long n;
    uint8_t oldSREG = SREG;
 
    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    n = TCNT1 + (overflowCount * 62500);
    n = n/62500.0*1000.0;
    SREG = oldSREG; 
    
    
    return n;
}

void pause(int delayTime)
{
  int startTime = time();
  
  while(time() <= startTime+delayTime) {}

  return;
}  
