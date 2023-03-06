#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED1 15
#define LED2 2
#define BUT1 19
#define BUT2 21
String ledStatus1 = "OFF";
String ledStatus2 = "OFF";
// Replace with your network credentials
const char *ssid = "POCO M3";
const char *password = "12345678";

// mqtt broker
const char *led1Id = "63d4fc57d977b7b5ec8076c2";
const char *led2Id = "63d4fc57d977b7b5ec8076c3";
volatile byte state1 = HIGH;
volatile byte state2 = HIGH;

StaticJsonDocument<256> sendingStatusLed;

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883

#define MQTT_LED_TOPIC_SUB "iot/group8/control"
#define MQTT_LED_TOPIC_PUB "iot/group8/data"

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

    if (doc["control"]["status"] == "OFF")
    {
        if (strcmp(doc["deviceId"], led1Id) == 0)
        {
            ledStatus1 == "OFF";
            digitalWrite(LED1, LOW);
            Serial.println("LED1 OFF");
        }
        if (strcmp(doc["deviceId"], led2Id) == 0)
        {
            ledStatus2 == "OFF";
            digitalWrite(LED2, LOW);
            Serial.println("LED2 OFF");
        }
    }
    else if (doc["control"]["status"] == "ON")
    {
        if (strcmp(doc["deviceId"], led1Id) == 0)
        {
            ledStatus1 == "ON";
            digitalWrite(LED1, HIGH);
            Serial.println("LED1 ON");
        }
        if (strcmp(doc["deviceId"], led2Id) == 0)
        {
            ledStatus2 == "ON";
            digitalWrite(LED2, HIGH);
            Serial.println("LED2 ON");
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
    pinMode(BUT1, INPUT_PULLUP);
    pinMode(BUT2, INPUT_PULLUP);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
}

void loop()
{
    client.loop();
    if (!client.connected())
    {
        connect_to_broker();
    }
    if (digitalRead(BUT1) == 0)
    {
        Serial.print(digitalRead(BUT1));
        while (digitalRead(BUT1) == 0){
            /* cho nut dc nha */
        }
        ledStatus1 = (ledStatus1 == "ON") ? "OFF" : "ON";
        state1 = (ledStatus1 == "ON")? HIGH : LOW;
        digitalWrite(LED1, state1);
        sendingStatusLed["control"]["status"] = ledStatus1;
        sendingStatusLed["deviceId"] = led1Id;
        char out[256];
        serializeJson(sendingStatusLed, out);
        client.publish(MQTT_LED_TOPIC_PUB, out, false);
    }
    if (digitalRead(BUT2) == 0)
    {
        Serial.print(digitalRead(BUT2));
        while (digitalRead(BUT2) == 0){
            /* cho nut dc nha */
        }
        ledStatus2 = (ledStatus2 == "ON") ? "OFF" : "ON";
        state2 = (ledStatus2 == "ON") ? HIGH : LOW;
        digitalWrite(LED2, state2);
        sendingStatusLed["control"]["status"] = ledStatus2;
        sendingStatusLed["deviceId"] = led2Id;
        char out[256];
        serializeJson(sendingStatusLed, out);
        client.publish(MQTT_LED_TOPIC_PUB, out, false);
    }
}