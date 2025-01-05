// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#ifndef THERMOSTAT_CONTROLLER_HPP_bb49d0e517d2
#define THERMOSTAT_CONTROLLER_HPP_bb49d0e517d2

#include "networking.hpp"

namespace wi::sh::temp_control {

class thermostat_controller_t {
    static const char *s_mqtt_topic_root;

    custom_mqtt_t& m_client;
    String m_name;
    char m_mqtt_topic[64];
    const uint8_t m_pin;

    long m_last_msg_time;

    std::function<void(uint8_t*, unsigned int)> m_callback;

public:
    thermostat_controller_t(
        custom_mqtt_t& client,
        const char *name,
        const uint8_t pin);

    void setup();
    void update(const long delta_ms);
};

} // namespace wi::sh::temp_control

#endif // THERMOSTAT_CONTROLLER_HPP_bb49d0e517d2
