#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED1 15
#define LED2 2

String ledStatus1 = "OFF";
String ledStatus2 = "OFF";
// Replace with your network credentials
const char *ssid = "WFKL";
const char *password = "71759919kl";

//mqtt broker
const char *led1Id = "led001";
const char *led2Id = "led002";

StaticJsonDocument<256> sendingStatusLed;

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

#define MQTT_LED_TOPIC_SUB "mqtt_esp32/smart_home"
#define MQTT_LED_TOPIC_PUB "mqtt_esp32/led2"


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void connect_to_broker()
{
    while (!client.connected())
    {
        Serial.print("Attemping mqtt connection...");
        String clientId = "ESP32";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            client.subscribe(MQTT_LED_TOPIC_SUB);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    char status[20];
    Serial.println("-------new message from broker-----");
    Serial.print("topic: ");
    Serial.println(topic);
    Serial.print("message: ");
    Serial.write(payload, length);
    Serial.println();
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload);

    if (doc["value"] == "OFF"){
        if (strcmp(doc["deviceId"], led1Id) == 0 ){
            ledStatus1 == "OFF";
            digitalWrite(LED1, LOW);
            Serial.println("LED1 OFF");
            //publish led1
            // sendingStatusLed["value"] = ledStatus1;
            // sendingStatusLed["deviceId"] = led1Id;
            // char out[256];
            // serializeJson(sendingStatusLed, out);
            // client.publish(MQTT_LED_TOPIC_PUB, out, true);
        }
        if (strcmp(doc["deviceId"], led2Id) == 0){
            ledStatus2 == "OFF";
            digitalWrite(LED2, LOW);
            Serial.println("LED2 OFF");
            // sendingStatusLed["roomId"] = roomId1;
            sendingStatusLed["value"] = ledStatus2;
            sendingStatusLed["deviceId"] = led2Id;
            char out[256];
            serializeJson(sendingStatusLed, out);
            client.publish(MQTT_LED_TOPIC_PUB, out, true);

        }
    }else if (doc["value"] == "ON"){
        if (strcmp(doc["deviceId"], led1Id) == 0){
            ledStatus1 == "ON";
            digitalWrite(LED1, HIGH);
            Serial.println("LED1 ON");
            //publish led1
            // sendingStatusLed["value"] = ledStatus1;
            // sendingStatusLed["deviceId"] = led1Id;
            // char out[256];
            // serializeJson(sendingStatusLed, out);
            // client.publish(MQTT_LED_TOPIC_PUB, out, true);
        }
        if (strcmp(doc["deviceId"], led2Id) == 0){
            ledStatus2== "ON";
            digitalWrite(LED2, HIGH);
            Serial.println("LED2 ON");

            // sendingStatusLed["roomId"] = roomId1;
            sendingStatusLed["value"] = ledStatus2;
            sendingStatusLed["deviceId"] = led2Id;
            char out[256];
            serializeJson(sendingStatusLed, out);
            client.publish(MQTT_LED_TOPIC_PUB, out, true);
        }
    }
}
void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(500);
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
    connect_to_broker();
    Serial.println("Start transfer");
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
}

void loop(){
    client.loop();
    if (!client.connected())
    {
        connect_to_broker();
    }
    
}