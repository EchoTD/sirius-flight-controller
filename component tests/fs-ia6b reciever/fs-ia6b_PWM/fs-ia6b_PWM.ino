/*
  FS-IA6B 6 kanallı 2.4gHz alıcı test kodu
*/

// Aşağıdaki kod kullanacağımız kanalları STM32 geliştirici kartındaki uygun pinlere atama yapıyor.
#define CH1 PB1                                                                           
#define CH2 PB0
#define CH3 PA7
#define CH4 PA6
#define CH5 PA3
#define CH6 PA2

// Kanallardan gelen değerlerin integer atamaları.
int ch1Value, ch2Value, ch3Value, ch4Value, ch5Value, ch6Value;


int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

void setup() {
  // Set up serial monitor
  Serial.begin(115200);
  
  // Set all pins as inputs
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
  
}

void loop() {
  // Get values for each channel
  ch1Value = readChannel(CH1, 1000, 2000, 1500);
  ch2Value = readChannel(CH2, 1000, 2000, 1500);
  ch3Value = readChannel(CH3, 1000, 2000, 1500);
  ch4Value = readChannel(CH4, 1000, 2000, 1500);
  ch5Value = readChannel(CH5, 1000, 2000, 1500);
  ch6Value = readChannel(CH6, 1000, 2000, 1500);
  
  // Print to Serial Monitor
  Serial.print("Ch1: ");
  Serial.print(ch1Value);
  Serial.print(" | Ch2: ");
  Serial.print(ch2Value);
  Serial.print(" | Ch3: ");
  Serial.print(ch3Value);
  Serial.print(" | Ch4: ");
  Serial.print(ch4Value);
  Serial.print(" | Ch5: ");
  Serial.print(ch5Value);
  Serial.print(" | Ch6: ");
  Serial.println(ch6Value);
  

}
