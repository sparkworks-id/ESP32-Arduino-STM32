# HTTP GET Request Using QUECTEL EC600K-CN

In this project, we will make HTTP Get Request with NO AUTHENTICATION using QUECTEL EC600K-CN 4G LTE modules.

## Description
This file contains a sketch about the demo of using the Quectel EC600K 4G LTE module to perform the HTTP Get request process. The EC600K 4G LTE module is connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).


## Instruction
- Before uploading the sketch, you can change the APN according to the APN of your cellular provider.
- Upload the sketch to the ESP32 to run the HTTP GET Request process with to the demo server.
- After the sketch has been successfully uploaded to the ESP32, then open the serial monitor. Type "HTTP" then press enter to perform the HTTP Get Request process.

If the HTTP Get Request process runs successfully, the serial monitor output is shown as below

![](https://github.com/sparkworks-id/misc/blob/main/tutorial/quectel/ec600kcn/ec600kcn-http-serialMonitor-watermark.gif)

You can also change the server host, port, and requestTarget data according to the server address you have.

```
String APN  = "internet";                     // replace with your APN
String HOST = "sparkworks.id";                // replace with your server address
String PORT = "43334";                        // replace with your HTTP port
String requestTarget = "/check";              // request target for url, so the full url is http://sparkworks.id:43334/check
```

Change the parameters above according to your needs

