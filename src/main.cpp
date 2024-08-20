#include <Arduino.h>
#include <cstring>

#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void alarm()
{
    Serial.print("Alarm: ");
    for (int i = 0; i < BUZZ_COUNT; i++)
    {
        Serial.print("LOW ");
        digitalWrite(BUZZER_PIN, LOW);
        delay(BUZZ_DURATION);
        Serial.print("HIGH ");
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZ_INTERVAL);
    }
    Serial.print("LOW\n");
    digitalWrite(BUZZER_PIN, LOW);
}

void pubSubCallback(char *topic, byte *raw, unsigned int length)
{
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    alarm();
}

bool mqtt_connect(PubSubClient *client)
{

    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client->connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
        Serial.println("Public EMQX MQTT broker connected");
        return true;
    }
    else
    {
        Serial.print("Failed with state ");
        Serial.print(client->state());
        delay(2000);
        return false;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(0);
    while (!Serial)
        ; // wait for serial port to connect.

    pinMode(BUZZER_PIN, OUTPUT);
    // pinMode(BUZZER_PIN, PULLUP);

    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");

    // connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(pubSubCallback);
    while (!client.connected())
    {
        if (mqtt_connect(&client))
        {
            break;
        }
    }

    // Publish and subscribe
    Serial.println("- Subscribe to topic");
    client.subscribe(topic);
}

void loop()
{
    while (!client.connected())
    {
        if (mqtt_connect(&client))
        {
            break;
        }
    }
    client.loop();
}