# MQTT Publish and Subscribe Using SIMCOM A7670C

In this project, we will make MQTT Publish and Subscribe using SIMCOM A7670C 4G LTE modules.

## Description
This file contains a sketch about the demo of using the SIMCOM A7670C 4G LTE module to perform the MQTT Publish and Subscribe process. The SIMCOM A7670C 4G LTE module is connected to the ESP32 via UART2/SERIAL2 (GPIO16, GPIO17).

## Instruction
- Before uploading the sketch, you can change the APN according to the APN of your cellular provider.
- Upload the sketch to the ESP32 to run the MQTT Publish and Subscribe process to the demo server.
- After the sketch has been successfully uploaded to the ESP32, then open the serial monitor. 
- Type "MQTT" then press enter to perform the MQTT Connection to Demo Server. The serial monitor output is shown as below:

![](https://github.com/sparkworks-id/misc/blob/main/tutorial/simcom/a7670c/a7670c-mqtt-connect-serialMonitor-watermark.gif)


- After the MQTT connection is connected, you can publish a message with the topic that has been set in the sketch. To publish a message, type "**PUB#your_message#**" in the serial monitor, then press enter. 
The serial monitor output is shown as below (The PowerShell section shows the client that subscribes to the topic published by the module)

![](https://github.com/sparkworks-id/misc/blob/main/tutorial/simcom/a7670c/a7670c-mqtt-publish-serialMonitor-watermark.gif)


- If there is a new incoming message, the message will automatically appear on the serial monitor as shown below

![](https://github.com/sparkworks-id/misc/blob/main/tutorial/simcom/a7670c/a7670c-mqtt-subscribe-serialMonitor-watermark.gif)


- To disconnect the MQTT connection, type "DISC" in the serial monitor. The serial monitor output is shown as below

![](https://github.com/sparkworks-id/misc/blob/main/tutorial/simcom/a7670c/a7670c-mqtt-disconnect-serialMonitor-watermark.gif)
