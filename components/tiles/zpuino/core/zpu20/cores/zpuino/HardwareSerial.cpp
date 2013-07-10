#include "HardwareSerial.h"

HardwareSerial Serial(1); /* 1st instance/slot */

void HardwareSerial::begin_slow(const unsigned int baudrate) {
	REG(1) = BAUDRATEGEN(baudrate)|BIT(UARTEN);
}

void HardwareSerial::flush() {
	while (REG(1) & 4);
}

size_t HardwareSerial::write(unsigned char c) {
	while ((REG(1) & 2)==2);
	REG(0) = c;
	return 1;
}
