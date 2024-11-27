/* 
---------------------------------- \\ sparkworks.id // ----------------------------------
Description:
  This file contains a sketch about the demo of using the SIMCOM A7670C 
  4G LTE module to perform the HTTPS Get request process with NO AUTHENTICATION.
  The A7670C 4G LTE module is connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).

Instructions:
  1. Before uploading the sketch, you can change the APN according to the APN of 
  your cellular provider.
  2. Upload the sketch to the ESP32 to run the HTTPS GET Request process with NO 
  AUTHENTICATION to the demo server.
  3. After the sketch has been successfully uploaded to the ESP32, then open the 
  serial monitor. Type "HTTPS" then press enter to perform the HTTPS Get Request process.

**You can change the server host, port, and requestTarget data according to the server 
address you have.

---------------------------------- \\ sparkworks.id // ----------------------------------

*/

String APN  = "internet";                                 // replace with your APN
String HOST = "sparkworks.id";                            // replace with your server address
String requestTarget = "/playground/check";               // request target for url, so the full url is https://sparkworks.id/playground/check

String URL = "https://" + HOST + requestTarget;

bool HTTPSGET = false;

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
  Serial.println("Type HTTPS to do https get request");

}

void loop() {
  if(Serial.available()){
    String recv = Serial.readStringUntil('\n');
    Serial.println(recv);
    if(recv.indexOf("HTTPS")>=0){
      HTTPSGET = true;
    }
  }

  while(HTTPSGET){
    // Check if LTE Module is connected to ESP32 properly
    moduleStateCheck();

    // Attach PDP
    sendDataWaitResponse("AT+CGATT=1", "OK", 5000, true);
    
    // Configure APN
    sendDataWaitResponse("AT+CGDCONT=1,\"IP\",\"" + APN + "\"", "OK", 5000, true);

    // Activate PDP Context
    sendDataWaitResponse("AT+CGACT=1,1", "OK", 15000, true);

    // Start HTTPS service
    sendDataWaitResponse("AT+HTTPINIT", "OK", 15000, true);

    // Set the URL which will be accessed, for HTTPS, the request URL begins with "https://"
    sendDataWaitResponse("AT+HTTPPARA=\"URL\",\"" + URL + "\"", "OK", 5000, true);

    // Send HTTPS GET request
    sendDataWaitResponse("AT+HTTPACTION=0", "+HTTPACTION:", 15000, true);

    // Read HTTPS response header
    sendDataWaitResponse("AT+HTTPHEAD", "+HTTPHEAD:", 15000, true);

    // Read the response information of HTTPS server, the length to read is 500 bytes
    sendDataWaitResponse("AT+HTTPREAD=0,500", "+HTTPREAD:", 15000, true);

    // Stop HTTPS Service
    sendDataWaitResponse("AT+HTTPTERM", "OK", 5000, true);

    HTTPSGET = false;
  }

}


