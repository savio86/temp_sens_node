#define WIFISSID "YOUR_WIFI_SSID";
#define PASS "YOUR_WIFI_PASSWOR";
#define KEY "YOUR_WEBHOOK_IFTTT_KEY"  // Get it from this page https://ifttt.com/services/maker/settings
#define Node_sensor_ID "TRIGGER_KEY_FOR_IFTTT_APPLET" // Name of your sensor ID, set when you are creating the applet
#define gas_sens_on 0 	//if the gas sensor is not present put 0, instead if the gas sens is present put 1
#define gas_alarm_th 15
#define DHTPIN     2         // Pin connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClientSecure client;
IFTTTMaker ifttt(KEY, client);
char ssid[] = WIFISSID;       // your network SSID (name)
char password[] = PASS;  // your network key
sensors_event_t event;
float m = -0.318; //Slope
float b = 1.133; //Y-Intercept
float R0 = 11.820; //Sensor Resistance in fresh air
float sensor_volt; //Define variable for sensor voltage
float RS_gas; //Define variable for sensor resistance
float ratio; //Define variable for ratio
float sensorValue,ppm_log,ppm,percentage;

void init_dht_sens(int verbose){
	
  dht.begin();
  if (verbose)
	Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  if (verbose){
	  Serial.println(F("------------------------------------"));
	  Serial.println(F("Temperature Sensor"));
	  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
	  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
	  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
	  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
	  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
	  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
	  Serial.println(F("------------------------------------"));
  }
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  if (verbose){
	  Serial.println(F("Humidity Sensor"));
	  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
	  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
	  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
	  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
	  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
	  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
	  Serial.println(F("------------------------------------"));
  }
}

void init_wifi(int verbose){
	// Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  if (verbose){
	Serial.print("Connecting Wifi: ");
	Serial.println(ssid);
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
	if (verbose){
		Serial.print(".");
	}
    delay(500);
  }
  if (verbose){
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
  }
  IPAddress ip = WiFi.localIP();
    if (verbose)
		Serial.println(ip);
}

float read_temp (int verbose){
// Get temperature event and print its value.
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
	if (verbose)
		Serial.println(F("Error reading temperature!"));
		return (-10);
  }
  else {
	if (verbose){
		Serial.print(F("Temperature: "));
		Serial.print(event.temperature);
		Serial.println(F("°C"));
	}
	return event.temperature;
  }
	
}

float read_humidity (int verbose){
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
	if (verbose)
		Serial.println(F("Error reading humidity!"));
		return (-10);
  }
  else {
	if (verbose){
		Serial.print(F("Humidity: "));
		Serial.print(event.relative_humidity);
		Serial.println(F("%"));
	}
	return event.relative_humidity;
  }
}

float calc_ppm_gas(int verbose,float sensorValue){
	sensor_volt = sensorValue * (3.3 / 1023.0); //Convert analog values to voltage
	RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; //Get value of RS in a gas
	ratio = RS_gas / R0;   // Get ratio RS_gas/RS_air
	ppm_log = (log10(ratio) - b) / m; //Get ppm value in linear scale according to the the ratio value
	ppm = pow(10, ppm_log); //Convert ppm value to log scale
	if (verbose){
		Serial.print("Gas ppm : ");
		Serial.println(ppm);
	}
	return ppm;
	
}
int post_on_IFTTT (int verbose,char ID[], char field_1[],char field_2[],char field_3[]){
	//triggerEvent takes an Event Name and then you can optional pass in up to 3 extra Strings
	if (gas_sens_on){// if the gas sensors is present, then send also the field 3
		 if(ifttt.triggerEvent(ID, field_1, field_2, field_3)){
			if (verbose)
				Serial.println("Successfully sent");
			return 0;
		 } else
		 {
				if (verbose)
					Serial.println("Failed!");
				return -1;
		 }
	}
	else // if the gas sensors is not present, then doesnt send the field 3
	{
			if(ifttt.triggerEvent(ID, field_1, field_2)){
			if (verbose)
				Serial.println("Successfully sent");
			return 0;
		 } else
		 {
				if (verbose)
					Serial.println("Failed!");
				return -1;
		 }
	}
		
}
