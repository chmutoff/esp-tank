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
        #stream-container {
            position: relative;
            width: max-content;
            margin: auto;
        }
        #stop-stream,
        #start-stream {
            position: absolute;
            right: 5px;
            top: 5px;
            z-index: 99;
            background: #ff3333;
            width: 16px;
            height: 16px;
            border-radius: 100px;
            color: #fff;
            text-align: center;
            line-height: 18px;
            cursor: pointer;
        }
    </style>
</head>

<body>
    <div id="main">
        <div id="framesize-container">
            <label for="framesize">Res</label>
            <select id="framesize"
                onchange="aux('framesize', this.value); setTimeout(() => rotateImg(document.getElementById('rotation').value), 500);">
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
            <label for="rotation">Rot</label>
            <select id="rotation" onchange="rotateImg(this.value);">
                <option value="270">-90&deg;</option>
                <option value="0" selected>0&deg;</option>
                <option value="90">90&deg;</option>
            </select>
        </div>
        <div id="stream-container">
            <img src="" id="image">
            <div id="stop-stream" onclick="stopStream(event);">&#xD7;</div>
            <div id="start-stream" onclick="startStream(event);" style="display: none;">&#x298A;</div>
        </div>
        <div id="joy1-container"></div>
        <div id="slider-container">
            <div>
                <span>F: </span>
                <input class="slider" type="range" min="0" max="100" value="0"
                    oninput="this.nextElementSibling.innerHTML = this.value + '%'; aux('led', this.value);">
                <span class="tooltiptext">0%</span>
            </div>
            <div>
                <span>H: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.nextElementSibling.innerHTML = this.value + '&#176;'; aux('x', this.value);">
                <span class="tooltiptext">90&#176;</span>
            </div>
            <div>
                <span>V: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.nextElementSibling.innerHTML = this.value + '&#176;'; aux('y', this.value);">
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
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/control?x=" + joy1xVal + "&y=" + joy1yVal, true);
            xhr.send();
        }, 100);

        var image = document.getElementById("image");
        window.onload = image.src = window.location.origin + ":81/stream";

        function rotateImg(deg) {
            if (deg == 0) {
                image.style.transform = '';
                image.parentElement.style.height = '';
                image.parentElement.style.width = '';
                return;
            } else if (deg == 90) {
                image.style.transform = 'rotate(90deg) translateY(-100%)';
            } else if (deg == 270) {
                image.style.transform = 'rotate(270deg) translateX(-100%)';
            } else {
                console.error("Wrong degree!");
                return;
            }
            image.style.transformOrigin = 'top left';
            image.parentElement.style.height = image.width + 'px';
            image.parentElement.style.width = image.height + 'px';
        }

        function aux(fn, val) {
            try {
                fetch('/aux?' + fn + '=' + val);
            } catch (e) {
                console.error(e.message);
            }
        }

        function stopStream(event) {
            event.target.style.display = 'none';
            event.target.nextElementSibling.style.display = '';
            window.stop();
        }

        function startStream(e) {
            event.target.style.display = 'none';
            event.target.previousElementSibling.style.display = '';
            image.src = window.location.origin + ":81/stream";
        }
    </script>
</body>
</html>)";
