/*
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include "hal/HalDisplay.h"
#include "hal/HalGpio.h"
#include "hal/HalPower.h"
#include "hal/HalStorage.h"
#include "os/Logging.h"
#include "os/OsTime.h"

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

  display.clear(0x00);
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
