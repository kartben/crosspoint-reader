#pragma once

class HalStorage {
 public:
  bool begin();
  bool ready() const { return mounted; }
  bool exists(const char* path);

 private:
  bool mounted = false;
};

extern HalStorage storage;
