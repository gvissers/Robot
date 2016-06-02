BOARD_TAG     = uno
MONITOR_PORT  = /dev/ttyACM0

ARDUINO_LIBS  = Adafruit_Sensor EEPROM Wire

CXXFLAGS = -std=c++11

include /usr/share/arduino/Arduino.mk
