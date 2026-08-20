#pragma once

#include <cstdint>

class HalDisplay {
 public:
  static constexpr uint16_t WIDTH = 800;
  static constexpr uint16_t HEIGHT = 480;
  static constexpr uint16_t WIDTH_BYTES = WIDTH / 8;
  static constexpr uint32_t BUFFER_SIZE = WIDTH_BYTES * HEIGHT;

  bool begin();
  void clear(uint8_t color = 0xFF);
  uint8_t* framebuffer();
  uint32_t bufferSize() const { return BUFFER_SIZE; }
  void present();

 private:
  bool ready = false;
};

extern HalDisplay display;
