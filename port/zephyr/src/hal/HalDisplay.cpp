#include "hal/HalDisplay.h"

#include <string.h>
#include <zephyr/devicetree.h>

#include "os/Logging.h"

#if DT_HAS_CHOSEN(zephyr_display)
#include <zephyr/drivers/display.h>
#endif

HalDisplay display;

// Same 48 KB 1-bpp panel buffer as the Arduino firmware. Static BSS avoids a
// 48 KB heap alloc on the ESP32-C3 (no PSRAM) and keeps the pointer stable.
alignas(4) static uint8_t frameBuffer[HalDisplay::BUFFER_SIZE];

bool HalDisplay::begin() {
  clear();
  ready = true;
  LOG_INF("DISP", "framebuffer %u bytes (%ux%u)", BUFFER_SIZE, WIDTH, HEIGHT);

#if DT_HAS_CHOSEN(zephyr_display)
  const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(dev)) {
    LOG_ERR("DISP", "display device not ready");
    return false;
  }
  display_blanking_off(dev);
#endif
  return true;
}

void HalDisplay::clear(uint8_t color) { memset(frameBuffer, color, BUFFER_SIZE); }

uint8_t* HalDisplay::framebuffer() { return frameBuffer; }

void HalDisplay::present() {
  if (!ready) {
    return;
  }

#if DT_HAS_CHOSEN(zephyr_display)
  const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(dev)) {
    return;
  }

  const struct display_buffer_descriptor desc = {
      .buf_size = BUFFER_SIZE,
      .width = WIDTH,
      .height = HEIGHT,
      .pitch = WIDTH,
  };
  const int err = display_write(dev, 0, 0, &desc, frameBuffer);
  if (err != 0) {
    LOG_ERR("DISP", "display_write failed: %d", err);
  }
#endif
}
