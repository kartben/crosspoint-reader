# Zephyr 4.4 port

Bring-up HAL for the Xteink X4. The EPUB reader still builds with PlatformIO.

```bash
pip install west
west init -l .
west update --narrow
pip install -r deps/zephyr/scripts/requirements-base.txt
```

Host (CI):

```bash
west build -b native_sim/native/64 port/zephyr
```

Device (Zephyr SDK 1.0+):

```bash
west build -b xteink_x4/esp32c3 port/zephyr
```
