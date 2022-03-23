# device-base-esp32

This repository is a sample implementation of IoT devices using device-console.

This repository includes OTA and sensor data uploads, configuration data downloads, and peripheral control.

By using device-console, IoT devices can be controlled safely and easily.

## How to build

1. Install ESP-IDF

2. clone this repository

3. symlink `./platform/jar-garden` to `./app`

4. prepare ENV

```
$  . ./script/esp32
```

5. build

```
$ idf.py -p /dev/ttyUSB0 clean
$ idf.py -p /dev/ttyUSB0 flash monitor
```
If you make new device support. You should make `./platform/<new platform>`.

## CAUTION

This repository doesn't include main/ca.crt and  main/server.crt.
If you need an HTTPS connection, you will need to provide main/ca.crt and main/server.crt yourself.
