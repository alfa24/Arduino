#include <ESP8266Ping.h>


/*
 *  This sketch shows the WiFi event usage
 *
 */

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
class Device {
  public:
  char* ip;
  int pin;
  int err = 0;
  int timeReset = 0;  
};


//const char* ssid     = "Keenetic-6004";
//const char* password = "60600000";
const char* ssid     = "alfa24";
const char* password = "Free38ride";
const int countDev = 1;  //количество устройств
const int timeRebot = 600;  ////Время для перезагрузки компа (сек)
boolean isWiFiConnected; //флаг подключения к сети

Device devices[countDev];

//настройки для роутера
char* internet = "ya.ru";
char* router = "192.168.1.1";
int pin0 = 16;
int timeReset0 = 120;
int wifiLed = 14; //wi-fi connected

void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println();
            Serial.println("WiFi connected!!!");
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
    initDevices();
    
    ArduinoOTA.setHostname("WatchDOG01");
    ArduinoOTA.begin();
    
    Serial.begin(115200);
    pinMode(wifiLed, OUTPUT);
    pinMode(pin0, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
    
    // delete old config
    WiFi.disconnect(true);

    delay(1000);

    WiFi.onEvent(WiFiEvent);
    WiFi.begin(ssid, password);
    Serial.println("Wait for WiFi... ");
}

void initDevices(){
    devices[0].ip = "192.168.1.11";
    devices[0].pin = 5;
    devices[0].ip = "192.168.1.12";
    devices[0].pin = 4;
    devices[0].ip = "192.168.1.13";
    devices[0].pin = 0;
    devices[0].ip = "192.168.1.14";
    devices[0].pin = 2;    
}

void loop() {
    ArduinoOTA.handle();
    checkRouter(); 
    
    delay(60000); 
    int i;
    for (i = 0; i < countDev; i = i + 1) {
       checkDev(devices[i]);
    }

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

void checkDev(Device dev) {
  if (isWiFiConnected) {
      if (!pingHost(dev.ip)) {
        dev.err++;
        if (dev.err >= 5) {
          if (resetHost(dev.pin, dev.timeReset)) {
            Serial.println("Reset dev.....");
            dev.timeReset = millis()/1000;  
          }
        }
                
      } else {
        dev.err = 0;
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





