#ifndef HTML_PAGE_H
#define HTML_PAGE_H

#include "utils.h"

const char MAIN_page[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
<style>
  
  html {
    font-size: 20pt;
    font-weight: bold;
  }
  .outer { width:100%  }
  .inner {
    display: table;
    margin: 0 auto;    
    text-align: center;
  }
  .inner > div { margin-top: 10px; }
    
</style>

<script>
  
  setInterval(function() { getData(); }, 2000);
  
  function getData() {    
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {        
        console.log(this.responseText);
        obj = JSON.parse(this.responseText);
        //
        if (obj.type == 'sensors') {
          for (let i = 0; i < obj.data.length; i++) {
            sensor_name = obj.data[i].name;
            sensor_value = obj.data[i].data.value;
            console.log(sensor_name);
            document.getElementById(sensor_name + "_value").innerText = sensor_value;    
          }
        }
      }
    };
    xhttp.open("GET", "getSensors", true);
    xhttp.send();
  }

  function btnClick() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "setMode?mode=1", true);
    xhttp.send();
  }
  
</script>
</head>
<body>
  <div class="outer">
    <div class="inner">
      <div>ESP32 Web Page</div>
      <div>Temperature:&nbsp;<span id="temperature_value">0</span><div>
      <div>Humidity:&nbsp;<span id="humidity_value">0</span><div>
      <div>Ambient:&nbsp;<span id="ambient_value">0</span><div>
      <br><button onclick="btnClick()">Test</button>
    </div>
  </div>
</div>
</body>
</html>
)=";

#endif
