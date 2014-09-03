
/*********************
 * 
 * LunaBot v3
 * 
 **********************/
#define debug( stuff ) if(DEBUG){Serial.print(stuff);}
#define debugln( stuff ) if(DEBUG){Serial.println(stuff);}

#define STATUS_LED 13
#define MOTOR_RELAY 51 //not sure if this is a motor relay, but try

boolean DEBUG = true;

void setup() {

  pinMode (MOTOR_RELAY,OUTPUT);
  pinMode (STATUS_LED,OUTPUT);  // Status LED

  init_comm();//  
  MD49_setup();
  setup_sensors();
  updateTime();
  Serial.println("Welcome to LunaBot v3.0");
  Serial.print("Battery Voltage: ");
  Serial.print(getVoltage());
  Serial.println("V.");
}
void loop(){
  //time = comm_receive();
  int commands_remain = execute();
  comm_send();
  updateTime();
}
