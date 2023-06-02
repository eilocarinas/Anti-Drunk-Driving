#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h> // Include the Wire library for I2C
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library for I2C LCD

#define MQ3 A0

//limits
#define alcohol_limit 400 

//motor 1

#define in1 2
#define in2 3
#define enA 9

//motor 2

#define in3 4
#define in4 5
#define enB 10

//sensors
#define IR 27 // eye blink, ir
#define trigger1 41 //front
#define echo1 43
#define trigger2 47 //back
#define echo2 49

#define LED 39
#define buzzer 53

int near = 10;
int far = 40;
int eye_blink;
long duration, distance, FrontSensor, BackSensor, front_detect, back_detect;

//sms
int state = 0;
const int Trig = 9; // Trigger for the SMS
float gpslat, gpslon;

TinyGPS gps;
SoftwareSerial sgps(25, 23);
SoftwareSerial sgsm(7, 6);

// mq3
float MQ3_sensor;

//lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

int sleep = 0;
int count = 0;
void setup() {

  Serial.begin(9600);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //ultrasonic sensor
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);

  pinMode(trigger2, OUTPUT);
  pinMode(echo2, INPUT);


  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  //lcd
  lcd.begin(16, 2); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  lcd.setCursor(15, 0); 
  lcd.clear();  
  lcd.setCursor(0, 0); 
  lcd.print("Hello"); 
  delay(1000); // need to initialize alcohol sensor

}
void motor_on(){
  analogWrite(enA, 255);
  analogWrite(enB, 255);

/*motor direction
 * depends on the physical connection in the motor driver
 */
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  }

void motor_off(){
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  }


void stop_the_car(){
  for(int i=255; i > 0; --i){

    analogWrite(enA, i);
    analogWrite(enB, i);
    delay(20);
    }
  motor_off();
  }
void calc(int trigger,int echo)
{
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigger, LOW);
  duration = pulseIn(echo, HIGH);
  distance = 0.017 * duration;
}

void drunk(){
  stop_the_car();
  digitalWrite(buzzer, HIGH);
  }
void warning(){
  motor_on();
  digitalWrite(buzzer, HIGH);
  }  
void normal(){
  motor_on();
  digitalWrite(buzzer, LOW);
  }

void Alcohol(){
  MQ3_sensor = analogRead(MQ3); 
  Serial.println("Sensor Value: ");
  Serial.println(MQ3_sensor);
  delay(1000); 
  }
void eye(){
  eye_blink = digitalRead(IR);
  if (eye_blink == 0){
     
     count +=1;
     if (count == 3){
      sleep = 1;
      } 
  }
  else{
     count = 0;
     sleep =0;
    }
   
  }
void ultrasonic_sensor(){
calc(trigger1, echo1);
FrontSensor = distance;

  if( distance <= near){
      front_detect = 1;
   
    }
   else{
    front_detect= 0;
    
    }


calc(trigger2, echo2);
BackSensor = distance;

  if( distance <= near){
      back_detect = 1;
   
    }
   else{
    back_detect = 0;
    
    }
  }

  
void loop() {

ultrasonic_sensor();
eye();
Alcohol();

  if (MQ3_sensor >= alcohol_limit||sleep == 1){
    drunk();
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("motor will stop");
    }
  else if(front_detect ==1){
    motor_off(); //immediate stop
    digitalWrite(buzzer, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("object detected at the front");
    }
  else if(back_detect ==1){
    motor_off(); //immediate stop
    digitalWrite(buzzer, HIGH);
  
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("object detected at the back");
  } 
  else if (MQ3_sensor >= alcohol_limit - alcohol_limit/4){ 
    warning(); 
    }
  else{
    normal();
    }
  

}
