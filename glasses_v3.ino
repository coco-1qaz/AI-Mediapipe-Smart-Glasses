/*

esp01(esp8266):
en  --- 3.3v
vcc --- 3.3v
tx  --- 1
rx  --- 0
--- upload ---
i00 --- gnd

Arduino code:

const int tx = 1;
const int rx = 0;
void setup() {
  pinMode(rx,INPUT_PULLUP);
  pinMode(tx,INPUT_PULLUP);
}
void loop() {
}


*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   
#include <ESP8266WebServer.h>  

#include <FastLED.h>

#define LED_PIN     0
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    60

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

int mode = 0;
int timeCounter = 0;
int timeCounter1 = 0;
bool gReverseDirection = false;
CRGB leds[NUM_LEDS];

ESP8266WiFiMulti wifiMulti;     
ESP8266WebServer esp8266_server(80);

#define WIFINAME "kekego2"  
#define WIFIPW   "tang0123"  
IPAddress local_IP(192, 168, 0, 11);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
 
void setup(void){
  Serial.begin(9600);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFINAME, WIFIPW);  
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);                      
    Serial.print(' ');
  }                                 
  Serial.print("WIFI SSID: ");  
  Serial.println(WiFi.SSID());             
  Serial.print("IP address: ");            
  Serial.println(WiFi.localIP());          
  
  esp8266_server.begin();                
  esp8266_server.on("/", HTTP_GET, handleRoot);
  esp8266_server.on("/one", HTTP_POST, handleM1);
  esp8266_server.on("/two", HTTP_POST, handleM2);
  esp8266_server.on("/stop", HTTP_POST, handleStop);
  esp8266_server.onNotFound(handleNotFound);
  
  Serial.println("HTTP esp8266_server started");

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}
  
void loop(void){
  esp8266_server.handleClient();
  if (mode == 1) {
    breathe();
  } else if (mode == 2) {
    Fire2012();
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
    FastLED.show();
  }
}
                                                                          
void handleRoot() {  
  esp8266_server.send(200, "text/html", "<h3>Hello from ESP8266</h3><form action=\"/one\" method=\"POST\"><input type=\"submit\" value=\"mode one\"></form><form action=\"/two\" method=\"POST\"><input type=\"submit\" value=\"mode two\"></form><form action=\"/stop\" method=\"POST\"><input type=\"submit\" value=\"mode stop\">");
}

void handleNotFound(){                                    
  esp8266_server.send(404, "text/plain", "404: Not found"); 
}

void handleM1() {                          
  mode = 1;
  Serial.print("1");
  esp8266_server.sendHeader("Location","/");
  esp8266_server.send(303); 
}
void handleM2() {        
  mode = 2;              
  Serial.print("2");
  esp8266_server.sendHeader("Location","/");
  esp8266_server.send(303); 
}
void handleStop() {        
  mode = 0;              
  Serial.print("0");
  esp8266_server.sendHeader("Location","/");
  esp8266_server.send(303); 
}

#define COOLING  55
#define SPARKING 120

void Fire2012() {
  static uint8_t heat[NUM_LEDS];
  for( int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  
  if( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  for( int j = 0; j < NUM_LEDS; j++) {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    if( gReverseDirection ) {
      pixelnumber = (NUM_LEDS-1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}

void breathe() {
  CRGB color = CRGB(248, 130, 10); // Direct assignment of color
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}