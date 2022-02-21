
- Acrylic car kit (W: 15cm, H: 6cm, L: 25cm) and TT gear motors, 3-6V, 90-200rpm.
![img](img/car-chassis.png)


### Battery
- 12V LiPo battery or 3x 18650 in paralell. 
- Multple 3.7V 18650 in series:
  - BMS: 1S-3876 (5A). Battery protection, MUST be installed for safety!
  - Charger: TP4056 (1A) or IP2312 (3A). Optional, to charge the batteries on-board with a USB cable. In case of TP405, choose the version without integrated BMS or it will constantly turn off the power due to high starting current of the motors!
  - 18650 battery holder. TP100 chassis can fit 3x batteries underneeth. Car chassis can fit 2x between motors or more batteries on the top.

### Step-UP (boost) converters 

| Model    | Current        | In Voltage   | Out Voltage   |
|---       |---             |---           |---            |
| SX1308   | 2A             | 2 - 24V      | 2 - 28V       |
| MT3608   | 2A             | 2 - 24V      | 5 - 28V       |


### Motor drivers

| Model   | Current        | Voltage     | Notes                                               |
|---      |---             |---          | ---                                                 |
| L298N   | 1A Max: 2A     | 4 - 50V     | Big voltage drop, high temperature, low efficiency. |
| TA6586  | 5A Max: 7A     | 3 - 14V     | Good specs. Hard do find.                           |
| MX1508  | 1.5A Max: 2.5A | 2 - 9.6V    | Max voltage is 10V!                                 |
| DRV8833 | 1.5A Max: 2.5A | 2.7 - 10.8V | Max voltage is 10.8V!                               |
| L9110S  | 0.8A Max: 1.5A | 2.5 - 12V   | Sometimes, they suddenly burn out!                  |
| TB6612  | 1.2A Max: 3A   | 4.5 - 13.5V | Connect STBY, PWMA, PWMB to VCC pin.                |

To attach a servo use standard servo pan/tilt mount with this [3D printed clip](https://www.thingiverse.com/thing:4934734)
