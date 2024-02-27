| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

This project builds a smart switch that can be controlled by a smartphone via BLE

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* An ESP32-S3
* An USB type C cable for power supply and programming

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Development History
### Firmware
v1.0.2 Bybrid sleeping pattern to save power consumption, add servo enable
v1.0.1 Finish the basic functions: ble, servo, battery management, button, led

### Hardware
v1.0.1 Remove CP2102/4, add auto-reset circuit, add servo enable
v1.0.0 Fail