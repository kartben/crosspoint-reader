/*
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	printk("CrossPoint Zephyr port on %s\n", CONFIG_BOARD);
	return 0;
}
