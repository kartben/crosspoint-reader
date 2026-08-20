#include "hal/HalDisplay.h"

#include <string.h>

#include "os/Logging.h"

HalDisplay display;

// Same 48 KB 1-bpp panel buffer as the Arduino firmware. Static BSS avoids a
// 48 KB heap alloc on the ESP32-C3 (no PSRAM) and keeps the pointer stable.
alignas(4) static uint8_t frameBuffer[HalDisplay::BUFFER_SIZE];

bool HalDisplay::begin() {
  clear();
  ready = true;
  LOG_INF("DISP", "framebuffer %u bytes (%ux%u)", BUFFER_SIZE, WIDTH, HEIGHT);
  return true;
}

void HalDisplay::clear(uint8_t color) { memset(frameBuffer, color, BUFFER_SIZE); }

uint8_t* HalDisplay::framebuffer() { return frameBuffer; }

void HalDisplay::present() {
  if (!ready) {
    return;
  }
  // Panel write is not wired: Zephyr 4.4 has no SSD1677 driver.
}
