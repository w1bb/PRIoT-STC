// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#include "networking.hpp"

namespace wi::sh::temp_control {

void connect_to_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(s_wifi_ssid, s_wifi_passwd);
    Serial.println("\nConnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

custom_mqtt_t::custom_mqtt_t(const char *uid)
: m_was_setup(false)
, m_uid(uid)
, m_esp_client()
, m_client(m_esp_client)
, m_server_time()
, m_last_loop(0)
{}

void custom_mqtt_t::setup()
{
    if (m_was_setup) {
        return;
    }
    if (WiFi.status() != WL_CONNECTED) {
        connect_to_wifi();
    }
    m_client.setServer(s_mqtt_server, s_mqtt_port);
    request_server_time();
    m_was_setup = true;
}

void custom_mqtt_t::reconnect()
{
    do {
        Serial.print("\nAttempting MQTT connection...");
        if (WiFi.status() != WL_CONNECTED) {
            connect_to_wifi();
        }
        if (m_client.connect(m_uid)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(m_client.state());
            Serial.println(" try again in 2 seconds");
            delay(2000);
        }
    } while (!m_client.connected());
}

void custom_mqtt_t::publish(const char *topic, String& msg)
{
    static char c_msg[256];
    msg.toCharArray(c_msg, 256);
    publish(topic, c_msg);
}

void custom_mqtt_t::publish(const char *topic, const char *msg)
{
    if (!m_client.connected()) {
        reconnect();
    }
    Serial.print("Publishing message: ");
    Serial.println(msg);
    m_client.publish(topic, msg);
}

void custom_mqtt_t::single_subscribe(const char *topic, std::function<void(uint8_t*, unsigned int)>& callback)
{
    m_client.setCallback([&callback](char*, uint8_t* payload, unsigned int length) {
        if (payload != nullptr) {
            String str_payload(payload, length);
            Serial.println("Received payload: " + str_payload);
            callback(payload, length);
        }
    });
    if (!m_client.connected()) {
        reconnect();
    }
    m_client.subscribe(topic);
}

void custom_mqtt_t::unsubscribe(const char *topic)
{
    m_client.unsubscribe(topic);
}

void custom_mqtt_t::loop()
{
    if (!m_client.connected()) {
        reconnect();
    }

    const long now = millis();
    if (now - m_last_loop < 100) {
        return;
    }
    m_last_loop = now;
    m_client.loop();
}

void custom_mqtt_t::request_server_time(const char *request_topic, const char *ans_topic)
{
    double server_time = 0.0;
    std::function<void(uint8_t*, unsigned int)> callback = [&server_time](uint8_t *payload, unsigned int length) {
        String str_payload(payload, length);
        server_time = str_payload.toDouble();
    };
    single_subscribe(ans_topic, callback);
    while (server_time == 0.0) {
        publish(request_topic, "{ \"request\": \"time\" }");
        delay(500);
        m_client.loop();
    }
    m_server_time = server_time - esp_timer_get_time() / 100000.0;
    unsubscribe(ans_topic);
}

double custom_mqtt_t::get_timestamp()
{
    return m_server_time + esp_timer_get_time() / 100000.0;
}

} // namespace wi::sh::temp_control
