/**
 * 
 * @file index_html.cpp
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Control webpage
 *  
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 * 
 */
#include <pgmspace.h>

static const char INDEX_HTML[] PROGMEM = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <title>ESP-Tank</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: sans-serif;
            text-align: center;
            margin: 0px auto;
            padding-top: 30px;
            background: #252525;
            color: #eaeaea;
        }
        img {
            width: auto;
            max-width: 100%;
            height: auto;
        }
        #main {
            margin: auto;
        }
        #joy1-container {
            width: 320px;
            height: 320px;
            margin: 10px auto;
        }
        #joystick1 {
            border: 2px solid #eaeaea;
        }
        #slider-container {
            width: 320px;
            margin: auto;
        }
        .slider {
            display: inline-block;
            margin: 15px 0;
            width: 70%;
        }
        .tooltiptext {
            display: inline-block;
            background-color: #555;
            color: #fff;
            text-align: center;
            border-radius: 6px;
            padding: 5px;
            width: 15%;
        }
    </style>
</head>

<body>
    <div id="main">
        <div class="framesize-container">  
            <label for="framesize">Resolution</label>
            <select id="framesize" onchange="try{fetch(document.location.origin+'/aux?framesize='+this.value);}catch(e){}">
                <option value="13">UXGA (1600x1200)</option>
                <option value="12">SXGA (1280x1024)</option>
                <option value="11">HD (1280x720)</option>
                <option value="10">XGA (1024x768)</option>
                <option value="9">SVGA (800x600)</option>
                <option value="8">VGA (640x480)</option>
                <option value="7">HVGA (480x320)</option>
                <option value="6">CIF (400x296)</option>
                <option value="5" selected>QVGA (320x240)</option>
                <option value="4">240X240 (240X240)</option>
                <option value="3">HQVGA (240x176)</option>
                <option value="2">QCIF (176x144)</option>
                <option value="1">QQVGA (160x120)</option>
                <option value="0">96x96 (96x96)</option>
            </select>
        </div>
        <img src="" id="photo">
        <div id="joy1-container"></div>
        <div id="joy1-info-container">
            <label for="joy1X">X1</label>
            <input id="joy1X" type="text" disabled /></br>
            <label for="joy1Y">Y1</label>
            <input id="joy1Y" type="text" disabled />
        </div>
        <div id="slider-container">
            <div>
                <span>F: </span>
                <input class="slider" type="range" min="0" max="100" value="0"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '%'; try{fetch(document.location.origin+'/aux?led='+this.value);}catch(e){}">
                <span class="tooltiptext">0%</span>
            </div>
            <div>
                <span>H: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '&#176;'; try{fetch(document.location.origin+'/aux?x='+this.value);}catch(e){}">
                <span class="tooltiptext">90&#176;</span>
            </div>
            <div>
                <span>V: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '&#176;'; try{fetch(document.location.origin+'/aux?y='+this.value);}catch(e){}">
                <span class="tooltiptext">90&#176;</span>
            </div>
        </div>
    </div>
    <script src="joy.js"></script>
    <script>
        var Joy1 = new JoyStick('joy1-container', { "title": "joystick1", "autoReturnToCenter": true, internalFillColor: "darkred", internalStrokeColor: "black", externalStrokeColor: "darkred" });
        var joy1X = document.getElementById("joy1X");
        var joy1Y = document.getElementById("joy1Y");

        setInterval(function () {
            var joy1xVal = Joy1.GetX();
            var joy1yVal = Joy1.GetY();
            joy1X.value = joy1xVal;
            joy1Y.value = joy1yVal;

            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/control?x=" + joy1xVal + "&y=" + joy1yVal, true);
            xhr.send();

        }, 100);

        window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
    </script>
</body>
</html>)";
