/*
YF‐ S201 Water Flow Sensor
Water Flow Sensor output processed to read in litres/hour
Adaptation Courtesy: www.hobbytronics.co.uk
*/
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 21; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
int sol=35;
unsigned long time_lapse;
int previousValue;
int currentValue;

void setup()
{
   pinMode(flowsensor, INPUT);   
   delay(5000);
   pinMode(sol,OUTPUT);
   digitalWrite(sol,HIGH);
   Serial.begin(9600);
   currentTime = millis();
   previousValue = digitalRead(flowsensor);
   flow_frequency = 0;
}
void loop ()
{
  currentValue = digitalRead(flowsensor);
  if(previousValue == 0 && currentValue == 1)
  {
    flow_frequency = flow_frequency + 1;
  }
  previousValue = currentValue;
  Serial.println(flow_frequency);
  time_lapse= millis() - currentTime;
  Serial.println(time_lapse);
  if(time_lapse>= 12500){
    digitalWrite(sol,LOW);}

}
