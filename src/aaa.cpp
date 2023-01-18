#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <WiFi.h>

#define DHTPIN 15
#define DHTTYPE DHT11

// WiFi
const char *ssid = "Nh3";           // Enter your WiFi name
const char *password = "123456789"; // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.233.220";
const char *topicSubcribe = "CarbonIoT/62ce96a2cd95012e5f7155de/command";
const char *topicPublish = "CarbonIoT/62ce96a2cd95012e5f7155de/data";
const char *ledId1 = "62d79b3f9b3e7f9f29318892";
const char *ledId2 = "62db7197454d13727755da9f"; // 62db7197454d13727755da9f
const char *tempId = "62d570e7ee81c4e7eca6ade2";
const char *humiId = "62dbab98676973fcd49acf89";
const char *mqtt_username = "62ce96a2cd95012e5f7155de";
const char *mqtt_password = "123456789";
const char *homeId = "62ce96a2cd95012e5f7155de";
const char *roomId1 = "62ce96c7cd95012e5f7155e1";
const char *roomId2 = "62ce96cfcd95012e5f7155e4";

StaticJsonDocument<256> sendingStatusLed;
StaticJsonDocument<256> sendingValueSensor;
const int mqtt_port = 1883;
volatile byte state1 = HIGH;
volatile byte state2 = HIGH;
int LED1 = 5;
int LED2 = 23;
int LED3 = 19;
int lightSensor = 18;
ezButton button1(32);
ezButton button2(33);
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long start = 0, delayTime = 200000;
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Co tin nhan moi tu topic:");
  Serial.println(topic);
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload);

  if (doc["value"] == "ON")
  {
    if (strcmp(doc["deviceId"], ledId1) == 0 && state1 == LOW)
    {
      Serial.println("LED1 on");
      digitalWrite(LED1, HIGH);
      state1 = HIGH;
      sendingStatusLed["roomId"] = roomId1;
      sendingStatusLed["name"] = "status";
      sendingStatusLed["value"] = "ON";
      sendingStatusLed["deviceId"] = ledId1;
      char out[256];
      serializeJson(sendingStatusLed, out);
      client.publish(topicPublish, out, true);
    }
    else if (strcmp(doc["deviceId"], ledId2) == 0 && state2 == LOW)
    {
      Serial.println("LED2 on");
      digitalWrite(LED2, HIGH);
      state2 = HIGH;
      sendingStatusLed["roomId"] = roomId2;
      sendingStatusLed["name"] = "status";
      sendingStatusLed["value"] = "ON";
      sendingStatusLed["deviceId"] = ledId2;
      char out[256];
      serializeJson(sendingStatusLed, out);
      client.publish(topicPublish, out, true);
    }
  }
  else if (doc["value"] == "OFF")
  {
    if (strcmp(doc["deviceId"], ledId1) == 0 && state1 == HIGH)
    {
      Serial.println("LED off");
      digitalWrite(LED1, LOW);
      state1 = LOW;
      sendingStatusLed["roomId"] = roomId1;
      sendingStatusLed["name"] = "status";
      sendingStatusLed["value"] = "OFF";
      sendingStatusLed["deviceId"] = ledId1;
      char out[256];
      serializeJson(sendingStatusLed, out);
      client.publish(topicPublish, out, true);
    }
    else if (strcmp(doc["deviceId"], ledId2) == 0 && state2 == HIGH)
    {
      Serial.println("LED off");
      digitalWrite(LED2, LOW);
      state2 = LOW;
      sendingStatusLed["roomId"] = roomId2;
      sendingStatusLed["name"] = "status";
      sendingStatusLed["value"] = "OFF";
      sendingStatusLed["deviceId"] = ledId2;
      char out[256];
      serializeJson(sendingStatusLed, out);
      client.publish(topicPublish, out, true);
    }
  }
}
void setup()
{
  // Set software serial baud to 9600;
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  button1.setDebounceTime(50);
  button2.setDebounceTime(50);
  digitalWrite(LED1, state1);
  digitalWrite(LED2, state2);

  dht.begin();
  sendingStatusLed["homeId"] = homeId;

  sendingValueSensor["homeId"] = homeId;
  sendingValueSensor["roomId"] = roomId1;
  // connecting to a WiFi network
  // WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe

  client.subscribe(topicSubcribe);
}

void loop()
{
  client.loop();
  button1.loop();
  button2.loop();
  if (button1.isPressed())
  {
    state1 = !state1;
    digitalWrite(LED1, state1);
    sendingStatusLed["roomId"] = roomId1;
    sendingStatusLed["name"] = "status";
    sendingStatusLed["value"] = state1 ? "ON" : "OFF";
    sendingStatusLed["deviceId"] = ledId1;
    char out[256];
    serializeJson(sendingStatusLed, out);
    client.publish(topicPublish, out, true);
    Serial.println("Pin status:" + state1);
  }
  if (button2.isPressed())
  {
    state2 = !state2;
    digitalWrite(LED2, state2);
    sendingStatusLed["roomId"] = roomId2;
    sendingStatusLed["name"] = "status";
    sendingStatusLed["value"] = state2 ? "ON" : "OFF";
    sendingStatusLed["deviceId"] = ledId2;
    char out[256];
    serializeJson(sendingStatusLed, out);
    client.publish(topicPublish, out, true);
  }
  if (start < delayTime)
  {
    start++;
  }
  else
  {
    start = 0;
    float humi = dht.readHumidity();
    // read temperature in Celsius
    float tempC = dht.readTemperature();
    // read temperature in Fahrenheit
    float tempF = dht.readTemperature(true);

    // check whether the reading is successful or not
    if (isnan(tempC) || isnan(tempF) || isnan(humi))
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    else
    {
      sendingValueSensor["name"] = "humidity";
      sendingValueSensor["deviceId"] = humiId;
      sendingValueSensor["value"] = humi;
      char outHumi[256];
      serializeJson(sendingValueSensor, outHumi);
      client.publish(topicPublish, outHumi);
      Serial.print("Humidity: ");
      Serial.print(humi);
      Serial.print("%");
      Serial.print("  |  ");
      char outTemp[256];
      sendingValueSensor["name"] = "temperature";
      sendingValueSensor["deviceId"] = tempId;
      sendingValueSensor["value"] = tempC;
      serializeJson(sendingValueSensor, outTemp);
      client.publish(topicPublish, outTemp);
      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.print("°C  ~  ");
      Serial.print(tempF);
      Serial.println("°F");
    }
  }
}
