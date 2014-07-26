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
const int difference = 10;
volatile int pauseTime = 1000;
volatile int doorTime = 800;
volatile boolean isStopped = false;
boolean isStopPrinted = false;
const int doorSize = 5;
const int doorLight[] = {A0, A1, A2, A3, A4, A5};
volatile boolean isDoorClosePressed = false;
volatile boolean isDoorOpenPressed = false;
volatile long doorOpenTime = 0;
volatile long doorCloseTime = 0;
volatile boolean isFirstTimeDoorOpened = true;
volatile long startTime = 0;
volatile boolean hasMoved = false;
volatile boolean isDoorTimePrinted = false;

void setup() {
  //Sets up all the necessary pins and their modes
  
  //These are the buttons for the various floors. Default value of high (input pullup)
  pinMode(floor0Pin, INPUT_PULLUP);
  pinMode(floor1Pin, INPUT_PULLUP);
  pinMode(floor2Pin, INPUT_PULLUP);
  pinMode(floor3Pin, INPUT_PULLUP);
  
  //The lights corresponding to the floors, ranging from 0-3
  pinMode(floor0LED, OUTPUT);
  pinMode(floor1LED, OUTPUT);
  pinMode(floor2LED, OUTPUT);
  pinMode(floor3LED, OUTPUT);
  
  //The speaker
  pinMode(speakerPin, OUTPUT);
  
  //These are the lights for the door
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  
  //At the beginning of the program, the doors are closed i.e. all the leds corresponding to the door is on
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);


  //Attach interrupts
  //Interrupt for the doorClose button. Corresponds to Pin 2
  attachInterrupt(0, doorCloseInterrupt, CHANGE);
  //Interrupt for the doorOpen button. Corresponds to Pin 3
  attachInterrupt(1, doorOpenInterrupt, CHANGE);
  
  //The pin coresponding to the express interrupt is at a default state of high (input pull up)
  digitalWrite(expressPin, HIGH);
  
  PCICR |= _BV(PCIE0); //Adds the bit value of Pin Change Interrupt Enable 0 to the Pin change interrupt control register i.e.(pins D8-D13)
  PCMSK0 = _BV(PCINT0); //Sets the pin change mask to the bit value of Pin change interrupt 8
  
  //The above ensures that the set of D8-D13 act as pin change interrupts, but the masking ensures that only
  //the pin interrupt defined (i.e. pin 8) acts as interrupt
  
  digitalWrite(emergencyStopPin, HIGH);
  PCICR |= _BV(PCIE2); //Also adds the bit value of Pin Change interrupt enable 2 to the pin change interrupt control register
  //This means that the set of pins D0-D7 will act as pin change interrupts
  PCMSK2 = _BV(PCINT16); //Masks the value so that pin 0 is the only one that will act as pin change interrupts
  

  TIMSK1=0x01; //Timer/Counter1 Overflow interrupt is enabled
  TCCR1A = 0x00; // Timer Counter Control Register A  -- normal operation mode
  TCNT1=0x0BDC; // set initial value of 3036 to remove time error (16bit counter register)
  TCCR1B = 0x04; // setting the clock select bits to 4 -> prescaler (value of 256)
  
  Serial.begin(9600); 
  
  //Lights up the first LED (floor 0)
  digitalWrite(currentFloor+difference, HIGH);
  
  
}

void loop() {
  
  //As long as the program isn't stopped (I.E. emergency button has not been pressed)
  if(!isStopped) {
  
  //Light up currentFloor
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
  
  //If a different floor than the current one is pressed
  //i.e. elevator is called or directed to move
  if(destinationFloor != currentFloor) {
    
    startTime = time();
    hasMoved=true;
    isDoorTimePrinted = false;
    
    //if going up
    if(destinationFloor-1 > currentFloor) {
      
      for(int i = currentFloor; i < destinationFloor && !isStopped; i++) {
        
        digitalWrite(i+difference-1, LOW);
        //travel time
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
        
        if(i != 3) {
          digitalWrite(i+difference+1, LOW);
        }
        //travel time
        pause(pauseTime);
        digitalWrite(i+difference, HIGH);
        Serial.print("On floor ");
        Serial.println(i);
        pause(pauseTime);
      }
    } 
    
   if(!isStopped){
     
     //set all leds to low
      digitalWrite(floor0LED, LOW);
      digitalWrite(floor1LED, LOW);
      digitalWrite(floor2LED, LOW);
      digitalWrite(floor3LED, LOW);
      
      pause(pauseTime);
      //light up the led of the destination floor
      digitalWrite(destinationFloor + difference, HIGH);
      Serial.print("Arrived at floor ");
      Serial.println(destinationFloor);
      
      //play ding ding tone
      tone(speakerPin, 1000, 100);
      pause(200);
      tone(speakerPin, 1000, 100);
      
      //open door
      openDoor();
     
      hasMoved = false;
   }
  }
    
    
    currentFloor = destinationFloor;

    } else if(!isStopPrinted) {

      Serial.println("EMERGENCY STOP!");
      openDoor();
      Serial.println();
      
      isStopPrinted = true;
    } else {
      
      //this loops and plays alarm sound
      tone(9, 2000, 100);
      pause(200);
    }
    
}

//Interrupt Service Routine called whenever the Timer 1's overflow flag is 1
ISR(TIMER1_OVF_vect)
{
    //sets initial value of timer to 3036  
    TCNT1=0x0BDC; 
    overflowCount++;
}
    
unsigned long time()
{
    unsigned long n;
    
    //saves the state of the old status register to store the interrupt status
    uint8_t oldSREG = SREG;
 
    // disable interrupts while we read timer1
    cli();
    
    //read timer 1 and add to overflow count
    n = TCNT1 + (overflowCount * 62500);
    
    //divide by 62500 to give # of seconds. Multiply by 1000 to give millis
    n = n/62500.0*1000.0;
    
    //restores state of old status register
    SREG = oldSREG; 
    
    //enable interrupts again
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

//ISR for expressButton correspoinding to pin 8
ISR (PCINT0_vect) {
  pauseTime *= 0.85;
  doorTime *= 0.85;
}

//ISR for emergency stop corresponding to pin 0
ISR (PCINT2_vect) {
  isStopped = true;
}

void openDoor() {
   
  isDoorClosePressed = false;
  isDoorOpenPressed = false;

   //start timer only first time door is opened
   if(isFirstTimeDoorOpened){
     doorOpenTime = time();
     isFirstTimeDoorOpened=false;
   }
  
  for(int i = doorSize/2; i >= 0; i--) {
     

     digitalWrite(doorLight[i], LOW);
     digitalWrite(doorLight[doorSize-i], LOW);
     
    //if door close is pressed, break immediately
    if(isDoorClosePressed) {
      isDoorClosePressed = false;
      isDoorOpenPressed = false;
      break;
     }
   pause(doorTime);  
 }
 
 //always calls closeDoor after it is done
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
    
    isFirstTimeDoorOpened = true;
    doorCloseTime = time();
    
    //if door time is NOT printed yet
    if(!isDoorTimePrinted) {
      Serial.print("The door was open for a total of ");
      Serial.print((doorCloseTime - doorOpenTime)/1000.0);
      Serial.println(" seconds.");
      isDoorTimePrinted = true;
    //only print trip time if elevator has moved
    if(hasMoved) {
      Serial.print("Your total trip time was ");
      Serial.print((time() - startTime)/1000.0);
      Serial.println(" seconds.");
      Serial.println();
    }
    }
    
}


