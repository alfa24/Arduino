#include <ESP8266Ping.h>


/*
 *  This sketch shows the WiFi event usage
 *
 */

#include <ESP8266WiFi.h>

const char* ssid     = "Keenetic-6004";
const char* password = "60600000";
boolean isWiFiConnected;

//Время для перезагрузки компа (сек)
const int timeRebot = 600;

char* internet = "ya.ru";
char* router = "192.168.1.1";
int pin0 = 16;
int timeReset0 = 120;
int wifiLed = 14; //wi-fi connected

char* rig1 = "192.168.1.11";
int pin1 = 5;
int timeReset1 = 0;

char* rig2 = "192.168.1.12";
int pin2 = 4;
int timeReset2 = 0;

char* rig3 = "192.168.1.13";
int pin3 = 0;
int timeReset3 = 0;

char* rig4 = "192.168.1.14";
int pin4 = 2;
int timeReset4 = 0;

void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println();
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            digitalWrite(wifiLed, HIGH);
            isWiFiConnected = true;
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            digitalWrite(wifiLed, LOW);
            isWiFiConnected = false;
            break;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(wifiLed, OUTPUT);
    pinMode(pin0, OUTPUT);
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
    
    // delete old config
    WiFi.disconnect(true);

    delay(1000);

    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);
    Serial.println("Wait for WiFi... ");
    
}


void loop() {
    checkRouter(); 
    
    delay(5000); 
    checkRig1();        
    checkRig2();        
    checkRig3();        
    checkRig4();       
}

void checkRouter() {
  boolean work = false;
  Serial.println("");
  Serial.println("");
  Serial.print("Ping router ");
  Serial.println(": ");

  if (pingICMP(router)){
    Serial.print("Ping internet ");
    Serial.println(": ");
    work = pingICMP(internet);    
  } else {
    work = false;
  }
  
  if (!work) {
    if (resetHost(pin0, timeReset0)) {
      Serial.println();
      Serial.println("Reset router.....");
      timeReset0 = millis()/1000;  
    }        
  } 
}

void checkRig1() {
  if (isWiFiConnected) {
      if (!pingHost(rig1)) {
        if (resetHost(pin1, timeReset1)) {
            Serial.println("Reset Rig1.....");
            timeReset1 = millis()/1000;  
        }        
      } 
  }
}

void checkRig2() {
  if (isWiFiConnected) {
      if (!pingHost(rig2)) {
        if (resetHost(pin2, timeReset2)) {
            Serial.println("Reset Rig2.....");      
            timeReset2 = millis()/1000;  
        }        
      } 
  }
}

void checkRig3() {
  if (isWiFiConnected) {
      if (!pingHost(rig3)) {
        if (resetHost(pin3, timeReset3)) {
            Serial.println("Reset Rig3.....");      
            timeReset3 = millis()/1000;  
        }        
      } 
  }
}

void checkRig4() {
  if (isWiFiConnected) {
      if (!pingHost(rig4)) {
        if (resetHost(pin4, timeReset4)) {
            Serial.println("Reset Rig4.....");      
            timeReset4 = millis()/1000;  
        }        
      } 
  }
}

boolean resetHost(int pin, int timeReset) {
  int currentTime = millis()/1000;  
  Serial.println(currentTime - timeReset); 
  if ((currentTime - timeReset) > timeRebot) {   
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      return true;
  }
  return false;
}

boolean pingHost(char* remote_host) {
  Serial.println("");
  Serial.println("");
  Serial.print("Ping host ");
  Serial.print(remote_host);
  Serial.println(": ");

//Пинг ип адреса
  if (!pingICMP(remote_host)){
    return false;
  }

//Пинг программы Клаймор
  if (!pingClaymore(remote_host)){
    return false;
  }
  return true;   
}

boolean pingClaymore(char* remote_host)
{
  WiFiClient client;
  int httpPort = 3333;
  
  Serial.print("      Claymore: ");
  if (client.connect(remote_host, httpPort)) {   
    Serial.println("Success!!       ");
    client.stop();
    return true;
  } else {
    Serial.println("Error :(        ");
    return false;
  }
  return false; 
}

boolean pingICMP(char* remote_host) {
  Serial.print("      ICMP: ");

  if(Ping.ping(remote_host)) {
      Serial.println("Success!!       ");
      return true;
  } else {
      Serial.println("Error :(        ");
      return false;
  }
  return false;   
}

