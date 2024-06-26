#ifndef HTML_PAGE_H
#define HTML_PAGE_H

const char main_page[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
  <style>
    
    html, input, select, option, button {
      font-family: Tahoma;
      font-size: 13pt;
      font-weight: 100;
    }

    .outer { width: 100% }

    .inner {
      display: table;
      margin: 0 auto;
      text-align: center;
    }

    .input_value {
      width: 50px;
      position: relative;      
    }

    .check_mode {
      margin-right: 10px;
    }

    table {
      border-collapse: collapse;
    }

    td {
      padding: 10px;
      padding-top: 3px;
      padding-bottom: 3px;
      text-align: center;
      vertical-align: middle;
    }

    .btn_set {
      width: 150px;
      position: relative;
      top: 0px;
    }
    
    .alert_compare {
      width: 40px;
      text-align: center;
      height: 27px;
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
      position: relative;
      background: #afcde7;
      font-weight: bold;
    }

    .data_table {
      width: 500px;      
      border: 1px solid #c0c0c0;
    }    

    select {
      position: relative;            
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

    .current_value {
      font-weight: bold;
    }

    .device_switch {
      position: relative;
      top: 2px;
      width: 20px;
      height: 20px;
    }

    .column_header,
    .table_footer {
      background: #d8e6f3;
    }

    .table_header,
    .column_header td,
    .table_footer td {
      border: 1px solid #c0c0c0;
    }

    /*  ==========  */    

    .plc_table td {
      text-align: center;
    }

    .plc_outputs_table {
      width: 100%;
    }

    .plc_outputs_table td {
      padding: 0px;
      padding-left: 10px;
      border: 0px solid white;
    }  

    .plc_inputs_table_td table {
      width: 100%;
    }

    .plc_inputs_table_td td {
      padding: 0px;
      margin: 0px;
      border: none;
    }

    .plc_inputs_table {      
      width: 100%;
    }

    .plc_inputs_table td {
      width: 12%
    }

    .plc_led_out {
      position: relative;      
    }    

    .plc_led {
      position: absolute;
      top: -10px;
      left: 0px;
      width: 20px;
      height: 20px;
      background: gray;
    }

    .plc_led_bg {
      position: absolute;
      top: -11px;
      left: 0px;
      width: 22px;
      height: 22px;
      background: black;
    }

    .plc_output_led {
      left: -5px;
      border-radius: 50%;
    }

    .plc_output_led_bg {
      left: -6px;
      border-radius: 50%;
    }

    .plc_input_led {      
      left: 6px;
    }

    .plc_input_led_bg {      
      left: 5px;
    }

    .plc_mask_check {
      margin-left: 5px;
    }    

    .plc_inputs_table_title {
      width: 200px;
    }

    .outer_table_td {
      padding: 0px;
      margin: 0px;      
      vertical-align: top;
    }

    .data_td {
      padding: 5px;
    }

    #mqtt_td {
      padding: 0px;
    }

    .mqtt_td_table {
      width: 100%;
    }

    .mqtt_td_table  td:nth-child(1) {
      text-align: left;      
    }

    .mqtt_path_td {
      width: 68%;
    }

    .mqtt_value_td {
      width: 17%;      
    }

    #new_mqtt_path {      
      width: 98%;
    }

    .data_row:nth-child(odd) {
      background-color: #e9f3f3;
    }

    .data_row:nth-child(even) {
      background-color: #e6dfdf;
    }

    #check_button {
      position: relative;
      top: 0px;
    }

    .mqtt_btn {
      width: 50px;    
    }

    .publish_check {
      font-weight: normal;
      position: absolute;
    }

  </style>

  <script>
    
    setInterval(function() { getStatus(); }, 500);
    
    var mode_descriptions = ["with a frequency of", "with a change of", "on command"];
    var sensors = ["temperature", "humidity", "ambient"];
    var devices = ["led_red", "led_blue"];
    var compare = ["=", "<", ">"];

    var publish_check_map = new Map();
    publish_check_map.set("publish_modes", false);
    publish_check_map.set("publish_sensors", true);
    publish_check_map.set("publish_alerts", false);
    publish_check_map.set("publish_devices", false);
    publish_check_map.set("publish_plc", true);

    var publish_check_title = "&nbsp;pub";

    var PLC_INPUTS_COUNT = 4;
    var PLC_OUTPUTS_COUNT = 4;

    function initElements() 
    {
      document.getElementById("modes_publish_check_title").innerHTML = publish_check_title;
      //
      var html = '';
      //
      var sensors_td = document.getElementById("sensors_td");
      //
      html =
          '<table class="data_table sensors_table">' +
            '<tr>' +
              '<td colspan="4" class="table_header">' + 
                '<div class="publish_check"><input id="publish_sensors" type="checkbox" onchange="setPublishFlags()">' + publish_check_title + '</div>' +
                'SENSORS' + 
              '</td>' +
            '</tr>' +
            '<tr class="column_header">' +
              '<td rowspan="2">Name</td>' +
              '<td rowspan="2">Value</td>' +
              '<td colspan="2" style="position: relative">' +
                '<div class="publish_check"><input id="publish_alerts" type="checkbox" onchange="setPublishFlags()">' + publish_check_title + '</div>' +
                'Alert conditions' + 
              '</td>' +
            '</tr>' +
            '<tr class="column_header">' +
              '<td>current</td>' +
              '<td>set new</td>' +
            '</tr>';
      //
      sensors.forEach(function(sensor_name, sensor_id, arr) {
        html +=
            '<tr class="data_row">' +
              '<td>' + sensor_name[0].toUpperCase() + sensor_name.slice(1) + '</td>' +
              '<td id="' + sensor_name + '_current_value" class="current_value">?</td>' +
              '<td id="' + sensor_name + '_current_alert" class="current_value">?</td>' +
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
                '<button class="btn_set" onclick="setAlerts()">SET ALERTS</button>' +
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
          '<td colspan="3" class="table_header">' + 
            '<div class="publish_check"><input id="publish_devices" type="checkbox" onchange="setPublishFlags()">' + publish_check_title + '</div>' +
            'DEVICES' + 
          '</td>' +
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
          '<tr class="data_row">' +
            '<td>' + device_name[0].toUpperCase() + device_name.slice(1) + '</td>' +
            '<td id="' + device_name + '_current_state" class="current_value">?</td>' +
            '<td>' +
              '<input type="checkbox" class="device_switch" id="' + device_name + '_switch">' +
            '</td>' +
          '</tr>';
      });
      //
      html += 
            '<tr class="table_footer">' +
              '<td colspan="3">' +
                '<button class="btn_set" onclick="setDevices()">SET STATES</button>' +
              '</td>' +
            '</tr>' +
          '</table>';
      //
      devices_td.innerHTML = html;
      //
      //
      var plc_td = document.getElementById("plc_td");
      //
      html =
        '<table class="data_table plc_table">' +
          '<tr>' +
            '<td colspan="5" class="table_header">' + 
              '<div class="publish_check"><input id="publish_plc" type="checkbox" onchange="setPublishFlags()">' + publish_check_title + '</div>' +
              'PLC MASKS' + 
            '</td>' +
          '</tr>' +
          '<tr class="column_header">' +
            '<td rowspan="2">Outputs</td>' +
            '<td colspan="2">Current</td>' +
            '<td colspan="2">Set new</td>' +
          '</tr>' +
          '<tr class="column_header">' +
            '<td>mask</td>' +
            '<td>all bits</td>' +
            '<td>mask</td>' +
            '<td>all bits</td>' +
          '</tr>';
      //
      for (let i = 0; i < PLC_OUTPUTS_COUNT; i++) {
        html +=
          '<tr class="data_row plc_mask_row">' +
              '<td>' + 
                '<table class="plc_outputs_table">' + 
                  '<tr>' + 
                    '<td width=50%>' + (i + 1) + '</td>' + 
                    '<td width=50%>' + 
                      '<div class="plc_led_out">' +
                        '<div class="plc_led_bg plc_output_led_bg"></div>' +
                        '<div id="plc_output_' + (i + 1) + '" class="plc_led plc_output_led"></div>' + 
                      '</div>' +     
                    '</td>' + 
                  '</tr>' + 
                '</table>' + 
              '</td>' +
              '<td>';
        for (let j = 0; j < PLC_INPUTS_COUNT; j++)
          html +=
            '<input id="plc_output_' + (i + 1) + '_current_mask_' + (j + 1) + '" type="checkbox" class="plc_mask_check" onclick="return false;">';
        html += '</td>';
        //
        html += 
          '<td>' +
            '<input id="plc_output_' + (i + 1) + '_current_mask_all_bits" type="checkbox" class="plc_mask_check" onclick="return false;">' +
          '</td>';
        //      
        html += '<td>';
        for (let j = 0; j < PLC_INPUTS_COUNT; j++)
          html +=              
            '<input id="plc_output_' + (i + 1) + '_new_mask_' + (j + 1) + '" type="checkbox" class="plc_mask_check">';
        html += '</td>';                
        //    
        html +=
          '<td>' +
            '<input id="plc_output_' + (i + 1) + '_new_mask_all_bits" type="checkbox" class="plc_mask_check">' +
          '</td>';
          html += '</tr>';
      }  
      //
      html +=
        '<tr class="data_row">' +
          '<td colspan="5" class="plc_inputs_table_td">' +
            '<table>' +
              '<tr>' +
                '<td class="plc_inputs_table_title">Current inputs:</td>' + 
                //
                '<td>' + 
                  '<table class="plc_inputs_table">' + 
                    '<tr>';
                  for (let j = 0; j < PLC_INPUTS_COUNT; j++)
                    html +=
                      '<td>' + (j + 1) + '</td>' + 
                      '<td>' + 
                        '<div class="plc_led_out">' +
                          '<div class="plc_led_bg plc_input_led_bg"></div>' +
                          '<div id="plc_input_' + (j + 1) + '" class="plc_led plc_input_led"></div>' +
                        '</div>' +
                      '</td>';
            html += '</tr>' + 
                  '</table>' + 
                '</td>';

      html +=                     
              '</tr>' +
            '</table>' +
          '</td>' +
        '</tr>';      
      //
      html +=
        '<tr class="table_footer">' +
          '<td colspan="5">' +
            '<button class="btn_set" onclick="setPlcMasks()">SET MASKS</button>' +
          '</td>' +
        '</tr>' +
      '</table>';
      //
      plc_td.innerHTML = html;
      //
      //
      for (let key of publish_check_map.keys())
        document.getElementById(key).checked = publish_check_map.get(key);             
    }

    function handleStatus(status)
    {
      if (status.mode != undefined) {
        let mode = mode_descriptions[status.mode.type - 1];
        if (status.mode.type == 1)
          mode += " " + status.mode.period + " seconds";
        if (status.mode.type == 2)
          mode += " " + status.mode.percent + " percents";
        document.getElementById("current_mode").innerText = mode;
      }
      //
      if (status.sensors != undefined)
        for (let i = 0; i < status.sensors.length; i++) {
          sensor_name = status.sensors[i].name;
          sensor_value = status.sensors[i].data.value;
          let sensor_value_obj = document.getElementById(sensor_name + "_current_value");
          sensor_value_obj.innerText = sensor_value;
          sensor_value_obj.style.color = (status.sensors[i].data.alert_flag == 1) ? "red" : "black";
          //
          let alert_value = "?";
          if (status.sensors[i].data.alert_check == 0)
            alert_value = "not checked";
          else
            alert_value =
              compare[status.sensors[i].data.alert_compare - 1] + " " +
              status.sensors[i].data.alert_value;
          document.getElementById(sensor_name + "_current_alert").innerText = alert_value;
        }
      //
      if (status.devices != undefined)
        for (let i = 0; i < status.devices.length; i++) {
          device_name = status.devices[i].name;
          device_value = (status.devices[i].value == 0) ? "OFF" : "ON";
          let device_obj = document.getElementById(device_name + "_current_state");
          device_obj.style.color = (status.devices[i].value == 0) ? "gray" : "limegreen";
          device_obj.innerText = device_value;
        }
      //
      //
      if (status.plc_outputs_masks != undefined)
        for (let i = 0; i < status.plc_outputs_masks.length; i++) {
          index = status.plc_outputs_masks[i].index;
          mask = status.plc_outputs_masks[i].mask;
          all_bits = status.plc_outputs_masks[i].all_bits;
          //
          for (let j = 0; j < PLC_INPUTS_COUNT; j++) {
            let mask_obj = document.getElementById("plc_output_" + (index + 1) + "_current_mask_" + (j + 1));
            if (!mask_obj) continue;
            mask_obj.checked = ((mask & (1 << j)) > 0);
          }            
          let all_bits_obj = document.getElementById("plc_output_" + (index + 1) + "_current_mask_all_bits");
          if (!all_bits_obj) continue;
          all_bits_obj.checked = (all_bits > 0);
        }
      //
      if (status.plc_inputs_states != undefined) {
        let plc_inputs_states = status.plc_inputs_states;
        for (let i = 0; i < PLC_INPUTS_COUNT; i++) {
          let plc_input_obj = document.getElementById("plc_input_" + (i + 1));
          if (!plc_input_obj) continue;
          plc_input_obj.style.background = ((plc_inputs_states & (1 << i)) > 0) ? "lime" : "gray";
        }
      }
      //
      if (status.plc_outputs_states != undefined) {
        let plc_outputs_states = status.plc_outputs_states;
        for (let i = 0; i < PLC_OUTPUTS_COUNT; i++) {
          let plc_output_obj = document.getElementById("plc_output_" + (i + 1));
          if (!plc_output_obj) continue;
          plc_output_obj.style.background = ((plc_outputs_states & (1 << i)) > 0) ? "yellow" : "gray";
        }
      }
      //
      //
      if (status.mqtt_subscribe_topics != undefined)
      {
        var mqtt_td = document.getElementById("mqtt_td");
        var mst = status.mqtt_subscribe_topics;
        //
        html = '<table class="mqtt_td_table">';
        for(let i = 0; i < mst.length; i++)
          if (mst[i].path != "")
            html +=
              '<tr class="data_row" id="mqtt_path_' + i + '">' +
                '<td class="mqtt_path_td">' + mst[i].path + '</td>' +
                '<td class="mqtt_value_td current_value">' + mst[i].value + '</td>' +
                '<td>' +
                  '<button class="mqtt_btn" onclick="delMqttPath(' + i + ')">DEL</button>' +
                '</td>' +
              '</tr>';
        html += "<table>";
        //
        mqtt_td.innerHTML = html;
      }
    }

    function getStatus()
    {
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

    function setAlerts()
    {      
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
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function setMode()
    {
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
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function setDevices()
    {
      var q = "setDevices?";
      devices.forEach(function(item, i, arr) {
        let chk = item + "_switch";
        let chk_obj = document.getElementById(chk);
        //
        q += chk + "=" + (chk_obj.checked ? 1 : 0) + "&";
      });
      q = q.substring(0, q.length - 1);
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function setPlcMasks()
    {      
      var q = "setPlcMasks?";
      for (let i = 0; i < PLC_OUTPUTS_COUNT; i++) {
        let mask = 0;
        for (let j = 0; j < PLC_INPUTS_COUNT; j++) {        
          let plc_output_mask_obj = document.getElementById('plc_output_' + (i + 1) + '_new_mask_' + (j + 1));
          if (!plc_output_mask_obj) continue;
          mask |= ((plc_output_mask_obj.checked ? 1 : 0) << j);
        }
        q += 'plc_output_' + (i + 1) + '_mask=' + mask + "&";
        //
        let plc_output_all_bits_obj = document.getElementById('plc_output_' + (i + 1) + '_new_mask_all_bits');
        if (!plc_output_all_bits_obj) continue;
        q += 'plc_output_' + (i + 1) + '_all_bits=' + (plc_output_all_bits_obj.checked ? 1 : 0) + "&";
      }
      q = q.substring(0, q.length - 1);      
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }

    function delMqttPath(path_id)
    {      
      var q = "delMqttPath?path_id=" + path_id;
      //
      var ask = confirm("Are you sure you want to unsubscribe from the selected topic?");
      if (!ask) return;
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }  

    function addMqttPath()
    {
      var path_input = document.getElementById('new_mqtt_path'); 
      var new_path = path_input.value;
      var q = "addMqttPath?new_path=" + new_path;
      path_input.value = "";
      //
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();      
    }

    function setPublishFlags() 
    {
      var q = "setPublishFlags?";
      for (let key of publish_check_map.keys()) {
        let chk_obj = document.getElementById(key);
        q += key + "=" + (chk_obj.checked ? 1 : 0) + "&";        
      }
      q = q.substring(0, q.length - 1);
      //  alert(q);
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", q, true);
      xhttp.send();
    }
    
    function btnClick() {
      console.log("btnClick()");
    }

  </script>
  <title>V_Evdokimov's final project</title>
</head>
<body onload="initElements()">
  <div class="outer">
    <div class="inner">
      <table class="outer_table">
        <tr>
          <td class="outer_table_td">

            <table>
              <tr>
                <td class="data_td">
                  <table class="data_table mode_table">
                    <tr>
                      <td colspan="2" class="table_header">
                        <div class="publish_check"><input id="publish_modes" type="checkbox" onchange="setPublishFlags()"><span id="modes_publish_check_title"></span></div>
                        CHECK MODE
                      </td>
                    </tr>
                    <tr class="data_row">
                      <td colspan="2">current:&nbsp;<span id="current_mode">?</span></td>
                    </tr>
                    <tr class="data_row">
                      <td><input name="check_mode" class="check_mode" type="radio" value=1 checked>with a frequency of
                      <td><input id="check_period" type="number" class="input_value" min="1" max="999" size="40" value="5"> seconds
                    </tr>  
                    <tr class="data_row">
                      <td><input name="check_mode" class="check_mode" type="radio" value=2>with a change of
                      <td><input id="check_percents" type="number" class="input_value" min="1" max="999" size="40" value="5"> percents
                    </tr>
                    <tr class="data_row">
                      <td><input name="check_mode" class="check_mode" type="radio" value=3>on command</td>
                      <td><button id="check_button" onclick="getStatus()">CHECK SENSORS</button></td>
                    </tr>        
                    <tr class="table_footer">
                      <td colspan="2">
                        <button class="btn_set" onclick="setMode()">SET MODE</button>
                      </td>
                    </tr>
                  </table>
                </td>
              </tr>
              <tr><td class="data_td" id="sensors_td"></td></tr>
              <tr><td class="data_td" id="devices_td"></td></tr>
            </table>
      
          </td>
          <td class="outer_table_td">

            <table>
              <tr><td class="data_td" id="plc_td"></td></tr>
              <tr><td class="data_td">

                <table class="data_table">
                  <tr>
                    <td colspan="3" class="table_header">MQTT SUBSCRIPTIONS</td>
                  </tr>
                  <tr class="column_header">
                    <td class="mqtt_path_td">Path</td>
                    <td class="mqtt_value_td">Value</td>
                    <td>&nbsp;</td>
                  </tr>
                  <tr>
                    <td colspan="3" id="mqtt_td"></td>
                  </tr>
                  <tr class="table_footer">
                    <td colspan="2">
                      <input type="text" id="new_mqtt_path">
                    </td>
                    <td>
                      <button class="mqtt_btn" onclick="addMqttPath()">ADD</button>
                    </td>
                  </tr>
                </table>

              </td></tr>

            </table>

          </td>
        </tr>
      </table>      
    </div>
  </div>
</div>
</body>
</html>
)=";

#endif
