#include "hal/HalPower.h"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>

#include "os/Logging.h"
#include "os/OsTime.h"

#if defined(CONFIG_ADC)
#include <zephyr/drivers/adc.h>
#endif

HalPower power;

#define USER_NODE DT_PATH(zephyr_user)

#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, pwr_hold_gpios)
static const struct gpio_dt_spec pwrHold = GPIO_DT_SPEC_GET(USER_NODE, pwr_hold_gpios);
#endif

#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels) && \
    (DT_PROP_LEN(USER_NODE, io_channels) > 2)
static const struct adc_dt_spec batteryAdc = ADC_DT_SPEC_GET_BY_IDX(USER_NODE, 2);
#endif

bool HalPower::begin() {
#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, pwr_hold_gpios)
  if (!gpio_is_ready_dt(&pwrHold) || gpio_pin_configure_dt(&pwrHold, GPIO_OUTPUT_ACTIVE) != 0) {
    LOG_ERR("PWR", "latch init failed");
    return false;
  }
#endif

#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels) && \
    (DT_PROP_LEN(USER_NODE, io_channels) > 2)
  if (!adc_is_ready_dt(&batteryAdc) || adc_channel_setup_dt(&batteryAdc) != 0) {
    LOG_ERR("PWR", "battery adc init failed");
    return false;
  }
#endif

  return true;
}

uint16_t HalPower::batteryPercent() const {
#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels) && \
    (DT_PROP_LEN(USER_NODE, io_channels) > 2)
  int16_t sample = 0;
  struct adc_sequence sequence = {
      .buffer = &sample,
      .buffer_size = sizeof(sample),
  };
  adc_sequence_init_dt(&batteryAdc, &sequence);
  if (adc_read_dt(&batteryAdc, &sequence) != 0) {
    return 0;
  }

  int32_t mv = sample;
  if (adc_raw_to_millivolts_dt(&batteryAdc, &mv) != 0) {
    mv = (static_cast<int32_t>(sample) * 3300) / 4095;
  }
  mv *= 2;
  if (mv <= 3300) {
    return 0;
  }
  if (mv >= 4200) {
    return 100;
  }
  return static_cast<uint16_t>(((mv - 3300) * 100) / 900);
#else
  return 100;
#endif
}

void HalPower::shutdown() {
  LOG_INF("PWR", "releasing power latch");
#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, pwr_hold_gpios)
  gpio_pin_set_dt(&pwrHold, 0);
#endif
  osSleepMs(100);
  sys_reboot(SYS_REBOOT_COLD);
}
