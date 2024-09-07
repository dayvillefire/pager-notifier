#include <Arduino.h>
#include <cstring>

#include <WiFi.h>
#include <ESPPubSubClientWrapper.h>

#include "config.h"

WiFiClient espClient;
ESPPubSubClientWrapper client((const char *)mqtt_broker, mqtt_port);

void alarm()
{
    Serial.print("alarm: ");
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
    Serial.print("pubSubCallback: Message arrived in topic: ");
    Serial.println(topic);
    alarm();
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
        Serial.println("setup: Connecting to WiFi..");
    }
    Serial.println("setup: Connected to the Wi-Fi network");

    // connecting to a mqtt broker
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    client.on(topic, pubSubCallback);

    /*
    while (!client.connected())
    {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("setup: The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
        {
            Serial.println("setup: Public EMQX MQTT broker connected");
        }
        else
        {
            Serial.print("setup: Failed with state ");
            Serial.print(client.state());
            Serial.print("\n");
            delay(2000);
        }
    }

    // Publish and subscribe
    Serial.println("setup: Subscribe to topic");
    client.subscribe(topic);
    */

    Serial.println("ONLINE: TEST BEEP");
    alarm();
}

void loop()
{
    client.loop();
}