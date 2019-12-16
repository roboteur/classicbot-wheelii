/* ROBOT WHEELII (by The Roboteur)
 *   
 * Website: www.roboteur.me
 * Facebook: facebook.com/TheRoboteur
 * Instagram: instagram.com/the_roboteur
 * YouTube: bitly.com/RoboteurTV  
 * GitHub: github.com/roboteur
 *  
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

/***************************************************************/


const char* ssid = "ZTEH108N_3C01E";          // Wifi User Name
const char* password = "X9425TE9";            // Wifi Password


/********************** OTA CODES ******************************/

ESP8266WebServer server;

bool ota_flag = true;
uint16_t time_elapsed = 0;
int blinkSpeed = 1000;
int state_current = 0;

const int IN1 = D5;
const int IN2 = D6;
const int IN3 = D7;
const int IN4 = D8;

const int ENA = D1;      
const int ENB = D3;

void setup() {
  pinMode(2, OUTPUT);                                                    
  Serial.begin(115200); 
  Serial.println("Starting ...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Restarting...");
    delay(5000);
    ESP.restart();
  }
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode (ENB, OUTPUT);
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/ota", [](){
    server.send(200, "text/plain", "Upload the firmware.");
    delay(1000);
    ESP.restart();    
  });
 
  server.on("/reset", [](){
    server.send(200, "text/plain", "Resetting...");
    delay(1000);
    ESP.restart();    
  });

  server.on("/", handle_OnConnect);
  server.on("/forward", handle_Forward);
  server.on("/backward", handle_Backward);
  server.on("/right", handle_Right);
  server.on("/left", handle_Left);
  server.on("/mid", handle_Mid);  
  
  server.begin();  
  
}

void loop() {
  
  // OTA FLAG WORKS ONLY AT ROUTE IP/ota
  if(ota_flag)
  {
    while(time_elapsed < 25000)
    {
      ArduinoOTA.handle();
      time_elapsed = millis();
      delay(10);
    }
    ota_flag = false;
  }

  server.handleClient();

  digitalWrite(LED_BUILTIN, HIGH);
  state_machine();

}

void state_machine() {
  analogWrite(ENA, 375); 
  analogWrite(ENB, 300); 
    
    // state_previous = state_current;
    
  switch (state_current) {       
      
      case 0: // Stop
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, LOW);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, LOW);
           break;
      
      case 1: // Fixed 80 Speed Backward 
           digitalWrite(IN1, HIGH);
           digitalWrite(IN2, LOW);
           digitalWrite(IN3, HIGH);
           digitalWrite(IN4, LOW);
           break;
      
      case 2: // Fixed 80 Speed Forward            
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, HIGH);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, HIGH);
           break;
           
      case 3: // Left
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, HIGH);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, LOW);
           break;
          
      case 4: //Right
           digitalWrite(IN1, LOW);
           digitalWrite(IN2, LOW);
           digitalWrite(IN3, LOW);
           digitalWrite(IN4, HIGH);
           break;      
  }  
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
    state_current = 6;
  
  }

void handle_Forward()  {
  state_current = 2;
  server.send(200, "text/html", ForwardHTML());
  
  }

void handle_Backward()  {
  state_current = 1;
  server.send(200, "text/html", BackwardHTML());
  
  }

void handle_Right()  {
  state_current = 3;
  server.send(200, "text/html", RightHTML());
  
  }

void handle_Left()  {
  state_current = 4;
  server.send(200, "text/html", LeftHTML());
  
  }
  
void handle_Mid()  {
  state_current = 0;
  server.send(200, "text/html", MidHTML());
  
  }


String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

String LeftHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

String RightHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

String ForwardHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

String BackwardHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }

String MidHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>WHEELII</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 12px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>WHEELII</h1>\n";
  ptr +="<p><a href=\"/forward\"><button>F</button></a></p>";
  ptr +="<p><a href=\"/left\"><button>L</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/mid\"><button>-</button></a>&nbsp;&nbsp;&nbsp;<a href=\"/right\"><button>R</button></a></p>";
  ptr +="<p><a href=\"/backward\"><button>B</button></a></p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
  }
