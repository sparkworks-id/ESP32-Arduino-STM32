# SIMCOM A7600C1 Test with ESP32

In this project, we will make calls and send SMS using the LTE A7600C1 and ESP32 modules.

## Wiring
The power source for the A7600C1 module uses a Li-ion battery with a voltage of 3.7 - 4.2 volts connected to VBAT pin, while the ESP32 uses a 5v voltage from USB. For the connection between the LTE A7600C1 module and ESP32 as follows

| A7600C1 | ESP32 |
| ------ | ------ |
| TX | RX2 (GPIO17) |
| RX | TX2 (GPIO16) |
| GND | GND |

## Instruction
- Power up LTE Module and ESP32.
- Open the sketch, change your destination number.
- Upload the sketch to ESP32.
- Open Serial Monitor with baud rate 9600.
- Wait until A7600C1 is ready.
- Type "CALL" and hit enter button to make a call to destination number.
- Type "SMS" and hit enter button to make a call to destination number.