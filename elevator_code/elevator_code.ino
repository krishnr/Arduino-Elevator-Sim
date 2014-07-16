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
  //TCCR0B = (_BV(CS00));
  
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
        delay(500);
        digitalWrite(i+10, HIGH);
        Serial.print("On floor ");
        Serial.print(i);
        Serial.println();
        delay(500);
      }
    }  
    
    //if going down
    if(destinationFloor-1 < currentFloor) {
      
      for(int i = currentFloor; i > destinationFloor; i--) {
        //wait for travel time
        digitalWrite(i+11, LOW);
        delay(500);
        digitalWrite(i+10, HIGH);
        Serial.print("On floor ");
        Serial.println(i);
        delay(500);
      }
    } 
    
    digitalWrite(destinationFloor+9, LOW);
    digitalWrite(destinationFloor+11, LOW);
    delay(500);
    Serial.print("Arrived at ");
    Serial.println(destinationFloor);
    
    //open and close door
    
    currentFloor = destinationFloor;
  }
}
