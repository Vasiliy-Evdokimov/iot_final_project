#include "utils.hpp"
#include "mqtt_module.hpp"
#include "uart_module.hpp"
#include "web_module.hpp"

void setup() 
{
  uart_init();
  //
  wifi_init();
  mqtt_init();
  web_init();
  periphery_init();
  mqtt_publish_all();
  //
  delay(500);
  con_println("Setup!");
  //
  uart_complete_status();
}

void loop()
{
  uart_handle();
  mqtt_handle();
  web_handle();  
  //
  delay(1);  
}
