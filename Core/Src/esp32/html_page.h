#ifndef HTML_PAGE_H
#define HTML_PAGE_H

#include "utils.h"

const char main_page[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
  <style>
    
    html {
      font-family: Tahoma;
      font-size: 14pt;
      font-weight: 100;
    }

    .outer { width:100% }

    .inner {
      display: table;
      margin: 0 auto;
      text-align: center;
    }

    .input_value {      
      width: 50px;
      position: relative;      
      top: -2px;
    }

    .check_mode {
      margin-right: 10px;
    }

    table {
      border-collapse: collapse; 
    }

    td {           
      padding: 10px;      
      padding-top: 4px;
      padding-bottom: 4px;      
      text-align: center;
      vertical-align: middle;
    }

    button {
      width: 150px;
      position: relative;
      top: -2px;
    }
    
    .alert_compare {
      width: 50px;
      text-align: center;
      height: 22px;
    }

    input {
      margin: 0;
    }    

    .mode_table td {      
      text-align: left;
    }

    .mode_table tr:first-child td {      
      text-align: center;
    }

    .mode_table tr:last-child td {      
      text-align: center;
    }
    
    .sensors_table td:nth-child(3),
    .sensors_table td:nth-child(4) {
      width: 140px;
    }    

    .table_header {
      background: #c5c3c3;
      font-weight: bold;
    }

    .data_table {
      width: 500px;
    }

    .data_table td {      
      border: 1px solid #c0c0c0;
    }

    select {      
      position: relative;
      top: -2px;
      margin-left: 6px;
      margin-right: 6px;
    }

    .devices_table td {
      width: 150px;
    }

    .devices_table td:nth-child(2), 
    .devices_table td:nth-child(3) {
      width: 140px;
    }    

    .device_switch {
      position: relative;
      top: 2px;
      width: 20px;
      height: 20px;
    }

    .column_header,
    .table_footer {
      background: #e7e7e7;
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
      var sensors_td = document.getElementById("sensors_td");
      //
      html = 
          '<table class="data_table sensors_table">' +
            '<tr>' +
              '<td colspan="4" class="table_header">SENSORS</td>' +
            '</tr>' +
            '<tr class="column_header">' +
              '<td rowspan="2">Name</td>' +
              '<td rowspan="2">Value</td>' +
              '<td colspan="2">Alert conditions</td>' +
            '</tr>' +
            '<tr class="column_header">' +
              '<td>current</td>' +
              '<td>set new</td>' +
            '</tr>';
      //
      sensors.forEach(function(sensor_name, sensor_id, arr) {
        html +=
            '<tr>' +
              '<td>' + sensor_name[0].toUpperCase() + sensor_name.slice(1) + '</td>' +
              '<td id="' + sensor_name + '_current_value">?</td>' +
              '<td id="' + sensor_name + '_current_alert">?</td>' +
              '<td>' +
                '<input id="' + sensor_name + '_alert_check" type="checkbox" class="alert_check">' +
                '<select id="' + sensor_name + '_alert_compare" class="alert_compare" size="1">';
        compare.forEach(function(compare_name, compare_id, arr) {
          html += '<option value=' + (compare_id + 1) + '>' + compare_name + '</option>';
        });
        //                 
        html +=                
                '</select>' +
                '<input id="' + sensor_name + '_alert_value" type="number" class="input_value" value="5">' +
              '</td>' +
            '</tr>';  
      });
      //
      html += 
            '<tr class="table_footer">' +
              '<td colspan="4">' +
                '<button onclick="setAlerts()">SET SETTINGS</button>' +
              '</td>' +
            '</tr>' +
          '</table>';
      //    
      sensors_td.innerHTML = html;
      //
      //
      var devices_td = document.getElementById("devices_td");
      //
      html = 
        '<table class="data_table devices_table">' + 
        '<tr>' +
          '<td colspan="3" class="table_header">DEVICES</td>' +
        '</tr>' +
        '<tr class="column_header">' +
          '<td rowspan="2">Name</td>' +
          '<td colspan="2">State</td>' +
        '</tr>' +
        '<tr class="column_header">' +
          '<td>current</td>' +
          '<td>set new</td>' +
        '</tr>';

      devices.forEach(function(device_name, device_id, arr) {
        html += 
          '<tr>' +
            '<td>' + device_name[0].toUpperCase() + device_name.slice(1) + '</td>' +
            '<td id="' + device_name + '_current_state">?</td>' +
            '<td>' +
              '<input type="checkbox" class="device_switch" id="' + device_name + '_switch" onchange="setDevices()">' + 
            '</td>' +
          '</tr>';
      });
      //
      html += '</table>';
      //
      devices_td.innerHTML = html;      
    }

    function handleStatus(status) {
      console.log("status.sensors=" + status.sensors);
      if (status.sensors != undefined)
        for (let i = 0; i < status.sensors.length; i++) {
          sensor_name = status.sensors[i].name;
          sensor_value = status.sensors[i].data.value;
          console.log(sensor_name);
          document.getElementById(sensor_name + "_current_value").innerText = sensor_value;    
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
      <table class="outer_table">
        <tr><td>
          <table class="data_table mode_table">
            <tr>
              <td colspan="2" class="table_header">CHECK MODE</td>
            </tr>
            <tr>
              <td colspan="2">current:&nbsp;<span id="current_mode">(current mode description)</span></td>
            </tr>
            <tr>
              <td><input name="check_mode" class="check_mode" type="radio" value=1 checked>with a frequency of
              <td><input id="check_period" type="number" class="input_value" min="1" max="999" size="40" value="5"> seconds
            </tr>  
            <tr>
              <td><input name="check_mode" class="check_mode" type="radio" value=2>with a change of
              <td><input id="check_percents" type="number" class="input_value" min="1" max="999" size="40" value="5"> percents
            </tr>
            <tr>
              <td><input name="check_mode" class="check_mode" type="radio" value=3>on command</td>
              <td><button id="check_button" onclick="getStatus()">CHECK SENSORS</button></td>
            </tr>        
            <tr class="table_footer">
              <td colspan="2">
                <button onclick="setMode()">SET MODE</button>
              </td>
            </tr>
          </table>
        </td></tr>
        <tr><td id="sensors_td"></td></tr>
        <tr><td id="devices_td"></td></tr>
      </table>      
    </div>
  </div>
</div>
</body>
</html>
)=";

#endif
