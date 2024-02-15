idf_component_register(SRCS "main.c" "blink/blink.c" "ble_server/ble_server.c" "servo/servo.c" "button/button.c" "battery_management/battery_management.c"
                       INCLUDE_DIRS "." "./ble_server" "./blink" "./servo" "./button" "./battery_management")
