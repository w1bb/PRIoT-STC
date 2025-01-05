// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#include "uc_type.hpp"

#include "sensor_reader.hpp"
#if defined(WISHTC_UC_CONTROL_THERMOSTAT) && WISHTC_UC_CONTROL_THERMOSTAT == true
#include "thermostat_controller.hpp"
#endif

#include "networking.hpp"

using namespace wi::sh::temp_control;

const sensor_type_t sensor_type = sensor_type_t::DHT22;

custom_mqtt_t custom_mqtt(uid);

sensor_reader_t sensor_reader(
    custom_mqtt,
    name,
    temp_and_hum_sensor_pin,
    sensor_type);

#if defined(WISHTC_UC_CONTROL_THERMOSTAT) && WISHTC_UC_CONTROL_THERMOSTAT == true
thermostat_controller_t thermostat_controller(
    custom_mqtt,
    name,
    thermostat_relay_pin);
#endif

void setup() {
    // Start serial communication
    Serial.begin(9600);
    delay(1000);

    Serial.println("\n === WELCOME ===\n");

    // Connect to Wi-Fi
    sensor_reader.setup();

    // Control the thermostat
#if defined(WISHTC_UC_CONTROL_THERMOSTAT) && WISHTC_UC_CONTROL_THERMOSTAT == true
    thermostat_controller.setup();
#endif
}

void loop() {
#if defined(WISHTC_UC_CONTROL_THERMOSTAT) && WISHTC_UC_CONTROL_THERMOSTAT == true
    thermostat_controller.update(5000);
#endif

    sensor_reader.publish(3000);    
}
