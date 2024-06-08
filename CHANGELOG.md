# Changelog

## Firmware
### [v1.2.2] - 2024-04-13
- Adjusted servo on/off angle
- Reset servo go back to 0 degree each time the movement completed 
- Adjusted battery voltage range
- Adjusted version number (ble characteristic) return format
- Added GPIO trigger as a deep sleep wakeup source

### [v1.2.1] - 2024-03-09
- Fixed battery reading
- Improved battery reading accuracy
- Added bootup battery level check

### [v1.2.0] - 2024-03-09
Added
- Implemented a hybrid sleeping pattern to optimize power consumption: the device enters modem sleep mode between a user-defined period. It reduces frequency and goes into light sleep whenever possible; And it switches to deep sleep mode during other times to maximize battery life

Fixed
- Adjusted power management minimum frequency to 80 before servo operation to prevent shaking, then reset to 10 after completion
- Fix button bouncing effect by making debounce time longer

### [v1.1.0] - 2024-02-21
Added
- Implemented FreeRTOS multitasking with multiple threads
- A custom BLE profile, including standard BLE services battery level, current time, and self-defined services fw, hw version, servo state read and write, 
- Servo controller with PWM using mcpwm lib
- Battery measurement
- Button 
- Blink LED 

## Hardware
### [v1.1.1] - 2024-04-13
Added
- Replaced LM1117 with ME6211 for lower dropout voltage
- Replaced battery measurement voltage divider resistors from 2k to 10k to reduce power consumption

### [v1.1.0] - 2024-03-02
Added
- Removed CP2102/4
- Auto-reset circuit

Fixed
- Switched the order of UART wires between core and external programmer

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

## Additional Tools

### [v1.0.0] - 2024-03-18
Energy Consumption Estimator