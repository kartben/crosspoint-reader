#include "hal/HalSystem.h"

#include <zephyr/sys/reboot.h>

void HalSystem::reboot() { sys_reboot(SYS_REBOOT_COLD); }
