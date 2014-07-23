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
const int speakerPin = 9;
volatile int overflowCount=0;
const int difference = floor0LED;
volatile int pauseTime = 1000;
volatile int doorTime = 800;
volatile boolean isStopped = false;
boolean isPrinted = false;
const int doorSize = 5;
const int doorLight[] = {A0, A1, A2, A3, A4, A5};
volatile boolean isDoorClosePressed = false;
volatile boolean isDoorOpenPressed = false;
volatile long doorOpenTime = 0;
volatile long doorCloseTime = 0;
volatile boolean isFirstTime = true;
volatile long startTime = 0;
volatile boolean hasMoved = false;
volatile boolean isPrintedOnce = false;

void setup() {
  pinMode(floor0Pin, INPUT_PULLUP);
  pinMode(floor1Pin, INPUT_PULLUP);
  pinMode(floor2Pin, INPUT_PULLUP);
  pinMode(floor3Pin, INPUT_PULLUP);
  pinMode(floor0LED, OUTPUT);
  pinMode(floor1LED, OUTPUT);
  pinMode(floor2LED, OUTPUT);
  pinMode(floor3LED, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);

  attachInterrupt(0, doorCloseInterrupt, CHANGE);
  

  attachInterrupt(1, doorOpenInterrupt, CHANGE);
  
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
  
  digitalWrite(currentFloor+difference, HIGH);
  
  
}

void loop() {
  
  
  if(!isStopped) {
  
  digitalWrite(currentFloor+difference, HIGH);
  
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
  
  if(isDoorOpenPressed) {
    isDoorOpenPressed = false;
    openDoor();
  }else if(isDoorClosePressed){
    isDoorClosePressed = false;
    closeDoor();
  }
  
  if(destinationFloor != currentFloor) {
    
    startTime = time();
    hasMoved=true;
    isPrintedOnce = false;
    
    //if going up
    if(destinationFloor-1 > currentFloor) {
      
      for(int i = currentFloor; i < destinationFloor && !isStopped; i++) {
        //wait for travel time
        digitalWrite(i+difference-1, LOW);
        pause(pauseTime);
        digitalWrite(i+difference, HIGH);
        Serial.print("On floor ");
        Serial.print(i);
        Serial.println();
        pause(pauseTime);
      }
    }  
    
    //if going down
    if(destinationFloor-1 < currentFloor) {
      
      for(int i = currentFloor; i > destinationFloor && !isStopped; i--) {
        //wait for travel time
        
        digitalWrite(i+difference+1, LOW);
        pause(pauseTime);
        digitalWrite(i+difference, HIGH);
        Serial.print("On floor ");
        Serial.println(i);
        pause(pauseTime);
      }
    } 
    
   if(!isStopped){
      digitalWrite(destinationFloor + difference -1, LOW);
      digitalWrite(destinationFloor + difference +1, LOW);
      pause(pauseTime);
      digitalWrite(destinationFloor + difference, HIGH);
      Serial.print("Arrived at floor ");
      Serial.println(destinationFloor);
      tone(speakerPin, 1000, 100);
      pause(200);
      tone(speakerPin, 1000, 100);
      openDoor();
      hasMoved = false;
   }
  }
    
    //open and close door
    currentFloor = destinationFloor;

    } else if(!isPrinted) {
      Serial.println("EMERGENCY STOP!");
      openDoor();
      Serial.println();
      
      isPrinted = true;
    } else {
      tone(9, 2000, 100);
      pause(200);
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

void doorCloseInterrupt() {
  isDoorClosePressed = true;
}

void doorOpenInterrupt() {
  isDoorOpenPressed = true;
}

ISR (PCINT0_vect) {
  pauseTime *= 0.85;
  doorTime *= 0.85;
}

ISR (PCINT2_vect) {
  isStopped = true;
}

void openDoor() {
   
  isDoorClosePressed = false;
  isDoorOpenPressed = false;

   
   if(isFirstTime){
     doorOpenTime = time();
     isFirstTime=false;
   }
  
  for(int i = doorSize/2; i >= 0; i--) {
     

     digitalWrite(doorLight[i], LOW);
     digitalWrite(doorLight[doorSize-i], LOW);
     

    if(isDoorClosePressed) {
      isDoorClosePressed = false;
      isDoorOpenPressed = false;
      break;
     }
   pause(doorTime);  
 }
 
 if(!isStopped){
  pause(1000);
  closeDoor();
 }


}
 
void closeDoor() {
  
  isDoorClosePressed = false;
  isDoorOpenPressed = false;
  
  for(int i = 0; i <= doorSize/2; i++) {
    
    digitalWrite(doorLight[i], HIGH);
    digitalWrite(doorLight[doorSize-i], HIGH);
    pause(doorTime);
    if(isDoorOpenPressed) {
      isDoorClosePressed=false;
      isDoorOpenPressed = false;
      openDoor();
      break;
    }
  }
    
    isFirstTime = true;
    doorCloseTime = time();
    if(!isPrintedOnce) {
      Serial.print("The door was open for a total of ");
      Serial.print((doorCloseTime - doorOpenTime)/1000.0);
      Serial.println(" seconds.");
      isPrintedOnce = true;
    if(hasMoved) {
      Serial.print("Your total trip time was ");
      Serial.print((time() - startTime)/1000.0);
      Serial.println(" seconds.");
      Serial.println();
    }
    }
    
}


