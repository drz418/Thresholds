
/* poplar leaves are known to turn over before a hard rain due to 
 * sudden change in humidity
 * Using an arduino MKR 1000
 */
 
#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
#include <Servo.h>

#include "arduino_secrets.h"

Servo servoleaf;  // create servo object to control a servo
Servo servoleaf2;
Servo servoleaf3;

int pos = 0;    // variable to store the servo position

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

//open weather map api key
String apiKey= SECRET_APIKEY;

//the city you want the weather for
String location= "New York,US";

int status = WL_IDLE_STATUS; // the Wifi radio's status
char server[] = "api.openweathermap.org"; 

WiFiClient client; 

String nextWeatherTime[]={" "," "," "};
String nextWeather[] ={" "," "," "};
unsigned long WeatherTimer= 0UL;
unsigned long interval = 300000UL; // 300 seconds or 5 minutes

int swayspeed = 1; // speed the leaves move at

void setup() {

  servoleaf.attach(11); // attaches the servo on pin 11,12, 13 to the servo object
  servoleaf2.attach(12);
  servoleaf3.attach(13);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid);
    //use the line below if your network is protected by wpa password 
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection:
    delay(1000);
  }
  Serial.println("Connected to wifi");
}

void loop() {

 
  // timer for when to access weather data 
   unsigned long currentMillis = millis();
    if(currentMillis - WeatherTimer > interval) {
  getWeather();
  WeatherTimer = currentMillis;
    }

//  if / else to make the servo speed change 
if (nextWeather[0]=="rain"||nextWeather[1]=="rain") {swayspeed = 2;
}
else {swayspeed = 1;
}
   
  
for (pos = 0; pos <= 180; pos += 1*swayspeed) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoleaf.write(pos);   // tell servo to go to position in variable 'pos'
    servoleaf2.write(pos);
    servoleaf3.write(pos);
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1*swayspeed) { // goes from 180 degrees to 0 degrees
   
    servoleaf.write(pos);   // tell servo to go to position in variable 'pos'
    servoleaf2.write(pos);
    servoleaf3.write(pos);      
    delay(15);                     // waits 15 ms for the servo to reach the position
    }
  }


void getWeather() {

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /data/2.5/forecast?");
    client.print("q="+location);
    client.print("&appid="+apiKey);
    client.print("&cnt=3");
    client.println("&units=metric");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("unable to connect");
  }

  delay(1000);
   String line = "";
  while (client.connected()) {
  line = client.readStringUntil('\n');
   // Serial.println(line);
    Serial.println("parsingValues");
     //create a json buffer where to store the json data 
   StaticJsonDocument<5000> doc; 
   DeserializationError error = deserializeJson(doc, line);
   //JsonObject& root = doc.parseObject(line); 
   if (error) { 
     Serial.print("deserializeJson() failed with code"); 
     Serial.println(error.c_str());
     return; 
  }
  //get the data from the json tree 
 String nextWeatherTime0 = doc["list"][0]["dt_txt"]; 
 String nextWeather0 = doc["list"][0]["weather"][0]["main"]; 

 String nextWeatherTime1 = doc["list"][1]["dt_txt"]; 
 String nextWeather1 = doc["list"][1]["weather"][0]["main"]; 
 /*
 String nextWeatherTime2 = doc["list"][2]["dt_txt"]; 
 String nextWeather2 = doc["list"][2]["weather"][0]["main"]; 
 */
 // Print values. 
 Serial.println(nextWeatherTime0); 
 Serial.println(nextWeather0); 
 Serial.println(nextWeatherTime1); 
 Serial.println(nextWeather1);   
 /*
 Serial.println(nextWeatherTime2); 
 Serial.println(nextWeather2); 
 */
  }
}
