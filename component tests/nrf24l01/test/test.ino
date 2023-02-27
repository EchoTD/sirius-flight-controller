/*
Radio    Arduino
CE    -> 9  (PA0)
CSN   -> 10 (PA4)  (Hardware SPI SS)
MOSI  -> 11 (PA7)  (Hardware SPI MOSI)
MISO  -> 12 (PA6)  (Hardware SPI MISO)
SCK   -> 13 (PA5)  (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
RF24 radio(PA0, PA4); // CE, CSN on Blue Pill 
const byte address[6] = "00001"; 
 
void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
 
void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
}



///*
//#include <SPI.h>
//#include <nRF24L01.h>
//#include <RF24.h>
//
//RF24 radio(PA0, PA4); 
//const byte address[6] = "00001"; 
//
//struct joyData{
//  byte throttle;
//  byte yaw;
//  byte pitch;
//  byte roll;
//  };
//  
//struct joyData data;
//
//void resetData(){
//  data.throttle = 0;
//  data.yaw = 127;
//  data.pitch = 127;
//  data.roll = 127;
//  }
//
//void setup(){
//  Serial.begin(115200);
//  radio.begin();
//  radio.openWritingPipe(address);
//  radio.setPALevel(RF24_PA_MIN);
//  radio.stopListening();
//
//  resetData();
//  }
//
//int mapJoyValue(int val, int lower, int middle, int upper, bool reverse){
//  val = constrain(val, lower, upper);
//  if(val < middle)
//    val = map(val, lower, middle, 0, 128);
//  else
//    val = map(val, middle, upper, 128, 255);
//  return(reverse ? 255 - val : val);
//  }
//  
//void loop(){
//  
//  data.throttle = mapJoyValue(analogRead(PA10), 13, 524, 1015, true);
//  data.yaw = mapJoyValue(analogRead(PA11), 1, 505, 1020, true);
//  data.pitch = mapJoyValue(analogRead(PA12), 12, 544, 1021, true);
//  data.roll = mapJoyValue(analogRead(PA13), 34, 522, 1020, true);
// 
//  radio.write(&data, sizeof(joyData)); 
//  
//  }
//*\
