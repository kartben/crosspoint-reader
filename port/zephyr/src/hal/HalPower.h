#pragma once

#include <cstdint>

class HalPower {
 public:
  bool begin();
  uint16_t batteryPercent() const;
  void shutdown();
};

extern HalPower power;
