//create a bot
#define Bot_ID 0xB1//Bot 1
#define BAUD_RATE_COMM_SERIAL 115200
#define BAUD_RATE_XBEE 115200
#define BAUD_RATE_MOT 9600
#define MAX_LENGTH 10

#define START 0xAA
#define CMD   0xC0

//receive commands
#define ERR    0xEE
#define DATA   0xDA
#define JPEG   0xF5
#define PING   0xB0
//send commands
#define REQ    0xDD  //[imu,adc,motors,servo,lcd,encoder,mode,errors...]
#define SET    0xA0  //[auto,man,obj,semi?]
#define SERV   0x5E  //[mask,angle]
#define MOT    0x11  //[motor mask, spped, direction]
#define OBJ    0x5B  //[move, mine, dump, halt]
#define SHUTD  0xE5  //[time till, override flag]

#define MAX_COMMAND_LENGTH 250
#define MAX_COMMANDS 5
#define MAX_SEQUENCE_LENGTH 10
#define OVERFLOW_WARNING_THRESHOLD 6


int execute_pointer = MAX_COMMANDS;
int command_pointer = MAX_COMMANDS;
int commands_size = 0;
String commands[MAX_COMMANDS];
String sequence[MAX_COMMANDS];

int addCommand(String command){
  command_pointer++;
  if(command_pointer > MAX_COMMANDS){
    command_pointer = 0;
    //debugln("Command pointer reset.");  
  }
  commands[command_pointer]=command;
  //debug("$received: ");
  //debugln(command);
  commands_size++;
  return commands_size;
}
void init_commands()
{

  for (int i = 0; i < MAX_COMMANDS; i++){
    commands[i].reserve(MAX_COMMAND_LENGTH);
    sequence[i].reserve(MAX_SEQUENCE_LENGTH);
  }

}

boolean execute(String command){
  boolean ok = false;
  int numSeq = parse(command, "," , sequence);
  debug(" ( ");
  debug(numSeq);
  debugln(" )*");

  if(sequence[0] == "PING"){
    ok = addPacket("PING");//
  }
  else if(sequence[0] == "REQ"){
    if(sequence[1] == "MEAS"){
      debugln("MEASURES:");
      getMeasures();
    }
  }
  else if(sequence[0] == "CMD"){
    debugln("COMMAND!");//addPacket("PING");//
    if(sequence[1] == "MOT"){
      debugln("MOT!");
      if(sequence[2] == "SET"){
        ok = MD49_set(sequence);
      }
      else
        ok = MD49_drive(sequence);
    }
    else if(sequence[1] == "STOP"){
      debugln("STOP!");
      ok = MD49_stop();
    }
    else if(sequence[1] == "RESET"){
      //ok = MD49_reset();
      if(sequence[2] == "OFF"){
        debugln("OFF");
        ok = MD49_enable(LOW);
      }
      if(sequence[2] == "ON"){
        debugln("ON");
        ok = MD49_setup();
      }
    }
  }
  clear(sequence,MAX_SEQUENCE_LENGTH);
  return ok;
}
void clear( String string[] ,int length){
  for(int i = 0; i < length; i++){
    string[i] = 0;
  }
}
int execute(){//String command){

  if(commands_size>0){
    //debugList(commands,0,MAX_COMMANDS,"  -");
    //debugList(commands,command_pointer,commands_size);

    execute_pointer++;
    if(execute_pointer > MAX_COMMANDS){
      execute_pointer = 0;
      //debugln("Execute_pointer reset.");
    }
    String command = commands[execute_pointer];
    debug("*");
    debug(commands_size);
    debug("*executing: ");
    debugln(command);

    execute(command);

    commands[execute_pointer]="";
    commands_size--;
    //debugList(commands,0,MAX_COMMANDS,"  ~");
  }
  return commands_size;
}
