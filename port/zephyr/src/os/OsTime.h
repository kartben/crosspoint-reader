#pragma once

#include <cstdint>

#include <zephyr/kernel.h>

inline uint32_t osUptimeMs()
{
  return k_uptime_get_32();
}

inline void osSleepMs(uint32_t ms)
{
  k_msleep(ms);
}
