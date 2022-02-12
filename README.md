# ESP Tank
This project allows to control any chassis with fixed wheels (like TP100 mini tank chassis or the famous 4wd chassis) using ESP32-CAM.

## Assembly
Connect the motor controller and two servos
| Part | ESP32-CAM Pin|
|---   |---
| Left motor A1     | 14 |
| Left motor B1     | 15 |
| Right motor A2    | 13 |
| Right motor B2    | 2  |
| Horizontal servo  | 12 |
| Vertical servo    | 3  |

## Compilation
Change the `WIFI_SSID` and `WIFI_PASS` in `platrformio.ini` file or create a new file ending with `_env.ini` (i.e.: `my_env.ini`) to avoid git from overwriting your changes. You can also override the motor pins, servo pins, etc.. Full list of options will be available on the [project documentation page](https://chmutoff.github.io/esp-tank/).
```
[env:esp32cam-serial]
board = esp32cam
monitor_speed = 115200
upload_speed = 921600
board_build.partitions = min_spiffs.csv
build_flags =
    ${env.build_flags}
    -DWIFI_SSID="MyRealWifiName"
    -DWIFI_PASS="MyRealWifiPassword"
    -DCAMERA_MODEL="AI_THINKER"
```
Once the device is flashed, it is possilbe to use OTA. Example ota environment:
```
[env:esp32cam-ota]
board = esp32cam
monitor_speed = 115200
upload_protocol = espota
upload_port = 192.168.1.10
upload_flags =
    --port=3232
    --timeout=30
board_build.partitions = min_spiffs.csv
build_flags =
    ${env.build_flags}
    -DWIFI_SSID="MyRealWifiName"
    -DWIFI_PASS="MyRealWifiPassword"
    -DCAMERA_MODEL="AI_THINKER"
    -DCORE_DEBUG_LEVEL=3
```
## Testing
This project has 3 tests, one for verifying the flash led, another one to check the motors and the last one to check that servos are working fine.
