#include <TimerThree.h>
#include <TimerOne.h>
#include <ros.h>
#include <std_msgs/String.h>
#include "quaternionFilters.h"
#include "MPU9250.h"
#include <std_msgs/Int8MultiArray.h>
#include <std_msgs/UInt8MultiArray.h>

#define M1_PWM 9 
#define M2_PWM 10
#define M1_DIR 11
#define M2_DIR 12
#define AHRS true         // Set to false for basic data read
#define SerialDebug true  // Set to true to get Serial output for debugging

// Pin definitions
int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins
int myLed  = 13;  // Set up pin 13 led for toggling
int dummy=0;
MPU9250 myIMU;

void Clockwise(int pin);
void AntiClockwise(int pin);
void Speed(int pwm,int pin);
void Forward(int pwm);
void rosSpin();
ros::NodeHandle  nh;

int test=-1; 

void messageCb( const std_msgs::UInt8MultiArray& msg){
  //digitalWrite(13, HIGH-digitalRead(13));   // blink the led
  
  // msg[0]: Direction of motor #1
  // msg[1]: Speed of motor #1
  // msg[2]: Direction of motor #2
  // msg[3]: Speed of motor #2
  test=msg.data[0];
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

//UnInt8Multi Array
std_msgs:: Int8MultiArray msg;
ros::Publisher chatter("MCU_Output", &msg);
ros::Subscriber<std_msgs::UInt8MultiArray> sub("MCU_Input", &messageCb );

void setup()
{
  //Timer3.initialize(1000000);
  //Timer3.attachInterrupt(rosSpin);
  //SET PWM Frequency For Motor Driver
  
  
  //IMU Routine
  Wire.begin();
  Serial.begin(57600);
  //pinMode(intPin, INPUT);
  //digitalWrite(intPin, LOW);
  //pinMode(myLed, OUTPUT);
  //digitalWrite(myLed, HIGH);
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
  myIMU.initMPU9250();
  myIMU.initAK8963(myIMU.magCalibration);

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
  
  //Publisher Setup
  msg.data = (int8_t *)malloc(sizeof(int8_t)*4);
  msg.data_length=4;

  
  nh.initNode();
  nh.advertise(chatter);
  nh.subscribe(sub);
}

void loop()
{

 int i = millis();
 chatter.publish( &msg );
 nh.spinOnce();
 //delay(1);
 int f = millis();
 Serial.print(f-i);
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

void rosSpin()
{

 //IMU 
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {  
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.getAres();

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes; // - accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes; // - accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes; // - accelBias[2];

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    myIMU.getGres();

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    myIMU.gx = (float)myIMU.gyroCount[0]*myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1]*myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2]*myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    myIMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    myIMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    myIMU.magbias[2] = +125.;

    myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
               myIMU.magbias[0];
    myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
               myIMU.magbias[1];
    myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
               myIMU.magbias[2];
  } 
  myIMU.updateTime();

  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx*DEG_TO_RAD,
                         myIMU.gy*DEG_TO_RAD, myIMU.gz*DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

  if (!AHRS)
  { 
  } 
  else
  {
    // Serial print and/or display at 0.5 s rate independent of data rates
    //myIMU.delt_t = millis() - myIMU.count;
       myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ() *
                    *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1) * *(getQ()+1)
                    - *(getQ()+2) * *(getQ()+2) - *(getQ()+3) * *(getQ()+3));
      myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ() *
                    *(getQ()+2)));
      myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2) *
                    *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1) * *(getQ()+1)
                    - *(getQ()+2) * *(getQ()+2) + *(getQ()+3) * *(getQ()+3));
      myIMU.pitch *= RAD_TO_DEG;
      myIMU.yaw   *= RAD_TO_DEG;
      // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
      //   8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
      // - http://www.ngdc.noaa.gov/geomag-web/#declination
      myIMU.yaw   -= 8.5;
      myIMU.roll  *= RAD_TO_DEG;
      dummy=(int)myIMU.yaw;
      //mmyIMU.count = millis();
      myIMU.sumCount = 0;
      myIMU.sum = 0;
       
  }
 msg.data[0]=test;
 msg.data[1]=1;
 msg.data[2]=dummy;
 msg.data[3]=0;
 //chatter.publish( &msg );
 
 //delay(1);
 digitalWrite(13, digitalRead(13) ^ 1);
}

