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

unsigned long lastRecieveTime = 0;
unsigned long currentTime = 0;

// Radyo setup komutları:
#define CE PA0
#define CSN PA4
RF24 radio(CE, CSN);
const byte address[6] = "00001"; // !!! Sonra değiştir bunu
 
// Alınacak olan verinin kodu
// NRF24L01 maksimum 32 byte gönderebiliyor, 1 byte = 8 bit, 8 bit max değer = 255
struct payload{
  byte thrust;
  byte yaw;
  byte pitch;
  byte roll;
//  byte switchState1; // For arming
//  byte switchState2; // For mission mechanism
//
};

struct payload data;

void resData(){
  // Başlangıç ve basit failsafe için değerleri atama
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
  radio.openReadingPipe(0, address);
  radio.startListening();              
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
    Serial.println("Setup is done! Going into loop...");
    delay(500);
    digitalWrite(PC13, LOW);
    Serial.println("Setup is done! Going into loop...");
    delay(500);
  }
}
 
void loop() {
  currentTime = millis();
  
  // Basit bir failsafe
  if(currentTime - lastRecieveTime > 1000){
    resData();
    }

  if(radio.available()){
    radio.read(&data, sizeof(payload));
    lastRecieveTime = millis();

    Serial.print("Throttle = ");
    Serial.print(data.thrust);
    Serial.print(" Yaw = ");
    Serial.print(data.yaw);
    Serial.print(" Pitch = ");
    Serial.print(data.pitch);
    Serial.print(" Roll = ");
    Serial.println(data.roll);
    }
  
  delay(100);
}
