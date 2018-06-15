#include <ros.h>
#include <std_msgs/UInt8MultiArray.h>

#define M1_PWM 9 
#define M2_PWM 10
#define M1_DIR 11
#define M2_DIR 12

void Clockwise(int pin);
void AntiClockwise(int pin);
void Speed(int pwm,int pin);
void Forward(int pwm);

ros::NodeHandle  nh;

void messageCb( const std_msgs::UInt8MultiArray& msg){
  //digitalWrite(13, HIGH-digitalRead(13));   // blink the led
  
  // msg[0]: Direction of motor #1
  // msg[1]: Speed of motor #1
  // msg[2]: Direction of motor #2
  // msg[3]: Speed of motor #2
  
  if(msg.data[0] == 1) 
    {Clockwise(M1_DIR);
     
     //digitalWrite(LED_BUILTIN,HIGH);
    }
  else if ( msg.data[0] == 0)
    {AntiClockwise(M1_DIR);
     //digitalWrite(LED_BUILTIN,LOW);
    }
  if(msg.data[2] == 1)
    {
      Clockwise(M2_DIR);
    }
  else if (msg.data[2] == 0)
     {
      AntiClockwise(M2_DIR);
     }
  
  analogWrite(M1_PWM,msg.data[1]);
  analogWrite(M2_PWM,msg.data[3]);
}


ros::Subscriber<std_msgs::UInt8MultiArray> sub("MCU_Input", &messageCb );

void setup() {
  // put your setup code here, to run once:

  //Motor Routine
  pinMode(M1_DIR,OUTPUT);
  pinMode(M2_DIR,OUTPUT);
  pinMode(M1_PWM,OUTPUT);
  pinMode(M2_PWM,OUTPUT);
  pinMode(13,OUTPUT);
  //setPwmFrequency(M1_PWM, 8); //9,8 ...3.9Khz
  //setPwmFrequency(M2_PWM, 8);
  TCCR2B = (TCCR2B & 0xF8) | 0x02; //3.9Khz 9&10
  //analogWrite(M1_PWM,50);
  //analogWrite(M2_PWM,50);

  nh.initNode();
  nh.subscribe(sub);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void Clockwise(int pin)
{
  digitalWrite(pin,HIGH);
}

void AntiClockwise(int pin)
{
  digitalWrite(pin,LOW);
}

void Speed(int pwm,int pin)
{
  analogWrite(pin, pwm);
}

void Forward(int pwm)
{
  Clockwise(M1_DIR);
  Speed(pwm,M1_DIR);
  Clockwise(M2_DIR);
  Speed(pwm,M2_DIR);
}
