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
    var compare = ["=", "<", ">"];

    function initElements() {
      var html = '';
      //
      var sensors_div = document.getElementById("sensors");
      html = '<div>SENSORS:</div>';
      //
      sensors.forEach(function(sensor_name, sensor_id, arr) {
        html += 
          '<div>' + 
          sensor_name + ':&nbsp;<span id="' + sensor_name + '_value">0</span>' +
          '<input id="' + sensor_name + '_alert_check" type="checkbox" name="alert_check">' +          
          '<select id="' + sensor_name + '_alert_compare" class="alert_compare" name="' + sensor_name + '_alert_compare" size="1">';          
        compare.forEach(function(compare_name, compare_id, arr) {
          html += '<option value=' + (compare_id + 1) + '>' + compare_name + '</option>';
        });
        html += '</select>';
        html += '<input id="' + sensor_name + '_alert_value" type="number" class="alert_value" value="5">';
        html += '</div>';          
      });
      sensors_div.innerHTML = html;
      //
      var devices_div = document.getElementById("devices");
      html = '<div>DEVICES:</div>';
      //
      devices.forEach(function(device_name, device_id, arr) {
        html += '<div><input type="checkbox" id="' + device_name +  '_switch" onchange="setDevices()"> ' + device_name + '</div>';
      });
      devices_div.innerHTML = html;
    }

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
<body onload="initElements()">
  <div class="outer">
    <div class="inner">      
      <div class="sensors" id="sensors"></div>
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
      <div class="devices" id="devices"></div>
    </div>
  </div>
</div>
</body>
</html>
)=";

#endif
