#include "hal/HalGpio.h"

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "os/Logging.h"

#if defined(CONFIG_ADC)
#include <zephyr/drivers/adc.h>
#endif

HalGpio gpio;

#define USER_NODE DT_PATH(zephyr_user)

#if DT_NODE_EXISTS(DT_NODELABEL(power_button))
static const struct gpio_dt_spec powerBtn = GPIO_DT_SPEC_GET(DT_NODELABEL(power_button), gpios);
#endif

#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, usb_detect_gpios)
static const struct gpio_dt_spec usbDetect = GPIO_DT_SPEC_GET(USER_NODE, usb_detect_gpios);
#endif

#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels)
static const struct adc_dt_spec adcBtn1 = ADC_DT_SPEC_GET_BY_IDX(USER_NODE, 0);
#if DT_PROP_LEN(USER_NODE, io_channels) > 1
static const struct adc_dt_spec adcBtn2 = ADC_DT_SPEC_GET_BY_IDX(USER_NODE, 1);
#endif
#endif

#if defined(CONFIG_ADC)
static constexpr int32_t kAdcNoButton = 3900;
static constexpr int32_t kLadder1[] = {3100, 2090, 750};
static constexpr int32_t kLadder2[] = {1120};

static int decodeLadder(int32_t raw, const int32_t* edges, int edgeCount) {
  if (raw >= kAdcNoButton) {
    return -1;
  }
  for (int i = 0; i < edgeCount; i++) {
    if (raw > edges[i]) {
      return i;
    }
  }
  return edgeCount;
}

static bool readAdcRaw(const struct adc_dt_spec* spec, int32_t* out) {
  int16_t sample = 0;
  struct adc_sequence sequence = {
      .buffer = &sample,
      .buffer_size = sizeof(sample),
  };

  adc_sequence_init_dt(spec, &sequence);
  const int err = adc_read_dt(spec, &sequence);
  if (err != 0) {
    return false;
  }
  *out = sample;
  return true;
}
#endif

bool HalGpio::begin() {
  bool ok = true;

#if DT_NODE_EXISTS(DT_NODELABEL(power_button))
  if (!gpio_is_ready_dt(&powerBtn) || gpio_pin_configure_dt(&powerBtn, GPIO_INPUT) != 0) {
    LOG_ERR("GPIO", "power button init failed");
    ok = false;
  }
#endif

#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, usb_detect_gpios)
  if (!gpio_is_ready_dt(&usbDetect) || gpio_pin_configure_dt(&usbDetect, GPIO_INPUT) != 0) {
    LOG_ERR("GPIO", "usb detect init failed");
    ok = false;
  }
#endif

#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels)
  if (!adc_is_ready_dt(&adcBtn1) || adc_channel_setup_dt(&adcBtn1) != 0) {
    LOG_ERR("GPIO", "adc btn1 init failed");
    ok = false;
  }
#if DT_PROP_LEN(USER_NODE, io_channels) > 1
  if (!adc_is_ready_dt(&adcBtn2) || adc_channel_setup_dt(&adcBtn2) != 0) {
    LOG_ERR("GPIO", "adc btn2 init failed");
    ok = false;
  }
#endif
#endif

  return ok;
}

void HalGpio::update() {
  prevPressedMask = pressedMask;
  pressedMask = 0;

#if DT_NODE_EXISTS(DT_NODELABEL(power_button))
  if (gpio_pin_get_dt(&powerBtn) > 0) {
    pressedMask |= static_cast<uint8_t>(1u << BTN_POWER);
  }
#endif

#if defined(CONFIG_ADC) && DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, io_channels)
  int32_t raw = 0;
  if (readAdcRaw(&adcBtn1, &raw)) {
    const int idx = decodeLadder(raw, kLadder1, 3);
    if (idx >= 0 && idx <= 3) {
      pressedMask |= static_cast<uint8_t>(1u << idx);
    }
  }
#if DT_PROP_LEN(USER_NODE, io_channels) > 1
  if (readAdcRaw(&adcBtn2, &raw)) {
    const int idx = decodeLadder(raw, kLadder2, 1);
    if (idx == 0) {
      pressedMask |= static_cast<uint8_t>(1u << BTN_UP);
    } else if (idx == 1) {
      pressedMask |= static_cast<uint8_t>(1u << BTN_DOWN);
    }
  }
#endif
#endif
}

bool HalGpio::isPressed(uint8_t button) const {
  if (button >= BTN_COUNT) {
    return false;
  }
  return (pressedMask & static_cast<uint8_t>(1u << button)) != 0;
}

bool HalGpio::wasPressed(uint8_t button) const {
  if (button >= BTN_COUNT) {
    return false;
  }
  const uint8_t bit = static_cast<uint8_t>(1u << button);
  return (pressedMask & bit) != 0 && (prevPressedMask & bit) == 0;
}

bool HalGpio::isUsbConnected() const {
#if DT_NODE_EXISTS(USER_NODE) && DT_NODE_HAS_PROP(USER_NODE, usb_detect_gpios)
  return gpio_pin_get_dt(&usbDetect) > 0;
#else
  return false;
#endif
}
