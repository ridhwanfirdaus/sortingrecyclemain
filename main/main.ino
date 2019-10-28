#include <HX711.h>
#include <Servo.h>


//declare object
HX711 scale(8,6);// Dout first, clk second (5, 6)
Servo servo1 , servo2 , servo3 ,servo4; // declare all servos 3 5 6 9 10 11

//pins
const int laserTransmit = 2;
const int laserReceiver = 3;
const int metalReceiver = 5;
const int ultraTrig = 4 ;
const int ultraEcho = 7 ;
const int DOUT = 8 ;
const int CLK = 6 ;
const int plasticServo = 9;
const int metalServo = 10;
const int paperServo = 11;
const int mainServo = 12;


//constant
float calibration_factor = -445;
long duration ;
int distance ;
int valueLaser ;
int valueMetal ;
bool valueWeight ;
bool valueUltra ;
int setdistanceincm; //PUT SET DISTANCE
int setweightingrams = 100; //PUT SET WEIGHT
float units;
float ounces;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 pinMode(metalReceiver,INPUT_PULLUP);
 pinMode(laserTransmit,OUTPUT);
 pinMode(laserReceiver,INPUT);
 scale.set_scale(calibration_factor);
 scale.tare();
 pinMode(ultraTrig,OUTPUT);
 pinMode(ultraEcho,INPUT);
// scale.begin(DOUT,CLK);
 servo1.attach(plasticServo);
 servo2.attach(metalServo);
 servo3.attach(paperServo);
 servo4.attach(mainServo);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  warningSwitch();
  bool valueUltra = ultraRead(); // detect trash with ultrasonic first
  while(valueUltra == true){  // trash detected maybe change it to 1
     warningSwitch();
    int valueMetal = metalRead(); //detect metal or not
    if(valueMetal == 0){          // if metal detected ,active low 
      moveServo(metalServo,30);   // move metal servo 30 degrees
      valueUltra = false;         //reset values
      valueMetal = 1;
      delay(2000);               //delay to make sure the divider all settled in
      moveServo(mainServo,180);  // move main servo to 180 degree to drop trash
      delay(5000);               // so that the trash can drop
      moveServo(mainServo,90);   //move back main servo to original place
      moveServo(metalServo,0);   // move back metal divider to original place
      break; // break of the while loop after finding metal
    }
    warningSwitch();
    bool valueWeight = weightRead();
    if(valueWeight == true){
      moveServo(mainServo,180);
      valueUltra = false;
      valueWeight = false;
      delay(5000);
      moveServo(mainServo,90); //move only the main servo servo can only move 90 degree to each side
      break;
    }
    warningSwitch();
    int valueLaser = laserRead();
    if(valueLaser == 1){
      moveServo(plasticServo,30);
      valueUltra = false;
      valueLaser = 0;
      delay(2000);
      moveServo(mainServo,180);
      delay(5000);
      moveServo(mainServo,90);
      delay(1000);
      moveServo(plasticServo,0);
      break;
    }else{
      moveServo(paperServo,30);
      valueUltra = false;
      valueLaser = 0;
      delay(2000);
      moveServo(mainServo,180);
      delay(5000);
      moveServo(mainServo,90);
      delay(1000);
      moveServo(paperServo,0);
      break;
    }
  }
}

bool weightRead(){
delay(1000);// probably need delay
Serial.println("Initiating weight sensor");
bool switchcondition = false;
units = scale.get_units(10),10;
if (units < 0)
{
  units = 0.00;
}
 ounces = units * 0.035274;
 Serial.print(units);
 Serial.println(" grams");
 if(units > setweightingrams){
  switchcondition = true;
 }
 return(switchcondition);
}

int laserRead(){
 Serial.println("Initiating laser sensor");
 digitalWrite(laserTransmit, HIGH);
 delay(2000);//delay 2s
 int value = digitalRead(laserReceiver);
 digitalWrite(laserTransmit, LOW);
 Serial.print("Laser ");
 Serial.println(value);
 return(value);
}

int metalRead(){
 Serial.println("Initiating metal sensor");
 int value = digitalRead(metalReceiver);
 Serial.print("Metal ");
 Serial.println(value);
 return(value);// must be zero to activate
}

bool ultraRead(){
 bool switchcondition = false;
 Serial.println("Initiating ultrasonic sensor");
 digitalWrite(ultraTrig, LOW);
 delayMicroseconds(2);
 // Sets the trigPin on HIGH state for 10 micro seconds
 digitalWrite(ultraTrig, HIGH);
 delayMicroseconds(10);
 digitalWrite(ultraTrig, LOW);
 // Reads the echoPin, returns the sound wave travel time in microseconds
 duration = pulseIn(ultraEcho, HIGH);
 // Calculating the distance
 distance= duration*0.034/2;
 // Prints the distance on the Serial Monitor
 Serial.print("Distance: ");
 Serial.println(distance);
 if(distance < setdistanceincm){
  switchcondition = true;
  Serial.println("Process starts ");
 } 
 return(switchcondition);  
}

void moveServo(int servo , int angle){
 switch (servo){
  case 9:
      servo1.write(angle);
      break;
  case 10:
      servo2.write(angle);
      break;
 case 11:
      servo3.write(angle);
      break;
 case 12:
      servo4.write(angle);
      break;
 default:
      break;    
 }
}

 void warningSwitch(){
  Serial.print("initiating DETECTOR TEST...\n");
  Serial.println("5\n");
  delay(1000);
  Serial.println("4\n");
  delay(1000);
  Serial.println("3\n");
  delay(1000);
  Serial.println("2\n");
  delay(1000);
  Serial.println("1\n");
  delay(1000);
 }
