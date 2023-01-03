/*
      DataLogger para monitorar temperatura.

      Salva dados no cartão SD.
      
 */

// --- Bibliotecas Auxiliares ---
#include <OneWire.h>                        // Biblioteca de aceso e comunicação com os sensorees de temperatura.
#include <SD.h>                             // Biblioteca cartão SD.
#include <SPI.h>                            // Biblioteca de interface de tranferência de dados e monitoramento utilizando a mesma linha de transmissão.
#include <DallasTemperature.h>              // Biblioteca para usar e coletar temperatura dos sensores.
#include <Wire.h>                           // Biblioteca para comunicar e integrar diferentes módulos entre si.
#include "RTClib.h"                         // Biblioteca para o módulo RTC(Real Time Clock).

RTC_Millis rtc;                             // Classe para usar o tempo em milisegundos.
RTC_DS1307 RTC;                             // Classe para identificar o módulo RTC.


// --- MAPEAMNETO DO HARDWARE ---
const int ONE_WIRE_BUS = 10;                // Sensores de temperatura conectado no pino analógico 10
const int chipSelect = 53;                  // Comunicação SPI, CD no pino digital 53

// Setup inicial.
OneWire ourWire(ONE_WIRE_BUS);              // Para comunicar com qualquer dispositivo OneWire.
DallasTemperature sensors(&ourWire);        // Passa a referência OneWire para DallasTEmperature.

// --- DECLARAR VARIÁVEIS ---
// Endereços e quantidade de sensores.
DeviceAddress T1 = {0x28, 0xFF, 0xDC, 0x91, 0x90, 0x16, 0x04, 0x56};
/* --- EXEMPLOS ---
DeviceAddress T2 = {0x28, 0xFF, 0x28, 0xBB, 0x90, 0x16, 0x04, 0x84};
DeviceAddress T3 = {0x28, 0xFF, 0x46, 0x9C, 0x87, 0x16, 0x03, 0x72};
*/


File logfile;               // Arquivo de registro de dados.
int incomingByte;           // Variável para o dado recebido.
float temp;                 // Variável de temperatura.
float tp[50];               // Vetor de temperaturas.
float tempC;


// --- DATALOGGER ---
#define LOG_INTERVAL 2000   // Define o intervalo entre o registro de duas medidas, neste caso 20000 ms, ou 20s.
#define SYNC_INTERVAL 10000 // Milisegundos entre a chamada para limpar e para gravar dados no cartão. 
uint32_t syncTime = 0;      // Regula o tempo com a ultima vez sincronizada com o tempo atual.
#define ECHO_TO_SERIAL 1    // Manda dados gravados no SD Card para o monitor serial.
#define WAIT_TO_START 0     // Aguarda o input no serial em setup().

void setup(void){
  Serial.begin(9600);       // Inicia a comuniação serial com baud rate de 9600.
  Serial.println();
  
  File logfile;   
  #if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
  #endif //WAIT_TO_START

  
  Serial.print("Initializing SD card...");    // Inicializa o SD card.
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(53, OUTPUT);                        // Configura pina CS como saída.
  
  // Verificca se o cartão está presente e pode ser inicializado.
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // Não faz mais nada.
    return;
  }
  Serial.println("card initialized.");
  
  // Cria um novo arquivo
  char filename[] = "DADOS00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // Apenas abre um novo arquivo se não exite outro.
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // Sai do loop.
    }
  }


  Serial.begin(9600);
  Serial.println();
  
  #if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
  #endif //WAIT_TO_START

  
  // Se o relógio não está rodando, verifica o tempo.
  if (! RTC.isrunning()) {
  Serial.println("RTC is running!");
  // following line sets the RTC to the date & time this sketch was compiled
  // RTC.adjust(DateTime(__DATE__, __TIME__));
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  }

  Wire.begin(); // Inicializa biblioteca wire, e a comunicação entre sesnores e modulo.
  RTC.begin();  // Inicializa o RTC.
  sensors.begin(); // Inicializa biblioteca de medição de temperatura..
  
  sensors.setResolution(T1, 9); // Configura a resolução da leitura em 9bit.
  /*
  sensors.setResolution(T2, 9);
  sensors.setResolution(T3, 9);
  */

}

// Imprime as temperaturas na tela.
void printTemperature(DeviceAddress deviceAddress)
{
  tempC = sensors.getTempC(deviceAddress);
  delay (100); // mudado de 10 para 100
  // Se não conseguir ler temperatura do sensor.
  if (tempC == -127.00) {
    Serial.print("Erro ao ler temperatura !");
  } else {
 
  }
}

// --- LOOP PRINCIPAL DE LEITURA --- 
void loop(void){ 
  DateTime now = rtc.now();
  //delay (600000);
  //delay (1470);
  
  sensors.requestTemperatures();                         // COMANDA TODOS OS DISPOSITIVOS PARA LER AS TEMPERATURAS
  temp = sensors.getTempCByIndex(0);                     // Selecionar mais de um sensor no mesmo pino.
  sensors.requestTemperatures();                         // Solicita temperatura de todos os sensores no barramento.
  Serial.print("\n\r");
  Serial.print("  ");
  Serial.print("Temperatura Sensor 1: ");
  printTemperature(T1);
  tp[1]=tempC;
  Serial.print(tp[1]);
  Serial.print("\n\r");
  Serial.print("  ");
  /*
  Serial.print("Temperatura Sensor 2: ");
   printTemperature(T2);
  tp[2]=tempC;
  Serial.print(tp[2]);
  Serial.print("\n\r");
  Serial.print("  ");
  Serial.print("Temperatura Sensor 3: ");
  printTemperature(T3);
  tp[3]=tempC;
  Serial.print(tp[3]);
  Serial.print("\n\r");
  Serial.print("  ");
  */
    
   

  Serial.print("\n\r\n\r");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  File dataFile = SD.open("DADOS00.txt", FILE_WRITE);

  if (dataFile) 
  dataFile.print("\n\r");
  dataFile.print(' ');
  dataFile.print("\n\r\n\r");
  dataFile.print (now.year(), DEC);
  dataFile.print('/');
  dataFile.print(now.month(), DEC);
  dataFile.print('/');
  dataFile.print(now.day(), DEC);
  dataFile.print(' ');
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
   
  dataFile.print(" ;");
  dataFile.print(tp[1]);
 
  /*
  dataFile.print(" ;");
  dataFile.print(tp[2]);
 
  dataFile.print(" ;");
  dataFile.print(tp[3]);

  */
 


  dataFile.print(" ");
  dataFile.close();
  delay (600000); // MUDAR O INTERVALO DE TEMPO DE COLETA DE DADOS DE TEMPERATURA EM MILISEGUNDOS.
 }