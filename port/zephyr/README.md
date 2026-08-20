# Zephyr 4.4 port

Bring-up HAL for the Xteink X4. The EPUB reader still builds with PlatformIO.

```bash
pip install west
mkdir -p .west
cat > .west/config << 'EOF'
[manifest]
path = .
file = west.yml
EOF
west update --narrow
pip install -r deps/zephyr/scripts/requirements-base.txt
```

(`west init -l .` puts `.west` in the parent directory; this repo is the workspace.)

Host (CI):

```bash
west build -b native_sim/native/64 port/zephyr
./build/zephyr/zephyr.exe -no-rt -stop_at=0.5
```

Device (Zephyr SDK 1.0+):

```bash
west build -b xteink_x4/esp32c3 port/zephyr
```
