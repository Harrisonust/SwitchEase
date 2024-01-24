idf_component_register(SRCS "main.c" "blink/blink.c" "ble_server/ble_server.c" "servo/servo.c" "button/button.c"
                       INCLUDE_DIRS "." "./ble_server" "./blink" "./servo" "./button")
