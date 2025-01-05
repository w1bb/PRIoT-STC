// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#include "thermostat_controller.hpp"

namespace wi::sh::temp_control {

const char *thermostat_controller_t::s_mqtt_topic_root = "wi/sh/thermostat/";

thermostat_controller_t::thermostat_controller_t(
    custom_mqtt_t& client,
    const char *name,
    const uint8_t pin)
: m_client(client)
, m_name(name)
, m_mqtt_topic()
, m_pin(pin)
, m_last_msg_time(0)
, m_callback([&m_pin = m_pin](uint8_t *payload, unsigned int length) {
    String str_payload(payload, length);
    digitalWrite(m_pin, str_payload.toInt());
})
{
    strcpy(m_mqtt_topic, thermostat_controller_t::s_mqtt_topic_root);
    strcat(m_mqtt_topic, name);
}

void thermostat_controller_t::setup()
{
    // Configure pin
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, 0);

    // Configure WiFi & MQTT
    m_client.setup();
    m_client.single_subscribe(
        m_mqtt_topic,
        m_callback
    );
}

void thermostat_controller_t::update(const long delta_ms)
{
    m_client.loop();

    const long now = millis();
    if (now - m_last_msg_time < delta_ms) {
        return;
    }
    m_last_msg_time = now;

    // Request a new status for the thermostat
    m_client.publish("wi/sh/request", 
        "{ \"request\": \"thermostat\""
        ", \"uid\": \"" + m_name + "\" }");
}

} // namespace wi::sh::temp_control
