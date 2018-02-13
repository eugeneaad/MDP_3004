#include <PinChangeInt.h>
#include <DualVNH5019MotorShield.h>
#include <SharpIR.h>
#include <PID_v1.h>

#define PIDInputPinM1 A0
#define PIDInputPinM2 A1

DualVNH5019MotorShield md(4, 2, 6, A0, 7, 8, 12, A1);

SharpIR sensorFR(GP2Y0A21YK0F, A0);
SharpIR sensorFL(GP2Y0A21YK0F, A1);
SharpIR sensorL(GP2Y0A21YK0F, A3);
SharpIR sensorR(GP2Y0A21YK0F, A2);

volatile int mLTicks = 0;
volatile int mRTicks = 0;

char inData;

//Define Variables we'll be connecting to
double SetpointM1, InputM1, OutputM1;
double SetpointM2, InputM2, OutputM2;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID M1PID(&InputM1, &OutputM1, &SetpointM1, Kp, Ki, Kd, DIRECT);
PID M2PID(&InputM2, &OutputM2, &SetpointM2, Kp, Ki, Kd, DIRECT);


void setup() {
  // put your setup code here, to run once:
  pinMode(4, INPUT);  //Interrupt Pin 4
  pinMode(13, INPUT); //Interrupt Pin 13
  
  InputM1 = analogRead(PIDInputPinM1);
  InputM2 = analogRead(PIDInputPinM2);
  
  SetpointM1 = 130;
  SetpointM1 = 130;

  M1PID.SetMode(AUTOMATIC);
  M2PID.SetMode(AUTOMATIC);
  md.init();
  
  PCintPort::attachInterrupt(11, &compute_mL_ticks, RISING);  //Attached to Pin 11
  PCintPort::attachInterrupt(3, &compute_mR_ticks, RISING); //Attached to Pin 3
  Serial.begin(9600);
  Serial.println("Waiting for data: ");
}


void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
                   inData += (char)Serial.read();
                   delay(2);
            }
//            Serial.println(inData);
            switch(inData)
            {
              case 'F':
                moveForward();
                getSensorsData();
                break;
              case 'R':
                turnRight();
                getSensorsData();
                break;
              case 'L':
                turnLeft();
                getSensorsData();
                break;
              default:
                break;        
                
            }
            inData = '\0';
}

void compute_mL_ticks() 
{
  mLTicks++;
}

void compute_mR_ticks() 
{
  mRTicks++;
}

void moveForward(){
  
  double dTotalTicks = 0;
  
  dTotalTicks = 275 / 10.0 * 10;

  
  while(mLTicks < dTotalTicks)
  { 
    InputM1 = analogRead(PIDInputPinM1);
    InputM2 = analogRead(PIDInputPinM2);
    
    M1PID.Compute();
    M2PID.Compute();     
    
    md.setSpeeds(OutputM1,OutputM2);

    //For Debug
    Serial.print("OutputM1:");
    Serial.println(OutputM1);
    Serial.print("OutputM2");
    Serial.println(OutputM2);

    Serial.print("mTicks:");
    Serial.print(mLTicks);
    Serial.print("/");
    Serial.println(dTotalTicks);
  } 

  forwardBrake();
}

void forwardBrake(){
  
  for(int i = 0; i < 100; i++)
  {
    md.setBrakes(350,400);
  }
  
  delay(100);
  
  mLTicks = 0;
  mRTicks = 0;
}

void turnRight(){
  double dTotalTicks = 0;
  
  dTotalTicks = 383/ 10.0 * 10;


  while(mLTicks < dTotalTicks)
  {      
     md.setSpeeds(400,-400);
  }
 
  leftBrake();
}

void rightBrake(){
  
  for(int i = 0; i < 100; i++)
  {
    md.setBrakes(325,310);
  }
  
  delay(100);
  mLTicks = 0;
  mRTicks = 0;
}



void turnLeft(){
  double dTotalTicks = 0;
  
  dTotalTicks = 360 / 10.0 * 10;


  while(mLTicks < dTotalTicks)
  {      
     md.setSpeeds(-400,400);
  }

  rightBrake();
}

void leftBrake(){
  
  for(int i = 0; i < 100; i++)
  {
    md.setBrakes(370,-400);
  }
  
  delay(100);
  
  mLTicks = 0;
  mRTicks = 0;
}

void getSensorsData(){
  int disFL, disFR, disL, disR;
  double avgFL = 0, avgFR = 0, avgL = 0, avgR = 0;
  
  for(int i =0; i<7; i++)
  {
      disFL = sensorFL.getDistance(); //Calculate the distance in centimeters and store the value in a variable
      disFR = sensorFR.getDistance();
      disL = sensorL.getDistance();
      disR = sensorR.getDistance();
      
      avgFL += disFL;
      avgFR += disFR;
      avgL += disL;
      avgR += disR; 
  }
  avgFL = avgFL / 7;
  avgFR = avgFR / 7;
  
  avgL = avgL / 7;
  avgR = avgR / 7;
  
  Serial.print("FL:"); //Print the value to the serial monitor
  Serial.println(avgFL);
  Serial.print("FR:");
  Serial.println( avgFR);
  Serial.print("L:");
  Serial.println(avgL);
  Serial.print("R:");
  Serial.println(avgR);
}
