## ~/arduino-1.8.12/hardware/espressif
## cd <ARDUINO>/hardware

## mkdir espressif
## cd espressif	|| exit 1

git clone https://github.com/espressif/arduino-esp32.git esp32	|| exit 1

cd esp32	|| exit 1
git submodule update --init

cd tools	|| exit 1
python get.py