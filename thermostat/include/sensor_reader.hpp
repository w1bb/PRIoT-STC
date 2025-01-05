// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#ifndef SENSOR_READER_HPP_bb49d0e517d2
#define SENSOR_READER_HPP_bb49d0e517d2

#include <DHT.h>

#include "networking.hpp"

namespace wi::sh::temp_control {

enum class sensor_type_t {
    DHT22,
};

class sensor_reader_t {
    static const char *s_mqtt_topic_root;

    custom_mqtt_t& m_client;
    String m_name;

    char m_mqtt_topic[64];

    DHT m_sensor;

    long m_last_msg_time;
    
public:
    sensor_reader_t(
        custom_mqtt_t& client,
        const char *name,
        const uint8_t pin,
        const sensor_type_t sensor_type);

    void setup();
    void publish(const long delta_ms);
};

} // namespace wi::sh::temp_control

#endif // SENSOR_READER_HPP_bb49d0e517d2
