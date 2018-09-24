#define BLYNK_PRINT Serial

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>
#include <Ticker.h>

//Define variables
long duration;
int distance;
int delayed = 0;
bool shouldSaveConfig = false;

//Define constants
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
const int resetPin = 15; //D8
const int ledPin = 4; //D2
const int delayTime = 5000;
char blynk_token[34] = "";
char blynk_pin[4] = "";
char tank_height[10] = "";
char tank_overflow_height[10] = "";

BlynkTimer timer;
WiFiManager wifiManager;
Ticker ticker;

void setup() {
  Serial.begin(9600);
  Serial.print("Starting up water alarm...");

// Just for testing purposes ;)
//  WiFi.disconnect(true);
//  delay(2000);
//  ESP.reset();
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(resetPin, INPUT); //Sets the resetPin as an input
  pinMode(ledPin, OUTPUT);

  //Using ticker to check connection state every 200ms without getting affected by wifi or blynk process
  ticker.attach(0.2, checkConnection);

  //Reading configuration from FS json
  Serial.println("Mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("Mounted file system");
    if (SPIFFS.exists("/config.json")) {
      Serial.println("Reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("Opened config file");
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nParsed json");
          if(json.containsKey("blynk_token") && json.containsKey("blynk_pin") && json.containsKey("tank_height") && json.containsKey("tank_overflow_height")){
             strcpy(blynk_token, json["blynk_token"]);
             strcpy(blynk_pin, json["blynk_pin"]);
             strcpy(tank_height, json["tank_height"]);
             strcpy(tank_overflow_height, json["tank_overflow_height"]);
          }
        } else {
          Serial.println("Failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("Failed to mount FS");
  }


  //Configuring custom params
  WiFiManagerParameter custom_blynk_token("blynk", "Blynk Token", blynk_token, 34);
  WiFiManagerParameter custom_blynk_pin("blynk_pin", "Blynk V-Pin (Ex - 0,1...127)", blynk_pin, 4);
  WiFiManagerParameter custom_tank_height("tank_height", "Water Tank Height (In cm)", tank_height, 10);
  WiFiManagerParameter custom_tank_overflow_height("tank_overflow_height", "Water Tank Overflow Level (In cm)", tank_overflow_height, 10);

 
  //Start WiFi Manager
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //Adding custom CSS to WiFi Manager
  wifiManager.setCustomHeadElement("<style>body{background: #f7f5f5;}button{transition: 0.3s;opacity: 0.8;cursor: pointer;border:0;border-radius:1rem;background-color:#1dca79;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}button:hover {opacity: 1}button[type=\"submit\"]{margin-top: 15px;margin-bottom: 10px;font-weight: bold;text-transform: capitalize;}input{height: 30px;font-family:verdana;margin-top: 5px;background-color: rgb(253, 253, 253);border: 0px;-webkit-box-shadow: 2px 2px 5px 0px rgba(0,0,0,0.75);-moz-box-shadow: 2px 2px 5px 0px rgba(0,0,0,0.75);box-shadow: 2px 2px 5px 0px rgba(0,0,0,0.75);}div{color: #14a762;}div a{text-decoration: none;color: #14a762;}div[style*=\"text-align:left;\"]{color: black;}, div[class*=\"c\"]{border: 0px;}a[href*=\"wifi\"]{border: 2px solid #1dca79;text-decoration: none;color: #1dca79;padding: 10px 30px 10px 30px;font-family: verdana;font-weight: bolder;transition: 0.3s;border-radius: 5rem;}a[href*=\"wifi\"]:hover{background: #1dca79;color: white;}</style>");
  
  //Adding custom params
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.addParameter(&custom_blynk_pin);
  wifiManager.addParameter(&custom_tank_height);
  wifiManager.addParameter(&custom_tank_overflow_height);
  wifiManager.autoConnect("Water Level Sensor");

  //Applying custom params
  strcpy(blynk_token, custom_blynk_token.getValue());
  strcpy(blynk_pin, custom_blynk_pin.getValue());
  strcpy(tank_height, custom_tank_height.getValue());
  strcpy(tank_overflow_height, custom_tank_overflow_height.getValue());


  //Saving the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("Saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["blynk_token"] = blynk_token;
    json["blynk_pin"] = blynk_pin;
    json["tank_height"] = tank_height;
    json["tank_overflow_height"] = tank_overflow_height;
    
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("Failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }

  Blynk.config(blynk_token);

  //So, now if you are here, then you are connected to Blynk server :)
  Blynk.notify(String("{DEVICE_NAME} is up and running"));

  //Adding some timers here
  timer.setInterval(60000L, sendAlert);
  timer.setInterval(1000L, sendDistance);
  timer.setInterval(1000L, checkReset);
}

void loop() {
  Blynk.run();
  timer.run();
}

void checkReset(){
  //Reset everything if long pressed
  if(delayed == delayTime){
    wifiManager.resetSettings();
    delay(1000);
    ESP.restart();
  }

  if(digitalRead(resetPin)==HIGH){
     delayed = delayed+1000;
  }else{
     delayed = 0;
  }
}

void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void sendAlert(){
  int dif = atoi(tank_height)-atoi(tank_overflow_height);
  if(distance<=dif){
    Blynk.notify("Alert! Water tank is about to overflow");
  }
}

void checkConnection(){
  if(!Blynk.connected()){
    digitalWrite(ledPin, !digitalRead(ledPin));
  }else{
    digitalWrite(ledPin, HIGH);
  }
}

void sendDistance(){
  refreshDistance();
  int pin = atoi(blynk_pin);
  int height = atoi(tank_height);
  double variation = height-distance;
  int variation_percent = variation/height * 100;
  Serial.print("Distance : "+String(distance));
  Serial.print(", Height : "+String(height));
  Serial.println(", V-Pin : "+String(pin));
  Blynk.virtualWrite(pin, variation_percent);
}

void refreshDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
}
