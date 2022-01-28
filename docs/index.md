### Battery selection
- 12V battery. Usually come with a spare charger.
- 18650 in series. Output: 3.7V
- 3x 18650 in paralell. Output 11.1V. Needs special charging/balancing circuit. Not recommended.

When using 18650 batteries in series make sure to connect a BMS circuit and a charging circuit. Following parts are required for a safe 18650 assembly:
- Charger: TP4056 (1A) or IP2312 (3A)
- BMS: 1S-3876 (5A)
- 18650 battery holder

**NOTE:** in case of TP405 choose the version without integrated BMS or it will constantly turn off the power due to high starting current of the motors!

#### Step up converters (boosters)

| Model    | Current        | In Voltage   | Out Voltage   | Notes             
|---       |---             |---           |---            |---                        |
| SX1308   | 2A             | 2 - 24V      | 2 - 28V       | Small and powerful        |
| MT3608   | 2A             | 2 - 24V      | 5 - 28V       |                           |

#### Step down converters

| Model    | Current        | In Voltage   | Out Voltage   | Notes                     |
|---       |---             |---           |---            |---                        |
| MP1584EN | 3A             | 4.5 - 28V    | 0.8 - 20V     | Small and powerful        |
| MP2315   | 1.5-2A         | 4.5 - 24V    | 0.8 - 22V     | Tiny and powerful module  |
| LM2596   | 3A             | 4.5 - 40V    | 1.23 - 37V    | Overpowered. Big size.    |

### Motor drivers

| Model  | Current        | Voltage     | Notes                                               |
|---     |---             |---          | ---                                                 |
| L298N  | 1A Max: 2A     | 4 - 50V     | Big voltage drop, high temperature, low efficiency. |
| TA6586 | 5A Max: 7A     | 3 - 14V     | Good specs. Hard do find                            |
| MX1508 | 1.5A Max: 2.5A | 2 - 9.6V    | Max voltage is 10V!                                 |
| L9110S | 0.8A Max: 1.5A | 2.5 - 12V   | Sometimes they suddenly burn out!                   |
| TB6612 | 1.2A Max: 3A   | 4.5 - 13.5V | Connect STBY, PWMA, PWMB to VCC pin.                |

### Servo motors
The two types of servos are: 
- SG90: micro servo motor with plastic gears
- MG90: micro servo motor with metal gears

Both servos need a 5V power supply. To attach a servo use standard servo pan/tilt mount with the [Clip](https://www.thingiverse.com/thing:4934734)

### Main board
At the moment, the only tested board is [ESP32-CAM by AiThinker](http://www.ai-thinker.com/pro_view-24.html)

## Welcome to GitHub Pages

You can use the [editor on GitHub](https://github.com/chmutoff/esp-tank/edit/master/docs/index.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [Basic writing and formatting syntax](https://docs.github.com/en/github/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/chmutoff/esp-tank/settings/pages). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
