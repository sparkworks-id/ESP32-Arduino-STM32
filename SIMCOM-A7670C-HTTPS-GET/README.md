# HTTPS GET Request Using SIMCOM A7670C

In this project, we will make HTTPS Get Request with NO AUTHENTICATION using SIMCOM A7670C 4G LTE modules.

## Description
This file contains a sketch about the demo of using the SIMCOM A7670C 4G LTE module to perform the HTTPS Get request process with NO AUTHENTICATION. The A7670C 4G LTE module is connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).

## Instruction
- Before uploading the sketch, you can change the APN according to the APN of your cellular provider.
- Upload the sketch to the ESP32 to run the HTTPS GET Request process with NO AUTHENTICATION to the demo server.
- After the sketch has been successfully uploaded to the ESP32, then open the serial monitor. Type "HTTPS" then press enter to perform the HTTPS Get Request process.

If the HTTPS Get Request process runs successfully, the serial monitor output is shown as below

![](https://sparkworks.id/repo/tutorial/simcom/a7670c/a7670c-https-serialMonitor-watermark.gif)


You can also change the server host, port, and requestTarget data according to the server address you have.

```
String APN  = "internet";                                 // replace with your APN
String HOST = "sparkworks.id";                            // replace with your server address
String requestTarget = "/playground/check";               // request target for url, so the full url is https://sparkworks.id/playground/check
```

Change the parameters above according to your needs

