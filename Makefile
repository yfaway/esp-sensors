ESP_LIB = ~/.arduino15/packages/esp8266/hardware/esp8266/2.7.1/libraries
LIBS    = /home/yf/Arduino/libraries/ \
		  ${ESP_LIB}/ESP8266WiFi/ \
		  ${ESP_LIB}/Ticker
UPLOAD_PORT = /dev/ttyUSB0

include ~/git/makeEspArduino/makeEspArduino.mk
