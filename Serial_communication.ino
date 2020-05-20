// The home appliances device like FAN or TubeLight using the Arduino uno and wifi-module with serial communication.
//#include "DHT.h"
#include<SoftwareSerial.h>
//#define dht_apin 2 // Analog Pin sensor is connected to
 SoftwareSerial esp (2,3);
//DHT dht(dht_apin, DHT11);
String read_data;
void setup(){
 
  Serial.begin(9600);
  esp.begin(9600);
  delay(500);//Delay to let system boot
  pinMode(8,1);
  pinMode(9,1);
  Serial.println("DHT11 Humidity & temperature Sensor\n\n");
  delay(1000);//Wait before accessing Sensor
 
}//end "setup()"
 
void loop(){
  //Start of Program 
    if(esp.available())
    {
      read_data = esp.readString();
      Serial.println(read_data);
    }
  //   delay(10);
    read_data.trim();
    if(read_data == "ON")
    {
      digitalWrite(8,1);
    //  digitalWrite(9,0);
      Serial.println("Light1 switching ON..");
    //  delay(10);
    }
    if(read_data == "OFF")
    {
      digitalWrite(8,0);
    //  digitalWrite(9,1);
      Serial.println("Light1 switching OFF..");
    //   delay(10);
    }
    if(read_data == "ON THE FAN")
    {
      digitalWrite(9,1);
    //  digitalWrite(9,0);
      Serial.println("Fan switching ON..");
    //  delay(10);
    }
    if(read_data == "OFF THE FAN")
    {
      digitalWrite(9,0);
    //  digitalWrite(9,1);
      Serial.println("Fan switching OFF..");
    //   delay(10);
    }
//    delay(500);
 
 
}// end loop() 
