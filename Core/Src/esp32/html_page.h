#ifndef HTML_PAGE_H
#define HTML_PAGE_H

#include "utils.h"

const char MAIN_page[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
  <style>
    
    html {
      font-family: Arial, Helvetica, sans-serif;
      font-size: 20pt;
      font-weight: 100;
    }
    .outer { width:100% }
    .inner {
      display: table;
      margin: 0 auto;
      text-align: center;
    }
    .inner > div {
      margin-top: 10px;
    }
    .alert_value {
      width: 50px;
    }
    .test_btn {
      width: 100px;
      height: 50px;
    }

  </style>

  <script>
    
    setInterval(function() { getStatus(); }, 2000);

    var sensors = ["temperature", "humidity", "ambient"];
    var devices = ["led", "fan"];

    function handleStatus(status) {
      console.log("status.sensors=" + status.sensors);
      if (status.sensors != undefined)
        for (let i = 0; i < status.sensors.length; i++) {
          sensor_name = status.sensors[i].name;
          sensor_value = status.sensors[i].data.value;
          console.log(sensor_name);
          document.getElementById(sensor_name + "_value").innerText = sensor_value;    
        }        
    }           

    function getStatus() {     
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = 
        function() {
          if (this.readyState == 4 && this.status == 200) {        
            console.log(this.responseText);
            let obj = JSON.parse(this.responseText);          
            handleStatus(obj);
          }
        }
      xhttp.open("GET", "getStatus", true);
      xhttp.send();
    }    

    function setAlerts() {      
      var q = "setAlerts?";
      sensors.forEach(function(item, i, arr) {
        let chk = item + "_alert_check";
        let cmp = item + "_alert_compare";
        let val = item + "_alert_value";
        let chk_obj = document.getElementById(chk);
        let cmp_obj = document.getElementById(cmp);
        let val_obj = document.getElementById(val);
        //
        q += 
          chk + "=" + (chk_obj.checked ? 1 : 0) + "&" +
          cmp + "=" + cmp_obj.value + "&" +
          val + "=" + val_obj.value + "&";
      });
      q = q.substring(0, q.length - 1);
      //alert(q);
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function setMode() {
      var q = "setMode?mode=";
      var radios = document.getElementsByName('check_mode');
      var value = 0;
      for (var i = 0; i < radios.length; i++) {
          if (radios[i].type === 'radio' && radios[i].checked) {
            value = Number(radios[i].value);
            break;
          }
      }
      if (value == 0) return;
      //
      document.getElementById('check_button').disabled = true;
      q += value;
      switch (value) {
        case 1: {
          q += "&check_period=" + document.getElementById('check_period').value;
          break;
        }         
        case 2: {
          q += "&check_percents=" + document.getElementById('check_percents').value;
          break;
        }       
        case 3: {
          document.getElementById('check_button').disabled = false;
          break;
        }       
      }
      //alert(q);
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function setDevices() {      
      var q = "setDevices?";
      devices.forEach(function(item, i, arr) {
        let chk = item + "_switch";
        let chk_obj = document.getElementById(chk);
        //
        q += chk + "=" + (chk_obj.checked ? 1 : 0) + "&";
      });
      q = q.substring(0, q.length - 1);
      //alert(q);      
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();      
    }
    
    function btnClick() {      
      console.log("btnClick()");
    }

  </script>
</head>
<body>
  <div class="outer">
    <div class="inner">      
      <div class="sensors">
        <div>SENSORS:</div>
        <div>
          Temperature:&nbsp;<span id="temperature_value">0</span>

          <input id="temperature_alert_check" type="checkbox" name="alert_check">
          <select id="temperature_alert_compare" class="alert_compre" name="temperature_alert_compare" size="1">
            <option value=1>=</option>
            <option value=2><</option>
            <option value=3>></option>
          </select>
          <input id="temperature_alert_value" type="number" class="alert_value" value="5">

        <div>
        <div>
          Humidity:&nbsp;<span id="humidity_value">0</span>

          <input id="humidity_alert_check" type="checkbox" name="alert_check">
          <select id="humidity_alert_compare" class="alert_compre" name="temperature_alert_compare" size="1">
            <option value=1>=</option>
            <option value=2><</option>
            <option value=3>></option>
          </select>
          <input id="humidity_alert_value" type="number" class="alert_value" value="5">
          
        <div>
        <div>
          Ambient:&nbsp;<span id="ambient_value">0</span>

          <input id="ambient_alert_check" type="checkbox" name="alert_check">
          <select id="ambient_alert_compare" class="alert_compre" name="temperature_alert_compare" size="1">
            <option value=1>=</option>
            <option value=2><</option>
            <option value=3>></option>
          </select>
          <input id="ambient_alert_value" type="number" class="alert_value" value="5">

        <div>              
      </div>
      <button onclick="setAlerts()">SET ALERTS</button>
      <br><br>
      <div class="settings">
        <div>CHECK MODE:</div>
        <div>
          <input name="check_mode" type="radio" value=1 checked>Check periodically
          <input id="check_period" type="number" min="1" max="999" size="40" value="5">
        </div>
        <div>
          <input name="check_mode" type="radio" value=2>Check if changed
          <input id="check_percents" type="number" min="1" max="999" size="40" value="5">
        </div>
        <div>
          <input name="check_mode" type="radio" value=3>Check by command
          <button id="check_button" onclick="getStatus()">CHECK SENSORS</button>  
        </div>
        <button onclick="setMode()">SET MODE</button>
      </div>
      <br><br>
      <div class="devices">
        <div>DEVICES:</div>
        <div><input type="checkbox" id="led_switch" onchange="setDevices()"> led</div>
        <div><input type="checkbox" id="fan_switch" onchange="setDevices()"> fan</div>
      </div>        
      <!--
      <button class="test_btn" onclick="btnClick()">TEST</button>
      -->
    </div>
  </div>
</div>
</body>
</html>
)=";

#endif
