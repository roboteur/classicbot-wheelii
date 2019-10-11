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

/**** DEFINE NEW  ****/
const int IN1 = D5;
const int IN2 = D6;
const int IN3 = D7;
const int IN4 = D8;

const int ENA = D1;      // D1
const int ENB = D3;


/*******************/ 

/**** SETUP ********/
void setup() {
  pinMode(2, OUTPUT);   // DEFAULT PIN/LED                                                       
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


/**************************** ROUTE ************************************/
/*** DO NOT DELETE ***/

  // SET THE ROUTE IP/ota thru ESP.restart() function
  // BYPASSES THE LOOP
  server.on("/ota", [](){
    server.send(200, "text/plain", "Upload the firmware.");
    delay(1000);
    ESP.restart();    
  });

  server.on("/", [](){
    server.send(200, "text/plain", "1] ip/b 2] ip/f 3] ip/r 4] ip/l");
    delay(1000);
  });

  server.on("/reset", [](){
    server.send(200, "text/plain", "Resetting...");
    delay(1000);
    ESP.restart();    
  });

  server.on("/b", [](){
    server.send(200, "text/plain", "Backward");
    delay(1000);
    state_current = 1;
  });

  server.on("/f", [](){
    server.send(200, "text/plain", "Forward");
    delay(1000);
    state_current = 2;
  });

  server.on("/s", [](){
    server.send(200, "text/plain", "Stop");
    delay(1000);
    state_current = 0;
  }); 

  server.on("/l", [](){
    server.send(200, "text/plain", "Left");
    delay(1000);
    state_current = 3;
  });
  
  server.on("/r", [](){
    server.send(200, "text/plain", "Right");
    delay(1000);
    state_current = 4;
  });

  server.on("/stop", [](){
    server.send(200, "text/plain", "Stop");
    delay(1000);
    state_current = 0;
  });
  
  server.begin();

/**** SETUP NEW  ****/



/*******************/ 
  
  
}

/***************************** BLINK ************************************/
/*** DO NOT DELETE ***/

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


/********************** START YOUR CODE HERE  ****************************/
  digitalWrite(LED_BUILTIN, HIGH);
  state_machine();


/************************************************************************/  
}

void state_machine() {
  analogWrite(ENA, 230); // lowest was 230
  analogWrite(ENB, 230); 
    
    // state_previous = state_current;
    
  switch (state_current) {       
      /* case 1: // On BUILTIN LED
           digitalWrite(LED_BUILTIN, HIGH);
           Serial.print("You are at case 49.");
           Serial.println(state_current);
           delay(1000);           
           break;
    
      case 2: // Off BUILTIN LED
           digitalWrite(LED_BUILTIN, LOW);
           Serial.print("You are at case 50.");
           Serial.println(state_current);
           delay(1000);
           break;
      */
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
