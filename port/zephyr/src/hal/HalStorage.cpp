#include "hal/HalStorage.h"

#include "os/Logging.h"
#include "os/OsMutex.h"

#if defined(CONFIG_FILE_SYSTEM) && defined(CONFIG_FAT_FILESYSTEM_ELM)
#include <ff.h>
#include <zephyr/fs/fs.h>
#endif

HalStorage storage;

static OsMutex storageMutex;

#if defined(CONFIG_FILE_SYSTEM) && defined(CONFIG_FAT_FILESYSTEM_ELM)
static FATFS fatFs;
static fs_mount_t fatMount = {
    .type = FS_FATFS,
    .mnt_point = "/SD:",
    .fs_data = &fatFs,
};
#endif

bool HalStorage::begin() {
  OsLock lock(storageMutex);

#if defined(CONFIG_FILE_SYSTEM) && defined(CONFIG_FAT_FILESYSTEM_ELM)
  const int err = fs_mount(&fatMount);
  if (err != 0) {
    LOG_ERR("STOR", "FAT mount failed: %d", err);
    mounted = false;
    return false;
  }
  mounted = true;
  LOG_INF("STOR", "mounted %s", fatMount.mnt_point);
  return true;
#else
  LOG_INF("STOR", "SD not enabled on this board");
  mounted = false;
  return false;
#endif
}

bool HalStorage::exists(const char* path) {
  OsLock lock(storageMutex);
  if (!mounted || path == nullptr) {
    return false;
  }

#if defined(CONFIG_FILE_SYSTEM) && defined(CONFIG_FAT_FILESYSTEM_ELM)
  struct fs_dirent entry;
  return fs_stat(path, &entry) == 0;
#else
  return false;
#endif
}
