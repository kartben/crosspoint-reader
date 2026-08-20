#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define LOG_ERR(origin, fmt, ...) printk("ERR %s: " fmt "\n", origin, ##__VA_ARGS__)
#define LOG_INF(origin, fmt, ...) printk("INF %s: " fmt "\n", origin, ##__VA_ARGS__)
#define LOG_DBG(origin, fmt, ...) printk("DBG %s: " fmt "\n", origin, ##__VA_ARGS__)
