// Copyright 2024 Valentin-Ioan Vintilă.
// All rights reserved.

#ifndef UC_TYPE_HPP_bb49d0e517d2
#define UC_TYPE_HPP_bb49d0e517d2

#include <cstdint>

#include <Arduino.h>

// =============================================================================

#define WISHTC_UC_UID 1
#define WISHTC_UC_FLOOR 1
#define WISHTC_UC_ROOM 0

// For special uC UID == 0, also control the thermostat
#if defined(WISHTC_UC_UID) && WISHTC_UC_UID == 0
#define WISHTC_UC_CONTROL_THERMOSTAT true
#endif

// =============================================================================

#define WISHTC_STR_HELPER(x) #x
#define WISHTC_STR(x) WISHTC_STR_HELPER(x)

namespace wi::sh::temp_control {

static const char *uid =
    "wishtc-" WISHTC_STR(WISHTC_UC_UID);
static const char *name =
    "floor-" WISHTC_STR(WISHTC_UC_FLOOR)
    "/room-" WISHTC_STR(WISHTC_UC_ROOM);
static const uint8_t temp_and_hum_sensor_pin = GPIO_NUM_27;
#if defined(WISHTC_UC_CONTROL_THERMOSTAT) && WISHTC_UC_CONTROL_THERMOSTAT == true
static const uint8_t thermostat_relay_pin = GPIO_NUM_26;
#endif

} // namespace wi::sh::temp_control

#undef WISHTC_STR
#undef WISHTC_STR_HELPER

#endif // UC_TYPE_HPP_bb49d0e517d2
