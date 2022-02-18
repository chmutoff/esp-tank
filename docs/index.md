## Building an ESP controlled chassis

The ESP TANK project is designed to be used with any custom combination of chassis, motors and power supply. This guide will describe the 2 most popular chassis from Aliexpress and the optimal way to get them up and running. If you decide to build your own chassis, the only requirement is that the motor control driver must have 2 control pins for each motor.

- Select parts
- Assembly
- Compile and upload the firmware

### Chassis
- Mini TP100 chassis (W: 15cm, H: 6cm, L: 20cm) and 33GB-520 motors, 6-12V, 170-350rpm.
![img](img/tp100-tank-chassis.png)
- Acrylic car kit (W: 15cm, H: 6cm, L: 25cm) and TT gear motors, 3-6V, 90-200rpm.
![img](img/car-chassis.png)

Choose one of the following chassis configurations
| Chassis | IN V | Motor    | Motor V     | ESP & Servo V |
|---      |---   |---       |---          |---            |
| TP100   | 12V  | 33GB-520 | 12V direct  | 5V Step Down  | 
| TP100   | 3.7V | 33GB-520 | 12V Step UP | 5V Step UP    |
| 4WD Kit | 3.7V | TT motor | 6V Step UP  | 5V Step UP    |

### Battery
- 12V LiPo battery or 3x 18650 in paralell. 
- Multple 3.7V 18650 in series:
  - BMS: 1S-3876 (5A). Battery protection, MUST be installed for safety!
  - Charger: TP4056 (1A) or IP2312 (3A). Optional, to charge the batteries on-board with a USB cable. In case of TP405, choose the version without integrated BMS or it will constantly turn off the power due to high starting current of the motors!
  - 18650 battery holder. TP100 chassis can fit 3x batteries underneeth. Car chassis can fit 2x between motors or more batteries on the top.

### Step-UP (boost) converters 

| Model    | Current        | In Voltage   | Out Voltage   | Notes             
|---       |---             |---           |---            |---                        |
| SX1308   | 2A             | 2 - 24V      | 2 - 28V       | Small and powerful        |
| MT3608   | 2A             | 2 - 24V      | 5 - 28V       |                           |

### Step-down (buck) converters 

| Model    | Current        | In Voltage   | Out Voltage   | Notes                     |
|---       |---             |---           |---            |---                        |
| MP1584EN | 3A             | 4.5 - 28V    | 0.8 - 20V     | Small and powerful        |
| MP2315   | 1.5-2A         | 4.5 - 24V    | 0.8 - 22V     | Tiny and powerful module  |
| LM2596   | 3A             | 4.5 - 40V    | 1.23 - 37V    | Overpowered. Big size.    |

### Motor drivers

| Model   | Current        | Voltage     | Notes                                               |
|---      |---             |---          | ---                                                 |
| L298N   | 1A Max: 2A     | 4 - 50V     | Big voltage drop, high temperature, low efficiency. |
| TA6586  | 5A Max: 7A     | 3 - 14V     | Good specs. Hard do find.                           |
| MX1508  | 1.5A Max: 2.5A | 2 - 9.6V    | Max voltage is 10V!                                 |
| DRV8833 | 1.5A Max: 2.5A | 2.7 - 10.8V | Max voltage is 10.8V!                               |
| L9110S  | 0.8A Max: 1.5A | 2.5 - 12V   | Sometimes, they suddenly burn out!                  |
| TB6612  | 1.2A Max: 3A   | 4.5 - 13.5V | Connect STBY, PWMA, PWMB to VCC pin.                |

**NOTE**: It is recommended to put a 100nf (0.1uF) capacitor between the motor contacts to avoid interference.

### Servos
The two types of servos are: 
- SG90: micro servo motor with plastic gears
- MG90: micro servo motor with metal gears (better)

Both servos need a 5V power supply. To attach a servo use standard servo pan/tilt mount with this [3D printed clip](https://www.thingiverse.com/thing:4934734)

### Main board
At the moment, the only tested board is [ESP32-CAM by AiThinker](http://www.ai-thinker.com/pro_view-24.html)

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
