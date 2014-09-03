String xbeeString;// = "";         // a string to hold incoming data
String serialString;// = "";         // a string to hold incoming data
String outPacket[MAX_LENGTH];// = "";         // a string to hold incoming data
int outPacket_length = 0;    //to hold how long the string is


void init_comm()
{
  Serial.begin(BAUD_RATE_COMM_SERIAL);//USB connection
  Serial1.begin(BAUD_RATE_XBEE);//xbee connection
  Serial2.begin(BAUD_RATE_MOT);//motor control 1
  Serial3.begin(BAUD_RATE_MOT);//motor control 2
}
int addPacket(String data){
  outPacket[outPacket_length] = data;
  return outPacket_length++;//
}
void comm_send(){
  if(outPacket_length)
    debugln("Sending data:");
  for(int i = 0; i < outPacket_length; i++){
    Serial.println(outPacket[i]);//USB
    Serial1.println(outPacket[i]);//xbee
  }
  outPacket_length = 0;
}
// Serial Controller

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    if (inChar == '\n') {
      //debug("# ");
      //debugln(xbeeString);
      addCommand(serialString);
      serialString = "";
    }      
    else serialString += inChar;// add it to the inputString:
  }
}
void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read(); 
    if (inChar == '\n') {
      //debug("# ");
      //debugln(xbeeString);
      addCommand(xbeeString);
      xbeeString = "";
    }      
    else xbeeString += inChar;// add it to the inputString:
  }
}
