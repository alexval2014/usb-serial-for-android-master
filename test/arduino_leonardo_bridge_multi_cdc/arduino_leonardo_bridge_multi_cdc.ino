/*
  Преобразование USB-serial в аппаратный последовательный порт (bridge USB-serial to hardware-serial).
  для Arduino на базе ATmega32U4 (Leonardo и совместимых Pro Micro, микропроцессоров)
  аппаратный последовательный порт настраивается с учетом скорости передачи данных, битов данных, стоп-битов, четности при передаче по USB
  смотрите https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr/cores/arduino 
  -> CDC.cpp/HardwareSerial.cpp подробности о последовательной реализации

  эта программа в основном предназначен для демонстрации / тестирования
  производительности связи CDC, поскольку реальный usb-последовательный мост был бы неприемлем, поскольку каждый байт передается в отдельном USB-пакете
  
*/

uint32_t baud = 9600;
uint8_t databits = 8;
uint8_t stopbits = 1;
uint8_t parity = 0;

void setup() {
  Serial.begin(baud); // USB
  Serial1.begin(baud, SERIAL_8N1);
}

void loop() {
  // show USB connected state
  if (Serial) TXLED1;
  else        TXLED0;

  // configure hardware serial
  if (Serial.baud() != baud ||
      Serial.numbits() != databits ||
      Serial.stopbits() != stopbits ||
      Serial.paritytype() != parity) {
    baud = Serial.baud();
    databits = Serial.numbits();
    stopbits = Serial.stopbits();
    parity = Serial.paritytype();
    uint8_t config = 0; // ucsrc register
    switch (databits) {
      case 5: break;
      case 6: config |= 2; break;
      case 7: config |= 4; break;
      case 8: config |= 6; break;
      default: config |= 6;
    }
    switch (stopbits) {
      case 2: config |= 8;
        // 1.5 stopbits not supported
    }
    switch (parity) {
      case 1: config |= 0x30; break; // odd
      case 2: config |= 0x20; break; // even
        // mark, space not supported
    }
    Serial1.end();
    Serial1.begin(baud, config);
  }

  // bridge
  if (Serial.available() > 0)
    Serial1.write(Serial.read());
  if (Serial1.available() > 0)
    Serial.write(Serial1.read());
}
