#include <PubSubClient.h>
#include <MKRGSM.h>

#define TOKEN ""
const char PINNUMBER[]     = "";
// APN data
const char GPRS_APN[]      = "internet"; //Setup APN of XL Axiata
const char GPRS_LOGIN[]    = "";
const char GPRS_PASSWORD[] = "";

// To connect with SSL/TLS:
// 1) Change GSMClient to GSMSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

GPRS gprs;
GSM gsmAccess;
GSMClient gsmClient;
PubSubClient client(gsmClient);
long lastData = 0;

const char server[] = "demo.thingsboard.io";   //"test.mosquitto.org";
int port = 1883;

const char publishTopic[]  = "v1/devices/me/telemetry";

void setup() {
  
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial) {
    ; 
  }

  // attempt to connect to GSM and GPRS:
  Serial.print("Attempting to connect to GSM and GPRS");
   // connection state
  bool connected = false;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password

  while (!connected) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println(": You're connected to the network");
  Serial.println();
  client.setServer(server, port);
  Serial.println(server);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println();

}

void loop() {
  
  if(!client.connected()){

    reconnect();
    
  }

  client.loop();

  
  getData();
  
}

void getData(){

  
  float t = random(20, 50);
  float h = random(50, 100);

  //String temperature  = String(t);
  //String humidity = String(h);

  /*String payload = "{\"airTemp\":";
  payload += temperature;
  payload += ",\"airHum\":";
  payload += humidity;
  payload += "}";*/

  String payload = "{";
  payload += "\"Humidity\":";
  payload += h;
  payload += ",";
  payload += "\"Temperature\":";
  payload += t;
  payload += "}";

  char attributes[1000];
  payload.toCharArray(attributes, 1000);

  long now = millis();

  if (now - lastData > 5000)
  {
    
    lastData = now;
    client.publish(publishTopic, attributes);
    Serial.println(attributes);

  }
  
}

void reconnect(){

  while(!client.connected()){
     Serial.print("Attempting to connect to the MQTT broker: ");

    if(client.connect("ClientID", TOKEN, NULL)){
      Serial.println("You're connected to the MQTT broker!");
    }
    else{
      Serial.print("Failed!");
      Serial.println(" retrying in 5 seconds");
      delay(3000);
    }
  }
}
