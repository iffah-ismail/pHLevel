#include <WiFi.h>
#include <MQTT.h>

int led1_pin = 2;
const int potPin = A0; 
float ph = 0;
float voltage = 0;


#define WIFI_SSID             "FSKMPocketWiFi"
#define WIFI_PASSWORD         "88888888"
#define MQTT_HOST             "broker.hivemq.com"
#define MQTT_PREFIX_TOPIC     "csm3313_umt/group11"
#define MQTT_PUBLISH_TOPIC1   "/phlevel"
#define MQTT_SUBSCRIBE_TOPIC1 "/led01"

WiFiClient net;
MQTTClient mqtt(1024);
unsigned long lastMillis = 0;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi '" + String(WIFI_SSID) + "' ...");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void messageReceived(String topic, String payload) {
  Serial.println("Incoming Status from topic " + topic + " -> " + payload);

  // check if the topic equals MQTT_SUBSCRIBE_TOPIC1
  if (topic == (String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1))) {
    if (payload == "1") {
      digitalWrite(led1_pin, HIGH);
      Serial.println("LED1 turned ON");
    } else if (payload == "0") {
      digitalWrite(led1_pin, LOW);
      Serial.println("LED1 turned OFF");
    }
  } else {
    Serial.println("Command does not match.");
  }
}

void connectToMqttBroker() {
  Serial.print("Connecting to '" + String(WIFI_SSID) + "' ...");

  mqtt.begin(MQTT_HOST, net);
  mqtt.onMessage(messageReceived);

  String uniqueString = String(WIFI_SSID) + "-" + String(random(1, 98)) + String(random(99, 999));
  char uniqueClientID[uniqueString.length() + 1];

  uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);

  while (!mqtt.connect(uniqueClientID)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");

  Serial.println("Subscribe to: " + String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));
  mqtt.subscribe(String(MQTT_PREFIX_TOPIC) + String(MQTT_SUBSCRIBE_TOPIC1));
}

void setup(void) {
  Serial.begin(115200);
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);

  connectToWiFi();
  connectToMqttBroker();
  Serial.println();

}

void loop() {
  mqtt.loop();
  delay(10); 

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqtt.connected()) {
    connectToMqttBroker();
  }

if (millis() - lastMillis > 1800000) { // publish every 30 minutes
    lastMillis = millis();
    // Your code to publish goes here
}
    
    voltage = analogRead(potPin) * (3.3 / 4095.0); 
  ph=(3.3*voltage);
  Serial.print("pH Level: ");
  Serial.println(ph);

  // Publish pH level to MQTT
  Serial.print("Publishing to MQTT topic: ");
  Serial.println(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1));
  mqtt.publish(String(MQTT_PREFIX_TOPIC) + String(MQTT_PUBLISH_TOPIC1), String(ph));

  }
 
}
