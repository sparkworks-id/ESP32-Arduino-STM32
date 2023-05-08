// You can check your A7600C1 Module by entering command in serial monitor.
// First connect your RX pin of LTE module to TX2 pin of ESP32.
// Then connect your TX pin of LTE module to RX2 pin of ESP32.
// Power Up your ESP32 and LTE Modul, and connect ESP32 to your PC.
// Open Serial monitor with baud rate 9600
// Wait for about 20 seconds.
// You can type "CALL" for make a call, or
// type "SMS" to send SMS to the destination number


String admNumber = "+628997"; //change with the your destination phone number
String text = "Hello World!!, This is test message from A7600C1.\n\nStay Awesome"; //text to send via SMS
boolean DEBUG = true; //Print debug in serial monitor

String recv;
uint8_t MODE = 0;
uint8_t CounterInit = 0;
uint8_t CounterCall;

boolean calling = false;

uint32_t Timer[3];

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  delay(20000); //wait for 20 seconds until A7600C1 LTE module is ready
}

void loop() {
  if (MODE == 0)
  {
    if (CounterInit == 0)
    {
      CounterInit += 1;
      String CheckLTE = sendData("AT" , 1000, DEBUG);
      if(CheckLTE.indexOf("OK")>=0)
      {
        Serial.println("LTE Module init Successfully");
      }
      else
      {
        Serial.println("LTE Module init failed");
        while(1){}
      }
      CheckLTE = "";
      Serial.println("Enter the command to start LTE module testing.");
    }
    else
    {
      if (Serial.available())
      {
        recv = Serial.readStringUntil('\n');
        if (recv.indexOf("SMS") >= 0)
        {
          MODE = 1;
          recv = "";
        }
        else if (recv.indexOf("CALL") >= 0)
        {
          MODE = 2;
          recv = "";
        }
        else
        {
          MODE = 0;
          recv = "";
          Serial.println();
          Serial.println("Command Not Recognized");
          Serial.println("Enter the command to start LTE module testing.");
        }
      }
    }
    if (Serial2.available()) {
      Serial.write(Serial2.read());
    }
  }
  else if (MODE == 1)
  {
    SendSMS();
    delay(1000);
    MODE = 0;
    Serial.println("SMS test done");
    Serial.println("Enter the command to start LTE module testing.");

  }
  else if (MODE == 2)
  {
    MakeACall();
    while (calling)
    {
      if (millis() - Timer[0] > 1000)
      {
        Timer[0] = millis();
        CounterCall += 1;
        if (CounterCall == 20)
        {
          CounterCall = 0;
          sendData("AT+CHUP" , 1000, DEBUG);
          calling = false;
        }
      }
    }
    delay(1000);
    MODE = 0;
    Serial.println("CALL test done");
    Serial.println("Enter the command to start LTE module testing.");
  }
}


String sendData(String command, const int timeout, boolean debug)
{
  String response = "";

  Serial2.println(command);
  long int time = millis();
  while ((time + timeout) > millis())
  {
    while (Serial2.available())
    {
      char c = Serial2.read();
      response += c;
    }
  }
  Serial.println(response);
  return response;
}

void MakeACall()
{
  sendData("AT", 1000, DEBUG);
  sendData("ATD" + admNumber + ";" , 1000, DEBUG);
  calling = true;
}

void SendSMS()
{
  sendData("AT", 1000, DEBUG);
  sendData("AT+CCLK?", 1000, DEBUG);
  sendData("AT+CMGF=1", 1000, DEBUG);
  sendData("AT+CNMI=1,2,0,0,0", 1000, DEBUG);
  sendData("AT+CMGS=\"" + admNumber + "\"\r", 1000, DEBUG);
  sendData(text, 1000, DEBUG);
  Serial2.write(0x1A); //ascii code for ctrl-26
}
