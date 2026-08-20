#pragma once

#include <zephyr/kernel.h>

class OsMutex {
  struct k_mutex mutex{};

 public:
  OsMutex() { k_mutex_init(&mutex); }

  OsMutex(const OsMutex&) = delete;
  OsMutex& operator=(const OsMutex&) = delete;

  void lock() { k_mutex_lock(&mutex, K_FOREVER); }
  void unlock() { k_mutex_unlock(&mutex); }
};

class OsLock {
  OsMutex& mutex;

 public:
  explicit OsLock(OsMutex& mutex) : mutex(mutex) { mutex.lock(); }
  ~OsLock() { mutex.unlock(); }

  OsLock(const OsLock&) = delete;
  OsLock& operator=(const OsLock&) = delete;
};
