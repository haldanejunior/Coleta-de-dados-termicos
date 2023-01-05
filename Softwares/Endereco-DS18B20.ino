// Programa : Endereço DS18B20
// Este programa procura pelos sensores no circuito, mostrando o N° de endereço físico 
// pertencente ao sensor no Serial Monitor (Ctrl + Shift + M).

#include <OneWire.h> // Biblioteca necessaria a instalação: OneWire by Paul Stoffregen e a biblioteca DallasTemperature by Miles Burton.
// Conecte o cabo GND (geralmente preto) do sensor ao pino GND no barramento POWER da Placa.
// Conecte o cabo VCC (geralmente vermelho ou cinza) do sensor ao pino 5v no barramento POWER da Placa.
// Conecte o cabo DATA (geralmente branco ou amarelo) do sensor ao pino PWM 10 da Placa.
OneWire  ds(10);                  
void setup(void) {
  Serial.begin(9600);
  discoverOneWireDevices();
}

void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  Serial.print("Procurando dispositivos DS18B20...\n\r");
  while (ds.search(addr)) {
    Serial.print("\n\rEncontrado dispositivo \'DS18B20\' com endereco:\n\r");
    for ( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC nao e valido!\n");
      return;
    }
  }
  Serial.print("\n\r\n\rFinal da verificacao.\r\n");
  ds.reset_search();
  return;
}

void loop(void) {
}