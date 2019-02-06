#include <IFTTTMaker.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <wifi_node.h>
const int gasPin = A0; //GAS sensor output pin to Arduino analog A0 pin
const int READ_num=30, Delay_measure=10000;  
char temp[8],humidity[8], gas[8];
float temp_array[READ_num],humidity_array[READ_num],gas_array[READ_num], temp_avg, humidity_avg, gas_max;

int counter_loop=0;
void setup() {

// Initialize debug serial COM.
  Serial.begin(115200);
 
// connect to wifi.
  init_wifi(0);

// Initialize sensor.
  init_dht_sens(0);

}

void loop() {
  temp_array[counter_loop]=read_temp(0);
  temp_avg+=temp_array[counter_loop];
  
  humidity_array[counter_loop]=read_humidity(0);
  humidity_avg+=humidity_array[counter_loop];
 
  gas_array[counter_loop] = calc_ppm_gas(0,analogRead(gasPin)); //Read analog values of sensor
  if (gas_array[counter_loop]>= gas_max){
    gas_max = gas_array[counter_loop];
  }

  if (counter_loop==(READ_num-1)){
    counter_loop = 0;
    temp_avg /= READ_num;
    humidity_avg /= READ_num;

    Serial.println(temp_avg);
    Serial.println(humidity_avg);
    Serial.println(gas_max);
    //convert temp, humidity and gas to array of char
    dtostrf(temp_avg,5,2,temp);
    dtostrf(humidity_avg,5,2,humidity);
    dtostrf(gas_max,5,2,gas); 
    if (gas_max>= gas_alarm_th){
       post_on_IFTTT(1,gas_alarm,temp,Node_sensor_ID,gas);
    }
    //send array of char to IFTTT
    post_on_IFTTT(1,Node_sensor_ID,temp,humidity,gas);
    temp_avg=0;
    humidity_avg=0;
    gas_max=0;
  }
  else {
    counter_loop +=1;
  }
  // Delay between measurements.
  delay(Delay_measure);
}





  
