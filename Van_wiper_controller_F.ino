const int relayPin = 2;
const int potPin = A4;
const int x = 9; //the approximate maximum delay time in seconds
const int currentSensorPin = A3;
const int voltageSensorPin = A1;
int potValue = 0;
int waitTime = 5;
int parked = 1;
int parkedCheck = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
int counter = 0;
int voltage;
unsigned long voltageAverage;
int parkedCheckNew = 0;
unsigned long parkedCheckAverage = 0;


void setup() {
Serial.begin(9600);
pinMode(relayPin, INPUT_PULLUP); //this stops the relay cycling when it boots
pinMode(relayPin, OUTPUT);
pinMode(potPin, INPUT);
}

void loop() {
  start:
  digitalWrite(relayPin, 1); //connect MRf to MRm as default. Wiper movement will start if position 1 is selected
  potValue = analogRead(potPin); //read the value at the potentiometer
  Serial.print ("potValue: ");
  Serial.println (potValue);
  if (potValue<110) goto start; //no need to turn the relay off and on again if the delay is very small: 110 works
  voltage = analogRead (voltageSensorPin);
  //Serial.print("voltage: ");
  //Serial.println (voltage);
  if (voltage < 300) goto start; //if the switch is in position 0 there is no need to do anything. Go back to the start
  
  for (counter=0; counter<300; counter++){ //this for loop makes an average of 300 readings on the voltage sensor to smooth it out
        voltage = analogRead (voltageSensorPin);   //read the value at the voltage sensor
        voltageAverage = voltageAverage + voltage; //add the new reading to the cumulative total
        }
      voltageAverage = voltageAverage / 300; //calculate the average of the readings
  if (voltageAverage > 550){ //if the switch is in position 2, go back to the start
    goto start;
  }
      
  //if the switch is not in position 0 or 2, then it must be in position 1, so continue with the intermittent program:

  Serial.println("delay start ");
  delay (300); //delay to allow the wipers to leave the parked position. The relay is not connecting MRf with TLf so there is no short circuit danger
  digitalWrite(relayPin,0); //connect MRf to TLf. This will make the wipers complete their stroke and finish in the park position. This creates a short circuit danger if the switch is in position 0
  parked = 0;
  while(parked == 0){ //check to see if the wipers are parked yet. If not, wait until they are before moving on
      for (counter=0; counter<1000; counter++){ //this for loop makes an average of 1000 readings on the current sensor to smooth it out
        parkedCheckNew = analogRead(currentSensorPin);   //read the value at the current sensor
        parkedCheckAverage = parkedCheckAverage + parkedCheckNew; //add the new reading to the cumulative total
        voltage = analogRead (voltageSensorPin);
        if (voltage < 300) goto start; //if the voltage is zero then it's important to immediately turn off the relay so go back to the start
        potValue = analogRead(potPin); //read the value at the potentiometer
        if (potValue<40) goto start; //if the pot has been turned to zero there is no need to do any delay so no need to check to see if the wipers are parked. This avoids the hesitation from park problem after the pot is turned to zero
        }
      parkedCheckAverage = parkedCheckAverage / 1000; //calculate the average of the readings of the current
      Serial.print ("parkedCheckAverage ");
      Serial.println (parkedCheckAverage);
      if (parkedCheckAverage > 90) {
        parked = 1; //if the value is above 90 then the wipers are parked
        Serial.println("parked detected");
        }
      }
  
  previousMillis = millis();  //this is where the magic happens. The length of the delay before a new wiper stroke is determined by the pot setting.
  currentMillis = millis();
  waitTime = 5; //because if waitTime is zero it will always skip the delay
  while (currentMillis - previousMillis < waitTime){
  potValue = analogRead(potPin); //read the value at the potentiometer
  waitTime = (potValue*x); //convert to delay in milliseconds
  currentMillis = millis();
  voltage = analogRead (voltageSensorPin);
  if (voltage < 300) goto start; //if the voltage is zero then it's important to immediately turn off the relay so go back to the start
  }
}
