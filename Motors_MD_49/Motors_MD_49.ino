
/*********************************************
 *      Arduino code for drive control        *
 *             NASA Lunabot, UVM              *
 *********************************************/

#define CMD        (byte)0x00            // MD49 command address of 0                                 
#define GET_VER          0x29
#define GET_ENC1         0x23
#define GET_ENC2         0x24
#define GET_VI           0x2C
#define GET_ERROR        0x2D
#define SET_ACCEL        0x33
#define SET_SPEED1       0x31
#define SET_SPEED2       0x32
#define GET_ENCODERS     0x25
#define GET_VOLTS        0x26
#define GET_CURRENT1     0x27
#define GET_CURRENT2     0x28
#define GET_ACCEL        0x2A
#define GET_MODE         0x2B
#define SET_MODE         0x34
#define DISABLE_TIMEOUT  0x38


#define MOTOR1_TRIP 0x04
#define MOTOR2_TRIP 0x08
#define MOTOR1_SHORT 0x10
#define MOTOR2_SHORT 0x20
#define OVERVOLTAGE 0x40
#define UNDERVOLTAGE 0x80
  

#define MIN_SPEED 1
#define MAX_SPEED 127
const float WHEEL_BASE = 2.0;
const int SPIN_CONSTANT = 5;
int min_speed = 10;
int max_speed = 50;

byte enc1a, enc1b, enc1c, enc1d = 0;
byte enc2a, enc2b, enc2c, enc2d = 0;
byte bat_volt1, mot1_cur1, mot2_cur1 = 0;

boolean MD49_enable(boolean HighLow){
  digitalWrite(MOTOR_RELAY,HighLow);
  return HighLow;
}
boolean MD49_setup()
{
  //Serial.begin(38400);  //9600 if jumper installed
  MD49_enable(HIGH);
  delay(100);
  MD49_set(SET_MODE,1);
  MD49_set(SET_ACCEL,3);
  MD49_set(DISABLE_TIMEOUT);
  return true;
}
boolean MD49_set(byte cmnd){
  Serial2.write(CMD);    //Disable timeout
  Serial2.write(cmnd);
  Serial3.write(CMD);
  Serial3.write(cmnd);
  return true;
}
boolean MD49_set(byte cmnd, byte val){
  Serial2.write(CMD);    //Disable timeout
  Serial2.write(cmnd);
  Serial2.write(val);
  Serial3.write(CMD);
  Serial3.write(cmnd);
  Serial3.write(val);
  return true;
}

boolean MD49_set(String sequence[]){
  int value = sequence[4].toInt();
  if(value > 0){
    if(sequence[3] == "MAX"){
      max_speed = constrain(value,MIN_SPEED,MAX_SPEED);
      //debug("set MAX: ");
      //debugln(max_speed);
    }
    else if(sequence[3] == "MIN"){
      min_speed = constrain(value,MIN_SPEED,MAX_SPEED);
      //debug("set MINX: ");
      //debug(min_speed);
    }
    return true;
  }
  return false;
} 

boolean MD49_reset(){
  MD49_enable(LOW);
  delay(100);
  return MD49_setup();
}

void MD49_controlLR(int L, int R)    //For manual control of left and right sides
{
  MD49_setSpeeds(L,L,R,R);
}
int speedCap(int value){
  if(value < 0){
    return constrain(value,-max_speed,-min_speed);//
  }
  else if(value > 0){
    return constrain(value,min_speed,max_speed);//
  }
  else
    return 0;
}
boolean MD49_setSpeeds(int M1, int M2, int M3, int M4)    //Set all individual motor speeds
{

  M1 = speedCap(M1);
  M2 = speedCap(M2);
  M3 = speedCap(M3);
  M4 = speedCap(M4);
  Serial2.write(CMD);
  Serial2.write(SET_SPEED1);
  Serial2.write(-M1);                        
  Serial2.write(CMD);
  Serial2.write(SET_SPEED2);
  Serial2.write(-M2);

  Serial3.write(CMD);
  Serial3.write(SET_SPEED1);
  Serial3.write(-M3);                        
  Serial3.write(CMD);
  Serial3.write(SET_SPEED2);
  Serial3.write(-M4);

  return true;//TODO: return a value
}

void MD49_getError(){

byte error1 = 0;
byte error2 = 0;
  MD49_set(GET_ERROR); // Receive error byte back
  delay(50);
  if(Serial2.available() > 0){
    error1 = Serial2.read();
  }
  if(Serial3.available() > 0){
    error2 = Serial2.read();
  }
  switch (error1) {
  case MOTOR1_TRIP:
    debug("1-Motor_1 TRIPPED.");
    break;
  case MOTOR2_TRIP:
    debug("1-Motor_2 TRIPPED.");
    break;
  case MOTOR1_SHORT:
    debug("1-Motor_1 SHORTED.");
    break;
  case MOTOR2_SHORT:
    debug("1-Motor_2 SHORTED.");
    break;
  case OVERVOLTAGE:
    debug("1-OVERVOLTAGE.");
    break;
  case UNDERVOLTAGE:
    debug("1-UNDERVOLTAGE.");
    break;
  default: 
    debug("NO ERRORS");
    //no errors
  }
  switch (error2) {
  case MOTOR1_TRIP:
    debug("2-Motor_1 TRIPPED.");
    break;
  case MOTOR2_TRIP:
    debug("2-Motor_2 TRIPPED.");
    break;
  case MOTOR1_SHORT:
    debug("2-Motor_1 SHORTED.");
    break;
  case MOTOR2_SHORT:
    debug("2-Motor_2 SHORTED.");
    break;
  case OVERVOLTAGE:
    debug("2-OVERVOLTAGE.");
    break;
  case UNDERVOLTAGE:
    debug("2-UNDERVOLTAGE.");
    break;
  default: 
    debug("NO ERRORS");
    //no errors
  }
}

void MD49_getBatt(){
  Serial2.write(CMD);
  Serial2.write(GET_VI);                     // Receive battery volts and both motor currents back
  delay(50);
  if(Serial2.available() > 2)
  {
    bat_volt1 = Serial2.read();
    mot1_cur1 = Serial2.read();
    mot2_cur1 = Serial2.read();
  }

  Serial.print("Battery Voltage 1: ");
  Serial.println(bat_volt1);
  Serial.print("Motor 1 Current: ");
  Serial.println(mot1_cur1);
  Serial.print("Motor 2 Current: ");
  Serial.println(mot2_cur1);
} 

void MD49_resetEncoders(){
  Serial2.write(CMD);
  Serial2.write(0x35);
}

void MD49_getEncoders(){
  Serial2.write(CMD);
  Serial2.write(GET_ENCODERS);                   // Receive encoder 1 value
  delay(50);
  if(Serial2.available() > 7)
  {
    enc1a = Serial2.read();
    enc1b = Serial2.read();
    enc1c = Serial2.read();
    enc1d = Serial2.read();

    enc2a = Serial2.read();
    enc2b = Serial2.read();
    enc2c = Serial2.read();
    enc2d = Serial2.read();
  }

  long result1 = 0;
  result1 = enc1a << 24; 
  result1 += enc1b << 16; 
  result1 += enc1c << 8; 
  result1 += enc1d;

  long result2 = 0;
  result2 = enc2a << 24; 
  result2 += enc2b << 16; 
  result2 += enc2c << 8; 
  result2 += enc2d;

  Serial.print("Encoder 1: ");
  Serial.println(result1);
  Serial.print("Encoder 2: ");
  Serial.println(result2);

}
boolean MD49_stop(){
  Serial2.write(CMD);
  Serial2.write(SET_SPEED1);
  Serial2.write(0);                        
  Serial2.write(CMD);
  Serial2.write(SET_SPEED2);
  Serial2.write(0);

  Serial3.write(CMD);
  Serial3.write(SET_SPEED1);
  Serial3.write(0);                        
  Serial3.write(CMD);
  Serial3.write(SET_SPEED2);
  Serial3.write(0);
}
boolean MD49_spin(String inputString[]){

  int Angle = inputString[2].toInt();
  int Speed = inputString[3].toInt();
  int Duration = inputString[4].toInt();

  float Vl = 0;
  float Vr = 0;

  if(Angle > 0){//spin right
    Vl = Speed;
    Vr = -Speed;
  }
  if(Angle < 0){//spin left
    Vl = -Speed;
    Vr = Speed;
  }//
  return MD49_setSpeeds(Vl,Vr,Vl,Vr);
}
boolean MD49_drive(int Radius, int Speed, int Duration){
  debug("     =Radius : ");
  debug(Radius);
  debug("     =Speed : ");
  debug(Speed);
  debug("     =Duration : ");
  debug(Duration);

  float Vl = 0;
  float Vr = 0;
  float val = WHEEL_BASE/(2.0 * Radius);
  if(Radius == 0){
    Vl = Speed;
    Vr = Speed;
  }
  else{
    if(Speed == 0){
      Vr = Radius/SPIN_CONSTANT;
      Vl = -Radius/SPIN_CONSTANT;
    }
    else{
      Vr = Speed * ( 1.0 + val );
      Vl = Speed * ( 1.0 - val );
    }
  }

  //  Serial.print("     =VALl : ");
  //  Serial.println(val);
  //  Serial.print("     =Vl : ");
  //  Serial.println(Vl);
  //  Serial.print("     =Vr : ");
  //  Serial.println(Vr);
  ///right2,4 +
  return MD49_setSpeeds(Vl,Vr,Vl,Vr);
}
boolean MD49_drive(String inputString[]){
  if(inputString[2] == "S"){
    return MD49_stop();
  }
  int Radius = inputString[2].toInt();
  int Speed = inputString[3].toInt();
  int Duration = inputString[4].toInt();
  return MD49_drive(Radius,Speed,Duration);
  /*
  //Speed = map(Speed,0,100,0,127);
   int M1 = int(   Speed * cos(Angle) );
   
   if (inputString[2] == "F")//motor command type
   {
   debug(" =Forward");
   //Serial1.print("Driving Forward");
   //          M1 = Speed, M2 = Speed, M3 = Speed, M4 = Speed; 
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "B")//motor command type
   {
   debug(" =BACK");
   //          M1 = -Speed, M2 = -Speed, M3 = -Speed, M4 = -Speed;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "R")//motor command type
   {
   debug(" =RIGHT");
   //          M1 = Speed, M2 = -Speed, M3 = Speed, M4 = -Speed;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "L")//motor command type
   {
   debug(" =LEFT");
   //          M1 = -Speed, M2 = Speed, M3 = -Speed, M4 = Speed;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "BR")//motor command type
   {
   debug(" =BACKRIGHT");
   //          M1 = -Speed, M2 = -Speed/2, M3 = -Speed, M4 = -Speed/2;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "BL")//motor command type
   {
   debug(" =BACKLEFT");
   //          M1 = -Speed/2, M2 = -Speed, M3 = -Speed/2, M4 = -Speed;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "FR")//motor command type
   {
   debug(" =FRONTRIGHT");
   //          M1 = Speed, M2 = Speed/2, M3 = Speed, M4 = Speed/2;  
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "FL")//motor command type
   {
   debug(" =FRONTLEFT");
   //          M1 = Speed/2, M2 = Speed, M3 = Speed/2, M4 = Speed;  //  MD49_setSpeeds(Speed/2,-Speed/2,Speed,-Speed);
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   else if (inputString[2] == "S")//motor command type
   {
   debug(" =STOP");
   //          M1 = 0, M2 = 0, M3 = 0, M4 = 0;  // 
   //          MD49_setSpeeds(M1,M2,M3,M4);
   }
   */
  //        
}







