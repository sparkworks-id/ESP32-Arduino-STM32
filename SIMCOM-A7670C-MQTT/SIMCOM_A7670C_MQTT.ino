/* 
---------------------------------- \\ sparkworks.id // ----------------------------------
Description:
  This file contains a sketch about the demo of using the SIMCOM A7670C 4G LTE 
  module to perform the MQTT Publish and Subscribe. The A7670C 4G LTE module is 
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
String PUBTOPIC = "test/simcom";

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
    msg = sendData("AT", 1000);
    if (msg.indexOf("OK") >= 0) {
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

      // Attach PDP
      sendDataWaitResponse("AT+CGATT=1", "OK", 5000, true);
      
      // Configure APN
      sendDataWaitResponse("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", "OK", 5000, true);

      // Activate PDP Context
      sendDataWaitResponse("AT+CGACT=1,1", "OK", 10000, true);
      
      // Start MQTT service, 
      sendDataWaitResponse("AT+CMQTTSTART", "OK", 10000, true);

      // Subscribe to topics.
      sendDataWaitResponse("AT+CMQTTACCQ=0, \"" + DEVNAME + "\"", "", 5000, true);

      String WILLTOPIC = "WILL/" + DEVNAME;
      String WILLMESSAGE = DEVNAME + " - Connection Lost";

      // Set the will topic for the CONNECT message
      sendDataWaitResponse("AT+CMQTTWILLTOPIC=0," + String(WILLTOPIC.length()), ">", 10000, true);

      // Fill the will topic
      sendDataWaitResponse(WILLTOPIC, "OK", 5000, false);
      
      // Set the will message for the CONNECT message
      sendDataWaitResponse("AT+CMQTTWILLMSG=0," + String(WILLMESSAGE.length()) + ",1", ">", 10000, true);
      
      // Fill the will message
      sendDataWaitResponse(WILLMESSAGE, "OK", 5000, false);

      // Connect to a MQTT server
      sendDataWaitResponse("AT+CMQTTCONNECT=0,\"tcp://" + HOST + ":" + PORT + "\",60,1,\"" + USER + "\",\"" + PASS + "\"", "+CMQTTCONNECT:", 120000, true);

      // Subscribe a topic
      sendDataWaitResponse("AT+CMQTTSUB=0," + String(SUBTOPIC.length()) + ",1", ">", 10000, true);

      // Fill the subscribe topic 
      sendDataWaitResponse(SUBTOPIC, "+CMQTTSUB:", 60000, false);
      
      // Set the Topic for Publish messages
      sendDataWaitResponse("AT+CMQTTTOPIC=0," + String(PUBTOPIC.length()), ">", 10000, true);

      // Fill the topic
      sendDataWaitResponse(PUBTOPIC, "OK", 60000, false);

      String PAYLOAD = "HELLO FROM SIMCOM";

      // Set the payload for the PUBLISH message
      sendDataWaitResponse("AT+CMQTTPAYLOAD=0," + String(PAYLOAD.length()), ">", 10000, true);

      // Fill the payload message
      sendDataWaitResponse(PAYLOAD, "OK", 60000, false);

      // Publish the payload message
      sendDataWaitResponse("AT+CMQTTPUB=0,1,60", "+CMQTTPUB:", 60000, true);

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
          sendDataWaitResponse("AT+CMQTTDISC=0,120", "+CMQTTDISC:", 10000, true);
          
          // Release the client
          sendDataWaitResponse("AT+CMQTTREL=0", "OK", 10000, true);

          // Stop MQTT Service
          sendDataWaitResponse("AT+CMQTTSTOP", "+CMQTTSTOP:", 10000, true);

          Serial.println("MQTT DISCONNECT");

          connected = false;
          MQTT = false;
          break;
        }
        else if(recv.indexOf("PUB#")>=0){
          int sep1 = recv.indexOf("PUB#");
          int sep2 = recv.indexOf("#", sep1 + 4);

          String msg = recv.substring(sep1 + 4, sep2);
          int msgLength = msg.length();

          // Set the Topic for Publish messages
          sendDataWaitResponse("AT+CMQTTTOPIC=0," + String(PUBTOPIC.length()), ">", 10000, true);

          // Fill the topic
          sendDataWaitResponse(PUBTOPIC, "OK", 61000, false);

          // Set the payload for the PUBLISH message
          sendDataWaitResponse("AT+CMQTTPAYLOAD=0," + String(msgLength), ">", 10000, true);

          // Fill the payload message
          sendDataWaitResponse(msg, "OK", 61000, false);

          // Publish the payload message
          sendDataWaitResponse("AT+CMQTTPUB=0,1,60", "+CMQTTPUB:", 61000, true);

        }
      }
    }
  }
}


