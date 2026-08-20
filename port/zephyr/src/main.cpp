/*
 * SPDX-License-Identifier: MIT
 */

#include <stddef.h>
#include <stdint.h>
#include <zephyr/kernel.h>

#include "hal/HalDisplay.h"
#include "hal/HalGpio.h"
#include "hal/HalPower.h"
#include "hal/HalStorage.h"
#include "os/Logging.h"
#include "os/OsTime.h"

static void setPixel(uint8_t* fb, int x, int y, bool black) {
  if (x < 0 || y < 0 || x >= HalDisplay::WIDTH || y >= HalDisplay::HEIGHT) {
    return;
  }
  uint8_t* byte = fb + static_cast<size_t>(y) * HalDisplay::WIDTH_BYTES + static_cast<size_t>(x / 8);
  const uint8_t bit = static_cast<uint8_t>(0x80 >> (x % 8));
  if (black) {
    *byte = static_cast<uint8_t>(*byte & ~bit);
  } else {
    *byte = static_cast<uint8_t>(*byte | bit);
  }
}

static void fillRect(uint8_t* fb, int x0, int y0, int x1, int y1, bool black) {
  for (int y = y0; y < y1; y++) {
    for (int x = x0; x < x1; x++) {
      setPixel(fb, x, y, black);
    }
  }
}

static void paintDemoPage(uint8_t* fb) {
  display.clear(0xFF);
  fillRect(fb, 0, 0, HalDisplay::WIDTH, 4, true);
  fillRect(fb, 0, HalDisplay::HEIGHT - 4, HalDisplay::WIDTH, HalDisplay::HEIGHT, true);
  fillRect(fb, 0, 0, 4, HalDisplay::HEIGHT, true);
  fillRect(fb, HalDisplay::WIDTH - 4, 0, HalDisplay::WIDTH, HalDisplay::HEIGHT, true);
  fillRect(fb, 24, 24, HalDisplay::WIDTH - 24, 64, true);

  int y = 88;
  const int lineH = 3;
  const int gap = 14;
  while (y + lineH < HalDisplay::HEIGHT - 40) {
    const int inset = ((y / 30) % 3 == 0) ? 80 : 24;
    fillRect(fb, 24, y, HalDisplay::WIDTH - inset, y + lineH, true);
    y += lineH + gap;
  }
  fillRect(fb, HalDisplay::WIDTH - 120, HalDisplay::HEIGHT - 28, HalDisplay::WIDTH - 24, HalDisplay::HEIGHT - 12, true);
}

int main(void) {
  LOG_INF("BOOT", "CrossPoint Zephyr port on %s", CONFIG_BOARD);

  if (!power.begin()) {
    LOG_ERR("BOOT", "power init failed");
  }
  if (!gpio.begin()) {
    LOG_ERR("BOOT", "gpio init failed");
  }
  if (!display.begin()) {
    LOG_ERR("BOOT", "display init failed");
  }
  if (!storage.begin()) {
    LOG_INF("BOOT", "storage not mounted");
  }

  paintDemoPage(display.framebuffer());
  display.present();
  LOG_INF("BOOT", "battery %u%% usb %d", power.batteryPercent(), gpio.isUsbConnected() ? 1 : 0);

  while (true) {
    gpio.update();
    if (gpio.wasPressed(HalGpio::BTN_POWER)) {
      LOG_INF("BOOT", "power button");
    }
    osSleepMs(50);
  }
}
