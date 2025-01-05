// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#include "sensor_reader.hpp"

#include <string.h>
#include "time.h"

namespace wi::sh::temp_control {

const char *sensor_reader_t::s_mqtt_topic_root = "wi/sh/temp_control/";

sensor_reader_t::sensor_reader_t(
    custom_mqtt_t& client,
    const char *name,
    const uint8_t pin,
    const sensor_type_t sensor_type)
: m_client(client)
, m_name(name)
, m_mqtt_topic()
, m_sensor(pin, (sensor_type == sensor_type_t::DHT22) ? DHT22 : -1)
, m_last_msg_time(0)
{
    // Compute the mqtt topic
    strcpy(m_mqtt_topic, sensor_reader_t::s_mqtt_topic_root);
    strcat(m_mqtt_topic, name);
}

void sensor_reader_t::setup()
{
    // Configure WiFi & MQTT
    m_client.setup();

    // Setup the sensor
    m_sensor.begin();
}

void sensor_reader_t::publish(const long delta_ms)
{
    // Check if it is time to publish
    const long now = millis();
    if (now - m_last_msg_time < delta_ms) {
        return;
    }
    m_last_msg_time = now;

    // Get the UNIX time.
    double unix_time = m_client.get_timestamp();

    // Get the temperature.
    // If this fails, temperature will be set to 1000.0.
    float temperature = m_sensor.readTemperature();
    if (temperature != temperature || temperature < -45.0f || temperature > 85.0f) {
        temperature = 1000.0;
    }
    // Get the humidity level.
    // If this fails, humidity will be set to 1000.0.
    float humidity = m_sensor.readHumidity();
    if (humidity != humidity || humidity < -0.0f || humidity > 100.0f) {
        humidity = 1000.0;
    }

    // Publish the message in JSON format
    String msg =
        "{ \"temperature\": " + String(temperature) +
        ", \"humidity\": " + String(humidity) +
        ", \"timestamp\": " + String(unix_time) +
        ", \"uid\": \"" + m_name + "\" }";
    m_client.publish(m_mqtt_topic, msg);
}

} // namespace wi::sh::temp_control
