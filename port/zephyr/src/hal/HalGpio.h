#pragma once

#include <cstdint>

class HalGpio {
 public:
  static constexpr uint8_t BTN_BACK = 0;
  static constexpr uint8_t BTN_CONFIRM = 1;
  static constexpr uint8_t BTN_LEFT = 2;
  static constexpr uint8_t BTN_RIGHT = 3;
  static constexpr uint8_t BTN_UP = 4;
  static constexpr uint8_t BTN_DOWN = 5;
  static constexpr uint8_t BTN_POWER = 6;
  static constexpr uint8_t BTN_COUNT = 7;

  bool begin();
  void update();
  bool isPressed(uint8_t button) const;
  bool wasPressed(uint8_t button) const;
  bool isUsbConnected() const;

 private:
  uint8_t pressedMask = 0;
  uint8_t prevPressedMask = 0;
};

extern HalGpio gpio;
