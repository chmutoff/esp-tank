## Building an ESP controlled chassis

The ESP TANK project is designed to be used with any custom combination of chassis, motors and power supply. This guide will describe the 2 most popular chassis from Aliexpress and the optimal way to get them up and running. If you decide to build your own chassis, the motor control driver must have 2 control pins for each motor. At the moment, the only tested board is [ESP32-CAM by AiThinker](http://www.ai-thinker.com/pro_view-24.html)

- [TP100 chassis with 12V battery](tp100-12v.md)
- [TP100 chassis with 3.7V battery](tp100-3.7v.md)
- [Acrylic chassis with 3.7V battery](acrylic-3.7v.md)

## Compile and upload the firmware
TODO: compilation chapter goes here


## Schematics:
### TP100 with 12V battery and step down converter for ESP32 and servos.


### TP100 with 18650 in series (3.7V), step up converter for motor driver and step up converter for ESP32 and servos.


### 4WD kit with 18650 in series (3.7V), step up converter for motor driver and step up converter for ESP32 and servos.


### Using 18650 in series and 4 pin motor driver
This is the easiest solution. Because the battery voltage is below 5V we need a 5V step up converter to power the servos, another 5V to power the ESP32-CAM board and one  between 6-12V depending on the selected motor type.
![img](img/Sketch_18650_2PIN_DRV_bb.png)

### Using > 5V battery and 4 pin motor driver
Because the voltage is above 5V we need a step down converter to reduce the voltage to 5V and power the servos and the ESP32-CAM board. Depending on the battery voltage and selected motor you will have to choose wheter to step up or down the voltage.
![img](img/Sketch_9V_2PIN_DRV_bb.png)

### Using 18650 in series and TB6612 motor driver
Connect STBY, PWMA, PWMB to VCC(3.3V or 5V!) pin when using TB6612 instead of a common 4 pin motor driver.
![img](img/Sketch_18650_TB6612_bb.png)
