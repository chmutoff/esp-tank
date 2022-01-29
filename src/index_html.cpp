#include <pgmspace.h>

static const char INDEX_HTML[] PROGMEM = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <title>ESP-Tank</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin: 0px auto;
            padding-top: 30px;
        }
        img {
            width: auto;
            max-width: 100%;
            height: auto;
        }
        #main {
            width: 300px;
            height: 300px;
            margin: auto;
        }
        #joy1Div {
            width: 300px;
            height: 300px;
            margin: 10px auto;
        }
        #joystick1 {
            border: 2px solid DarkGoldenRod;
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
        <img src="" id="photo">
        <div id="joy1Div"></div>
        <div id="joy1DivInfo">
            <label for="joy1X">X1</label>
            <input id="joy1X" type="text" disabled /></br>
            <label for="joy1Y">Y1</label>
            <input id="joy1Y" type="text" disabled />
        </div>
        <div id="slidercontainer">
            <div>
                <span>F: </span>
                <input class="slider" type="range" min="0" max="100" value="0"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '%'; try{fetch(document.location.origin+'/aux?led='+this.value);}catch(e){}">
                <span class="tooltiptext">0%</span>
            </div>
            <div>
                <span>H: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '°'; try{fetch(document.location.origin+'/aux?x='+this.value);}catch(e){}">
                <span class="tooltiptext">90°</span>
            </div>
            <div>
                <span>V: </span>
                <input class="slider" type="range" min="0" max="180" value="90"
                    oninput="this.parentNode.querySelector('.tooltiptext').innerHTML = this.value + '°'; try{fetch(document.location.origin+'/aux?y='+this.value);}catch(e){}">
                <span class="tooltiptext">90°</span>
            </div>
        </div>
    </div>
    <script src="joy.js"></script>
    <script>
        var Joy1 = new JoyStick('joy1Div', { "title": "joystick1", "autoReturnToCenter": true, internalFillColor: "DarkGoldenRod", internalStrokeColor: "black", externalStrokeColor: "DarkGoldenRod" });
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
