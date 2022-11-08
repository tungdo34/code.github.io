#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "ThingsBoard.h"
#include <ESP8266WiFi.h>

#define pump D0
#define fan D1
#define heater D2
#define valve D3
#define modeam D5

#define WIFI_AP "Van Tung"
#define WIFI_PASSWORD "1223334444T"

char Thingsboard_Server[] = "demo.thingsboard.io";
//#define TOKEN "Cr7wjQlmzqxeQ0py0Q83"
#define TOKEN "Aewa0xyjAXX3I7YhaqBE"

WiFiClient wifiClient;
WiFiClient espClient;

ThingsBoard tb(espClient);
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

int temp, humid;
int modeam_s;
char ch;
char data[5];
float lastSend = 0;
String str;

void setup()
{
  temp = 0;
  humid = 0;

  pinMode(modeam, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(valve, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(modeam, OUTPUT);

  digitalWrite(pump, LOW);
  digitalWrite(valve, LOW);
  digitalWrite(fan, LOW);
  digitalWrite(heater, LOW);

  temp = 12;
  humid = 15;
  //  digitalWrite(modeam, HIGH);

  Serial.begin(9600);
  InitWiFi();
  client.setServer(Thingsboard_Server, 1883);
  client.setCallback(callback_sub);

}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  if (Serial.available())
  {
    temp = Serial.read();
    delay(5);
    humid = Serial.read();
    SendDataToThingsboard(temp, humid);
//    }
  }

  client.loop();

}

void callback_sub(const char* topic, byte* payload, unsigned int length)
{
  Serial.println("Yes");
  StaticJsonDocument<256> data;
  deserializeJson(data, payload, length);
  String method1 = data["method"].as<String>();

  if (method1 == "D5")
  {
    if (data["params"] == true)
    {
      digitalWrite(modeam, LOW);
      modeam_s = 0;
      String payload7 = "{\"D5\":\"true\"}";
      char attributes7[100];
      payload7.toCharArray( attributes7, 100 );
      client.publish( "v1/devices/me/attributes", attributes7 );
    }
    else if (data["params"] == false)
    {
      digitalWrite(modeam, HIGH);
      modeam_s = 1;
      String payload7 = "{\"D5\":\"false\"}";
      char attributes7[100];
      payload7.toCharArray( attributes7, 100 );
      client.publish( "v1/devices/me/attributes", attributes7 );
    }
  }

  if (method1 == "D1")
  {
    if ((data["params"] == true) && (modeam_s == 0))
    {
      digitalWrite(pump, HIGH);
      String payload3 = "{\"D1\":\"true\"}";
      char attributes3[100];
      payload3.toCharArray( attributes3, 100 );
      client.publish( "v1/devices/me/attributes", attributes3 );

    }
    else if ((data["params"] == false) && (modeam_s == 0))
    {
      digitalWrite(pump, LOW);
      String payload3 = "{\"D1\":\"false\"}";
      char attributes3[100];
      payload3.toCharArray( attributes3, 100 );
      client.publish( "v1/devices/me/attributes", attributes3 );
    }
  }
  if (method1 == "D2")
  {
    if ((data["params"] == true) && (modeam_s == 0))
    {
      digitalWrite(fan, HIGH);
      String payload4 = "{\"D2\":\"true\"}";
      char attributes4[100];
      payload4.toCharArray( attributes4, 100 );
      client.publish( "v1/devices/me/attributes", attributes4 );
    }
    else if ((data["params"] == false) & (modeam_s == 0))
    {
      digitalWrite(fan, LOW);
      String payload4 = "{\"D2\":\"false\"}";
      char attributes4[100];
      payload4.toCharArray( attributes4, 100 );
      client.publish( "v1/devices/me/attributes", attributes4 );
    }
  }
  if (method1 == "D3")
  {
    if ((data["params"] == true) & (modeam_s == 0))
    {
      digitalWrite(heater, HIGH);
      String payload5 = "{\"D3\":\"true\"}";
      char attributes5[100];
      payload5.toCharArray( attributes5, 100 );
      client.publish( "v1/devices/me/attributes", attributes5 );
    }
    else if ((data["params"] == false) & (modeam_s == 0))
    {
      digitalWrite(heater, LOW);
      String payload5 = "{\"D3\":\"false\"}";
      char attributes5[100];
      payload5.toCharArray( attributes5, 100 );
      client.publish( "v1/devices/me/attributes", attributes5 );
    }
  }
  if (method1 == "D4")
  {
    if ((data["params"] == true) & (modeam_s == 0))
    {
      digitalWrite(valve, HIGH);
      String payload6 = "{\"D4\":\"true\"}";
      char attributes6[100];
      payload6.toCharArray( attributes6, 100 );
      client.publish( "v1/devices/me/attributes", attributes6 );
    }
    else if ((data["params"] == false) & (modeam_s == 0))
    {
      digitalWrite(valve, LOW);
      String payload6 = "{\"D4\":\"false\"}";
      char attributes6[100];
      payload6.toCharArray( attributes6, 100 );
      client.publish( "v1/devices/me/attributes", attributes6 );
    }
  }
}

void SendDataToThingsboard(int temp, int humid)
{
  if ( millis() - lastSend > 1000 )
  {
    // Prepare a JSON payload string
    // Temperature
    String payload = "{";
    payload += "\"temp\":\"" + (String)temp + "\"}";
    char attributes[100];
    payload.toCharArray( attributes, 100 );
    client.publish( "v1/devices/me/telemetry", attributes );

    // Humidity
    String payload1 = "{";
    payload1 += "\"humid\":\"" + (String)humid + "\"}";
    char attributes1[100];
    payload1.toCharArray( attributes1, 100 );
    client.publish( "v1/devices/me/telemetry", attributes1 );
  }
  Serial.println("Sent data to Thingsboard ");
  lastSend = millis();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network



  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
  Serial.print("Connecting to Thingsboard node ...");
  if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
    Serial.println( "[DONE]" );
    client.subscribe("v1/devices/me/rpc/request/+");
  } else {
    Serial.print( "[FAILED]" );
    Serial.println( " : retrying in 5 seconds]" );
    // Wait 5 seconds before retrying
    delay( 5000 );
  }
}
