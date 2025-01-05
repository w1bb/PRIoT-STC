// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#ifndef NETWORKING_HPP_bb49d0e517d2
#define NETWORKING_HPP_bb49d0e517d2

#include <PubSubClient.h>
#include <WiFi.h>

// =============================================================================

namespace wi::sh::temp_control {

// This is secret (defined in src/secrets.cpp)
extern const char *s_wifi_ssid;
// This is secret (defined in src/secrets.cpp)
extern const char *s_wifi_passwd;
// This is secret (defined in src/secrets.cpp)
extern const char *s_mqtt_server;
// This is secret (defined in src/secrets.cpp)
extern const uint16_t s_mqtt_port;

} // namespace wi::sh::temp_control

// =============================================================================

namespace wi::sh::temp_control {

/**
 * Connect to the default WiFi network.
 */
void connect_to_wifi();

class custom_mqtt_t {
    bool m_was_setup;

    const char *m_uid;

    WiFiClient m_esp_client;
    PubSubClient m_client;

    double m_server_time;

    long m_last_loop;
public:
    custom_mqtt_t(const char *uid);

    void setup();

    void reconnect();

    void publish(const char *topic, String& msg);
    void publish(const char *topic, const char *msg);

    void single_subscribe(const char *topic, std::function<void(uint8_t*, unsigned int)>& callback);
    void unsubscribe(const char* topic);

    void loop();

    void request_server_time(const char *request_topic = "wi/sh/request", const char *ans_topic = "wi/sh/vntp");
    [[nodiscard]] double get_timestamp();
};

} // namespace wi::sh::temp_control

#endif // NETWORKING_HPP_bb49d0e517d2
