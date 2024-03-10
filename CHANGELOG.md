# Changelog

## Firmware
### [v1.0.2] - 2024-03-09
Added
- Implemented a hybrid sleeping pattern to optimize power consumption: the device enters modem sleep mode between a user-defined period. It reduces frequency and goes into light sleep whenever possible; And it switches to deep sleep mode during other times to maximize battery life

Fixed
- Adjusted power management minimum frequency to 80 before servo operation to prevent shaking, then reset to 10 after completion
- Fix button bouncing effect by making debounce time longer

### [v1.0.1] - 2024-02-21
Added
- Implemented FreeRTOS multitasking with multiple threads
- A custom BLE profile, including standard BLE services battery level, current time, and self-defined services fw, hw version, servo state read and write, 
- Servo controller with PWM using mcpwm lib
- Battery measurement
- Button 
- Blink LED 

## Hardware
### [v1.0.1] - 2024-03-2
Added
- Remove CP2102/4
- Auto-reset circuit

Fixed
- Switch the order of UART wires between core and external programmer

### [v1.0.0] - 2024-01-16
Added
- Main core ESP32S3 WROOM 
- Voltage regulator LM1117
- Battery management IC MCP73831
- USB to TTL convertor CP2102
- A pair of buttons for boot mode selection
- A user button 
- Servo motor PWM pins
- A ESD protection IC SP0503BAHT

