// Following Code used for connecting to io.Adafruit.com server through your own wifi connection and publishing the temperature and humidity level fetch from DHT 11 sensor.
// And also used for fetching the data from Adafruit server using Adafruit_MQTT_Subscribe varible to control the home appliances device like FAN or TubeLight using the Arduino uno and wifi-module
// Libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

// DHT 11 sensor
#define DHTPIN 2
#define DHTTYPE DHT11

 int count1 = 0, count2 = 0 ,count =0;
 String mode;

// WiFi parameters
#define WLAN_SSID       "__________" //put your wifi username
#define WLAN_PASS       "##########" //put your wifi passward

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "#########" //put your username
#define AIO_KEY         "1b0495cf5d704d7f8ad16316ca774ee4"  // Obtained from account info on io.adafruit.com

// DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup a feed called 'lamp' for subscribing to changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char LAMP_FEED1[] PROGMEM = AIO_USERNAME "/feeds/Switch1";
Adafruit_MQTT_Subscribe lamp1 = Adafruit_MQTT_Subscribe(&mqtt, LAMP_FEED1);

const char LAMP_FEED2[] PROGMEM = AIO_USERNAME "/feeds/Switch2";
Adafruit_MQTT_Subscribe lamp2= Adafruit_MQTT_Subscribe(&mqtt, LAMP_FEED2);

// Setup feeds for temperature & humidity
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish Switch1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Switch1");
Adafruit_MQTT_Publish Switch2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Switch2");
Adafruit_MQTT_Publish Temp_Graph = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp_graph");

/*************************** Sketch Code ************************************/

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}


void setup() {

 // Set lamp pin to output
 // pinMode(lamp_pin, OUTPUT);
  
  // Init sensor
  dht.begin();

  Serial.begin(9600);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.

 
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

// listen for events on the lamp feed
  mqtt.subscribe(&lamp1);
  mqtt.subscribe(&lamp2);

 
  // connect to adafruit io
  connect();

}

void loop() {

  Adafruit_MQTT_Subscribe *subscription;
  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }
 delay(100);
  // Grab the current state of the sensor
  float humidity_data = dht.readHumidity();
  float temperature_data = dht.readTemperature();

  // By default, the temperature report is in Celsius, for Fahrenheit uncomment
   //   following line.
 //  temperature_data = temperature_data*(9.0/5.0) + 32.0;
if(count>10 || count==0){
  // Publish data
  count=0;
  if (! temperature.publish(temperature_data))
    Serial.println(F("Failed to publish temperature"));
  else
    Serial.println(F("Temperature published!"));

  if (! humidity.publish(humidity_data))
    Serial.println(F("Failed to publish humidity"));
   else
    Serial.println(F("Humidity published!"));

  // Repeat every 10 seconds
  delay(100);
//..........................publish data to graph..............

if (! Temp_Graph.publish(temperature_data))
    Serial.println(F("Failed to publish temperature"));
  else
    Serial.println(F("Temperature published!"));


  if(temperature_data>=30.00 && temperature_data<=33.00 && humidity_data>26.00  && count1 == 0)
  { 
    if (! Switch1.publish("on"))
    Serial.println(F("Failed to publish Switch1 respose"));
  else
    Serial.println(F("Switch1 respose published!"));
    count1++;

    if (! Switch2.publish("OFF"))
    Serial.println(F("Failed to publish Switch2 respose"));
  else
    Serial.println(F("Switch2 respose published!"));
    count2--;
  }

delay(100);  
////.........................publish second switch

  if(temperature_data>=33.00 && temperature_data<=40.00 && humidity_data>26.00  && count2==0)
  {
    if (! Switch2.publish("ON"))
    Serial.println(F("Failed to publish Switch2 respose"));
  else
    Serial.println(F("Switch2 respose published!"));
    count2++;

     if (! Switch1.publish("off"))
    Serial.println(F("Failed to publish Switch1 respose"));
  else
    Serial.println(F("Switch1 respose published!"));
    count1--;
  }

} 
  delay(1000);
  count++;
  // this is our 'wait for incoming subscription packets' busy subloop
  while (subscription = mqtt.readSubscription(1000)) {

    // we only care about the lamp events
    if (subscription == &lamp1) {

      // convert mqtt ascii payload to int
      char *value = (char *)lamp1.lastread;
   //   Serial.print(F("Received: "));
   //   Serial.println(value);

      // Apply message to lamp
      String message = String(value);
      message.trim();
      if (message == "on the fan" || message == "on") {Serial.println(F("ON"));}
      if (message == "off the fan" || message == "off") {Serial.println(F("OFF"));}
      delay(100);
    }

    if (subscription == &lamp2) {

      // convert mqtt ascii payload to int
      char *value = (char *)lamp2.lastread;
   //   Serial.print(F("Received: "));
   //   Serial.println(value);

      // Apply message to lamp
      String message = String(value);
      message.trim();
      if (message == "ON") {Serial.println(F("ON THE FAN"));}
      if (message == "OFF") {Serial.println(F("OFF THE FAN"));}
      delay(100);
    }

  }
// delay(2000);
}


