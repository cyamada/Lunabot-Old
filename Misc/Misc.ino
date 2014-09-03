
#define VOLTAGE_SENSE A15
#define CURRENT_SENSE A14

float str2float(String str){
  char carray[str.length() + 1]; //determine size of the array
  str.toCharArray(carray, sizeof(carray)); //put readStringinto an array
  return atof(carray);
}

int parse(String string, const char* delim, String output[]){
  int count = 0;
  char ort[string.length()+1];
  char *p;
  string.toCharArray(ort,string.length()+1);
  p = ort;
  char *str;
  while ((str = strtok_r(p, delim, &p)) != NULL){ // delimiter is the semicolon
    output[count]=str;
    count++;
  }

  return count;//
}

void debugList(String list[],int start, int num,String chars){
  debugln("Debug List:");
  for(int i = start;i < num; i++){
    if(i == command_pointer)
      debug("  $");
    if(i == execute_pointer)
      debug("  *");
    if(i != execute_pointer && i != command_pointer)
      debug(chars);
    debugln(list[i]);
  }
}

long timeNow = 0;
long timeLast = 0;

long updateTime(){
  timeLast = timeNow;
  timeNow = millis();
}
long timeDelta(){
  return timeNow - timeLast;
}
/*
long poorManTimer(long milli){
  return millis() + milli;
}
*/


int totalCurrent = 0;
int totalPower = 0;

void setup_sensors(){
  pinMode(VOLTAGE_SENSE, INPUT);
  pinMode(CURRENT_SENSE, INPUT);
}

const float to_mAh = 0.02778;
int getVoltage(){
  int VRaw = analogRead(VOLTAGE_SENSE);
  return VRaw/12.99; //90 Amp board
}int getCurrent(){
  int IRaw = analogRead(CURRENT_SENSE);
  return IRaw/7.4; //90 Amp board
}
int calc(int current,int voltage){
  long deltaT = timeDelta();
  totalCurrent += current * deltaT; //mAs
  totalPower += current * voltage * deltaT; //mW
  
  debug("Total Current: ");
  debug(totalCurrent * to_mAh);
  debug("mAh. Total Power: ");
  debug(totalPower);
  debugln(" mW.");
  return totalPower;
}
int getMeasures(){
    //Measurement
  
  //Conversion
  //VFinal = VRaw/49.44; //45 Amp board
  int VFinal = getVoltage();
  //VFinal = VRaw/12.99; //180 Amp board  
  
  //IFinal = IRaw/14.9; //45 Amp board
  int IFinal = getCurrent();
  
  debug("Voltage: ");
  debug(VFinal);
  debug("V. Current: ");
  debug(IFinal);
  debugln("mA.");
  return IFinal * VFinal;
}







