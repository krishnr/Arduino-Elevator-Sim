int currentFloor = 0;
int destinationFloor = 0;
const int floor0Pin = 4;
const int floor1Pin = 5;
const int floor2Pin = 6;
const int floor3Pin = 7;
const int floor0LED = 10;
const int floor1LED = 11;
const int floor2LED = 12;
const int floor3LED = 13;
const int doorClosePin = 2;
const int doorOpenPin = 3;
const int expressPin = 8;
const int emergencyStopPin = 0;
volatile int overflowCount=0;
const int difference = floor0LED;
const int pauseTime = 750;
const int doorTime = 500;

void setup() {
  pinMode(floor0Pin, INPUT_PULLUP);
  pinMode(floor1Pin, INPUT_PULLUP);
  pinMode(floor2Pin, INPUT_PULLUP);
  pinMode(floor3Pin, INPUT_PULLUP);
  pinMode(floor0LED, OUTPUT);
  pinMode(floor1LED, OUTPUT);
  pinMode(floor2LED, OUTPUT);
  pinMode(floor3LED, OUTPUT);
  
  digitalWrite(doorClosePin, HIGH);
  attachInterrupt(0, doorClose, CHANGE);
  
  digitalWrite(doorOpenPin, HIGH);
  attachInterrupt(1, doorOpen, CHANGE);
  
  digitalWrite(expressPin, HIGH);
  PCICR |= _BV(PCIE0);
  PCMSK0 = _BV(PCINT0);
  
  digitalWrite(emergencyStopPin, HIGH);
  PCICR |= _BV(PCIE2);
  PCMSK2 = _BV(PCINT16);
  

  TIMSK1=0x01; // enabled global and timer overflow interrupt;
  TCCR1A = 0x00; // normal operation mode
  TCNT1=0x0BDC; // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x04; // start timer/ set clock
  
  Serial.begin(9600); 
  
}

void loop() {
  
  
  digitalWrite(currentFloor+difference, HIGH);
  long startTime = time();
  
    if(digitalRead(floor0Pin)==LOW){
      destinationFloor=0;
      if(destinationFloor != currentFloor){
        Serial.println("Going to floor 0");
      }
    }else if(digitalRead(floor1Pin)==LOW){
      destinationFloor=1;
      if(destinationFloor != currentFloor){
        Serial.println("Going to floor 1");
      }
    }else if(digitalRead(floor2Pin)==LOW){
      destinationFloor=2;
      if(destinationFloor != currentFloor){
        Serial.println("Going to floor 2");
      }
    }else if(digitalRead(floor3Pin)==LOW){
      destinationFloor=3;
      if(destinationFloor != currentFloor){
        Serial.println("Going to floor 3");
      }
    }
  
  if(destinationFloor != currentFloor) {
    //if going up
    if(destinationFloor-1 > currentFloor) {
      
      for(int i = currentFloor; i < destinationFloor; i++) {
        //wait for travel time
        digitalWrite(i+difference-1, LOW);
        pause(750);
        digitalWrite(i+difference, HIGH);
        Serial.print("On floor ");
        Serial.print(i);
        Serial.println();
        pause(pauseTime);
      }
    }  
    
    //if going down
    if(destinationFloor-1 < currentFloor) {
      
      for(int i = currentFloor; i > destinationFloor; i--) {
        //wait for travel time
        digitalWrite(i+difference+1, LOW);
        pause(pauseTime);
        digitalWrite(i+difference, HIGH);
        Serial.print("On floor ");
        Serial.println(i);
        pause(pauseTime);
      }
    } 
    
    digitalWrite(destinationFloor + difference -1, LOW);
    digitalWrite(destinationFloor + difference +1, LOW);
    pause(pauseTime);
    Serial.print("Arrived at floor ");
    Serial.println(destinationFloor);
    Serial.print("Your total trip time was ");
    Serial.print((time() - startTime)/1000.0);
    Serial.println(" seconds.");
    Serial.println();
    
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
    sei();
    
    return n;
}

void pause(int delayTime)
{
  long startTime = time();
  
  while(time() <= startTime+delayTime) {}

  return;
}  

void doorClose() {
  
}

void doorOpen() {
  
}

ISR (PCINT0_vect) {

}

ISR (PCINT2_vect) {
  
}
