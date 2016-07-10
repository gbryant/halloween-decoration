
#include <SdFat.h>
SdFat sd;

#define SD_ChipSelectPin SS  //use digital pin 4 on arduino Uno, nano etc, or can use other pins
#include <TMRpcm.h>          //  also need to include this library...
#include <SPI.h>

TMRpcm audio;   // create an object for use in this sketch

class Motor
{
public:

int speed;
int pinSpeed;
int pinA;
int pinB;

void stop();
void brake();
void cw();
void ccw();

Motor(int pinSpeed, int pinA, int pinB);
};

Motor::Motor(int pinSpeed, int pinA, int pinB)
{
this->pinSpeed = pinSpeed;
this->pinA = pinA;
this->pinB = pinB;
speed = 255;
pinMode(this->pinSpeed,OUTPUT);
pinMode(this->pinA,OUTPUT);
pinMode(this->pinB,OUTPUT);
digitalWrite(this->pinSpeed,LOW);
digitalWrite(this->pinA,LOW);
digitalWrite(this->pinB,LOW);
}

void Motor::brake()
{
digitalWrite(pinSpeed,HIGH);
digitalWrite(pinA,HIGH);
digitalWrite(pinB,HIGH);
delay(250);
digitalWrite(pinSpeed,LOW);
digitalWrite(pinA,LOW);
digitalWrite(pinB,LOW);

}

void Motor::stop()
{
digitalWrite(pinSpeed,LOW); 
}

void Motor::cw()
{
analogWrite(pinSpeed,speed);
digitalWrite(pinA,LOW);
digitalWrite(pinB,HIGH);
}

void Motor::ccw()
{
analogWrite(pinSpeed,speed);
digitalWrite(pinA,HIGH);
digitalWrite(pinB,LOW);
}

ofstream logfile;

int pinMotorFeedback = A0;
int pinButton = 0;

int pinEye1 = A1;
int pinEye2 = A2;

//Motor jawMotor(5,6,7);
Motor neckMotor(3,2,4);

void setup()
{
audio.speakerPin = 9;

//Serial.begin(115200);

if (!sd.begin(SD_ChipSelectPin, SPI_FULL_SPEED))
{return;}else{ /*Serial.println("SD OK");*/}

randomSeed(analogRead(3));

pinMode(pinEye1, OUTPUT);
pinMode(pinEye2, OUTPUT);
pinMode(pinButton, INPUT);

digitalWrite(pinEye1,HIGH);
digitalWrite(pinEye2,HIGH);


}


void loop()
{

  
  int buttonState = digitalRead(pinButton);
  while(buttonState==HIGH){buttonState = digitalRead(pinButton);delay(10);}

neckMotor.cw();
bool stopMiddle=true;
bool stopRight=true;
bool stopLeft=false;
bool goingCW=true;
while(1)
{
  if(digitalRead(pinButton)==HIGH){neckMotor.brake();break;}
  
  int position = analogRead(pinMotorFeedback);
  if(position<60 && stopRight)
  {
    stopMiddle = true;
    stopRight = false;
    stopLeft = true;
    goingCW=false;
    neckMotor.brake();
    neckMotor.speed = random(60,256);
    randomDelay(1000,8000);
    neckMotor.ccw();
  }
  else if(position>557 && stopLeft)
  {
    stopMiddle = true;
    stopRight = true;
    stopLeft = false;
    goingCW=true;
    neckMotor.brake();
    neckMotor.speed = random(60,256);
    randomDelay(1000,8000);
    neckMotor.cw();
  }
  else if(abs(position-308)<=50 && stopMiddle)
  {
    stopMiddle = false;
    neckMotor.brake();
    playRandomSound(1);
    neckMotor.speed = random(60,256);
    randomDelay(1000,8000);
    if(goingCW){neckMotor.cw();}
    else{neckMotor.ccw();}
  }
}

}

void randomDelay(int min, int max)
{
  delay(random(min, max+1));
}

void playRandomSound(bool movement)
{
    bool goingCW=true;
    int delayAmount = random(50, 151);
    char fileName[64];
    int dirID = random(1, 2+1);
    int fileID;
    if(dirID==1)
    {
      fileID = random(1, 29+1);  
    }
    else if(dirID==2)
    {
      fileID = random(1, 31+1); 
    }
    sprintf( fileName, "/%d/%d.wav", dirID,fileID );
    audio.play(fileName);

    int cwCount=0;
    int ccwCount=0;
    while(audio.isPlaying())
    {
      if(movement)
      {
        if(goingCW)
        {
          goingCW=false;
          neckMotor.ccw();
          ccwCount++;
          delay(delayAmount);
          neckMotor.brake();
        }
        else
        {
          goingCW=true;
          neckMotor.cw();
          cwCount++;
          delay(delayAmount);
          neckMotor.brake();
        }
      }
      else
      {
        delay(50);
      }
    }
    if(movement)
    {
      if(cwCount>ccwCount)
      {
        for(int i=0;i<cwCount-ccwCount;i++)
        {
          neckMotor.ccw();
          delay(delayAmount);
          neckMotor.brake();
        }
      }
      else if(ccwCount>cwCount)
      {
        for(int i=0;i<ccwCount-cwCount;i++)
        {
          neckMotor.cw();
          delay(delayAmount);
          neckMotor.brake();
        }
      }
    }
}

  /*
   //********************Sound Test Code
  while(1)
  {
    char fileName[64];
    int dirID = random(1, 2+1);
    int fileID;
    if(dirID==1)
    {
      fileID = random(1, 29+1);  
    }
    else if(dirID==2)
    {
      fileID = random(1, 31+1); 
    }
    sprintf( fileName, "/%d/%d.wav", dirID,fileID );
    audio.play(fileName);
  
    while(audio.isPlaying()){delay(50);}
    
    if(digitalRead(pinButton)==HIGH){break;}
  }

  while(1){}
  */
/*
  //********************Range Of Motion Test Code
  bool leftBoundary=false;
  bool rightBoundary=false;
  logfile.open("log",O_CREAT|O_TRUNC|O_WRITE);
  logfile << "started" << endl;
  neckMotor.cw();
  
  while(1)
  {
  int position = analogRead(pinMotorFeedback);
  logfile << position << endl;
  //Serial.println(position);
  if(position<6)
  {
    logfile << "right boundary" <<endl;
    rightBoundary=true;
    neckMotor.brake();
    neckMotor.ccw();
    delay(500);
  }
  else if(position>600)
  {
    logfile << "left boundary" <<endl;
    leftBoundary=true;
    neckMotor.brake();
    neckMotor.cw();
    delay(500);
  }
  delay(100);
  if(leftBoundary && rightBoundary){neckMotor.brake();break;}
  }
  logfile << "finished" << endl;
  logfile.close();
  while(1){}

*/



