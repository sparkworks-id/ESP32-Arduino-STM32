/* 
---------------------------------- \\ sparkworks.id // ----------------------------------
Description:
  This file contains a sketch about the demo of using the Quectel EC600K 
  4G LTE module to perform the HTTPS Get request process with NO AUTHENTICATION.
  The EC600K 4G LTE module is connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).

Instructions:
  1. Before uploading the sketch, you can change the APN according to the APN of 
  your cellular provider.
  2. Upload the sketch to the ESP32 to run the HTTPS GET Request process with NO 
  AUTHENTICATIONto the demo server.
  3. After the sketch has been successfully uploaded to the ESP32, then open the 
  serial monitor. Type "HTTPS" then press enter to perform the HTTPS Get Request process.

**You can change the server host, port, and requestTarget data according to the server 
address you have.

---------------------------------- \\ sparkworks.id // ----------------------------------

*/

String APN  = "internet";                     // replace with your APN
String HOST = "sparkworks.id";                // replace with your server address
String PORT = "443";                          // for https use port 433
String requestTarget = "/playground/check";   // request target for url, so the full url is https://sparkworks.id:443/playground/check

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

    // Configure PDP context as 1. APN is based on your cellular provider
    sendDataWaitResponse("AT+QICSGP=1,1,\"" + APN + "\",\"\",\"\",1", "OK", 5000, true);
    
    // Activate a PDP Context
    sendDataWaitResponse("AT+QIACT=1", "OK", 150000, true);

    // Set SSL context ID and set the module to automatically set SSL Version.
    sendDataWaitResponse("AT+QSSLCFG=\"sslversion\",1,4", "OK", 10000, true);

    // Set SSL context ID as 1 and authentication mode as No authentication.
    sendDataWaitResponse("AT+QSSLCFG=\"seclevel\",1,0", "OK", 10000, true);

    // Set up an SSL Connection and Enter Direct Push Mode
    sendDataWaitResponse("AT+QSSLOPEN=1,1,1,\""+ HOST +"\"," + PORT + ",1", "+QSSLOPEN: 1,", 10000, true);

    // Make HTTP GET syntax, for more information go to https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET
    String get = "GET " + requestTarget + " HTTP/1.1\r\n";
    get += "Host: " + HOST + "\r\n";
    get += "User-Agent: quectel/ec600k\r\n";
    get += "Accept: */*\r\n";
    get += "Connection: keep-alive\r\n";
    get += "\r\n";
    
    int getLen = get.length();

    // Send fixed-length data.
    sendDataWaitResponse("AT+QSSLSEND=1," + String(getLen), ">", 1000, true);

    // Send HTTP GET Syntax and wait for the response from server
    // server response will appear automatically, start with "+QSSLURC:"
    sendDataWaitResponse(get, "+QSSLURC:", 150000, false);

    // Close an SSL connection
    sendDataWaitResponse("AT+QSSLCLOSE=1", "OK", 10000, true);

    // Close PDP Context
    sendDataWaitResponse("AT+QIDEACT=1", "OK", 10000, true);

    HTTPSGET = false;
  }

}


