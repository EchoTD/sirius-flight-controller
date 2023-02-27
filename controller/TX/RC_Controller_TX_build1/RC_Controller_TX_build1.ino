/*
Radio  | STM32
CE    -> (PA0)
CSN   -> (PA4)  (Hardware SPI SS)
MOSI  -> (PA7)  (Hardware SPI MOSI)
MISO  -> (PA6)  (Hardware SPI MISO)
SCK   -> (PA5)  (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND
*/

/* YAPILACAKLAR LİSTESİ, EKLENMESİ GEREKEN ÖZELLİKLER
-Arm switch
-Otopilota geçiş switch
-Failsafe
-Troubleshooting için bloklar
-Buzzer ekle


*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int i = 0;

// PA10, PA11, PA12, PA13 girişleri analog joystick girişleri
// Dijital pin girişleri:
#define button1 PA14
#define button2 PA15

// Radyo setup komutları:
#define CE PA0
#define CSN PA4
RF24 radio(CE, CSN);
const byte address[6] = "00001"; // !!! Sonra değiştir bunu
 
// Gönderilecek olan verinin kodu
struct payload{
  byte thrust;
  byte yaw;
  byte pitch;
  byte roll;
//  byte buttonState1;
//  byte buttonState2;
};

struct payload data;

void getInputSignal(){
  // Analog sinyaller
  data.thrust = map(analogRead(PB0), 0, 1023, 0, 255);
  data.yaw = map(analogRead(PB1), 0, 1023, 0, 255);
  data.pitch = map(analogRead(PB14), 0, 1023, 0, 255);
  data.roll = map(analogRead(PB13), 0, 1023, 0, 255);

}

void resData(){
  // Başlangıç değerleri atama
  data.thrust = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
//  data.buttonState1 = 0;
//  data.buttonState2 = 0;
}
 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Radyo çalıştırma ve ayarlama komutları, daha fazlası için: https://nrf24.github.io/RF24/classRF24.html
  radio.begin();
  radio.openWritingPipe(address);
  radio.stopListening();              // Verici olduğu için alma fonksiyonunu kapatıyor
  delay(100);

  // Pin modları ayarlama
//  pinMode(button1, INPUT);
//  pinMode(button2, INPUT);
  pinMode(PC13, OUTPUT); // built-in led

  resData();

  Serial.println("Setup is done! Going into loop...");
  
  // Loop döngüsüne girmeden dışarıya bilgi vermek amaçlı
  for(i = 0; i < 3; i++){
    digitalWrite(PC13, HIGH);
    delay(500);
    digitalWrite(PC13, LOW);
    delay(500);
  }
}
 
void loop() {
  // Kumanda üzerindeki komponentlerin sinyallerini çağır
  getInputSignal();

  Serial.print("Throttle = ");
  Serial.print(data.thrust);
  Serial.print(" Yaw = ");
  Serial.print(data.yaw);
  Serial.print(" Pitch = ");
  Serial.print(data.pitch);
  Serial.print(" Roll = ");
  Serial.println(data.roll);

  radio.write(&data, sizeof(payload));
  delay(100);
}
