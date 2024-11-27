/* 
---------------------------------- \\ sparkworks.id // ----------------------------------
Description:
  This file contains a sketch about the demo of using the QUECTEL EC600K-CN 4G LTE 
  module to perform the MQTT Publish and Subscribe. The EC600K-CN 4G LTE module is 
  connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).

Instructions:
  1. Before uploading the sketch, you can change the APN according to the APN of 
  your cellular provider.
  2. Upload the sketch to the ESP32 to run the MQTT Publish and Subscribe process to the 
  demo server.
  3. After the sketch has been successfully uploaded to the ESP32, then open the 
  serial monitor. Type "MQTT" then press enter to perform the MQTT Publish and Subscribe process.

**You can change the server host, port, device name, user, password, and topic according to 
your preferences.

---------------------------------- \\ sparkworks.id // ----------------------------------

*/

String APN  = "internet";                     // replace with your APN
String HOST = "sparkworks.id";                // replace with your server address
String PORT = "34443";                        // replace with your MQTT port

String DEVNAME = "TestDevice";                // change with your device name (must be unix for every device)
String USER = "user1";
String PASS = "user1";
String SUBTOPIC = "test/laptop";
String PUBTOPIC = "test/quectel";

bool MQTT = false;
bool connected = false;

String sendData(String command, const int timeout) {
  String response = "";

  Serial2.println(command);

  long int time = millis();
  while ((time + timeout) > millis()) {
    if (Serial2.available()) {
      response = Serial2.readString();
    }
  }
  
  Serial.print(response);
  return response;
}

String sendDataWaitResponse(String command, String waitString, const int timeout, bool ln) {
  String response = "";
  bool timeoutReach = true;

  if (ln) {
    Serial.println();
    Serial.println(command);
    Serial2.println(command);
  } else {
    Serial.println();
    Serial.print(command);
    Serial2.print(command);
  }

  long int time = millis();
  while ((time + timeout) > millis()) {

    if (Serial2.available()) {
      response = Serial2.readString();
      if (response.indexOf(waitString) >= 0) {
        if (!ln) {
          Serial.println();
        }
        Serial.println("Response: ");
        Serial.println(response);
        timeoutReach = false;
        break;
      }
    }
  }



  if(timeoutReach){
    Serial.println("Request Timeout");
  }
  return response;
}

bool moduleStateCheck() {
  bool moduleState = false;
  
  for (int i = 0; i < 15; i++) {
    String msg = String("");
    msg = sendData("AT+QINISTAT", 1000);
    if (msg.indexOf("+QINISTAT: 1") >= 0) {
      Serial.println("LTE Module had turned on and ready.");
      moduleState = true;
      break;
    }
    delay(1000);
  }
  return moduleState;
}


void setup() {
  // Enable Serial Connection to print status via serial monitor
  Serial.begin(9600);

  // Set serial2 as connection from esp to LTE module
  Serial2.begin(115200);

  delay(5000);
  Serial.println("Type MQTT to do MQTT Connection");

}

void loop() {
  if(Serial.available()){
    String recv = Serial.readStringUntil('\n');
    Serial.println(recv);
    if(recv.indexOf("MQTT")>=0){
      MQTT = true;
    }
  }

  while(MQTT){
    if(!connected){
      // Check if LTE Module is connected to ESP32 properly
      moduleStateCheck();

      // Configure PDP context as 1. APN is based on your cellular provider
      sendDataWaitResponse("AT+QICSGP=1,1,\"" + APN + "\",\"\",\"\",1", "OK", 5000, true);
      
      // Configure receiving mode.
      sendDataWaitResponse("AT+QMTCFG=\"recv/mode\",0,0,1", "OK", 150000, true);

      // Open a network for MQTT client
      sendDataWaitResponse("AT+QMTOPEN=0,\"" + HOST + "\"," + PORT, "+QMTOPEN: 0,0", 10000, true);

      // /Connect a client to MQTT server.
      sendDataWaitResponse("AT+QMTCONN=0,\"" + DEVNAME + "\",\"" + USER + "\",\"" + PASS + "\"", "+QMTCONN: 0,0,0", 10000, true);

      // Subscribe to topics.
      sendDataWaitResponse("AT+QMTSUB=0,1,\"" + SUBTOPIC + "\",2", "+QMTSUB: 0,1,0,2", 150000, true);

      String PUBMESSAGE = "HELLO FROM QUECTEL";
      unsigned int PUBLENGTH = PUBMESSAGE.length();

      // Publish messages
      sendDataWaitResponse("AT+QMTPUBEX=0,0,0,0,\"" + PUBTOPIC + "\"," + String(PUBLENGTH), ">", 10000, true);

      // Fill the message
      sendDataWaitResponse(PUBMESSAGE, "+QMTPUBEX: 0,0,0", 10000, false);

      connected = true;

    }
    else{
      // Print incoming message
      if(Serial2.available()){
        Serial.write(Serial2.read());
      }

      // Listen to serial monitor if there is a command to disconnect mqtt or to publish a message
      // Type DISC to disconnect MQTT Connection
      // Type PUB#your message# to publish a message
      if(Serial.available()){
        String recv = Serial.readStringUntil('\n');
        if(recv.indexOf("DISC")>=0){
          Serial.println();
          Serial.println("DISCONENCT MQTT");
          // Disconnect MQTT Connection
          sendDataWaitResponse("AT+QMTDISC=0", "+QMTDISC: 0,0", 10000, true);
          
          // Close PDP Context
          sendDataWaitResponse("AT+QIDEACT=1", "OK", 10000, true);
          connected = false;
          MQTT = false;
          break;
        }
        else if(recv.indexOf("PUB#")>=0){
          int sep1 = recv.indexOf("PUB#");
          int sep2 = recv.indexOf("#", sep1 + 4);

          String msg = recv.substring(sep1 + 4, sep2);
          int msgLength = msg.length();

          // Publish messages
          sendDataWaitResponse("AT+QMTPUBEX=0,0,0,0,\"" + PUBTOPIC + "\"," + String(msgLength), ">", 10000, true);

          // Fill the message
          sendDataWaitResponse(msg, "+QMTPUBEX: 0,0,0", 10000, false);

        }
      }
    }
  }
}


