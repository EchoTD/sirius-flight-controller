#include <MPU6500_WE.h>
#include <Wire.h>
#define MPU6500_ADDR 0x68
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);



void setup() {
  pinMode(PC13, OUTPUT);
  
  Serial.begin(115200);
  Wire.begin();
  delay(5000);
  Serial.println("void setup() is now running.");
  
  // IMU Sensörü kalibrasyonu - start
  Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
  delay(1000);
  myMPU6500.autoOffsets();
  Serial.println("Done!");
  // IMU Sensörü kalibrasyonu - end
  myMPU6500.enableGyrDLPF(true);                           // Digital Low Pass Filter | last change without testing
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
  myMPU6500.setSampleRateDivider(5);
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_1000);
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_8G);
  myMPU6500.enableAccDLPF(true);
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);
  delay(200);
  
  Serial.println("void setup() is finished, entering loop mode...");
  digitalWrite(PC13, HIGH);
  delay(3000);
  digitalWrite(PC13, LOW);
}

void loop() {
  xyzFloat gValue = myMPU6500.getGValues();             // İvmeölçer verileri
  xyzFloat gyr = myMPU6500.getGyrValues();              // Jiroskop verileri
  float temp = myMPU6500.getTemperature();              // Sıcaklık verisi
  float resultantG = myMPU6500.getResultantG(gValue);

  Serial.println("Acceleration in g (x,y,z):");
  Serial.print(gValue.x);
  Serial.print("   ");
  Serial.print(gValue.y);
  Serial.print("   ");
  Serial.println(gValue.z);
  Serial.print("Resultant g: ");
  Serial.println(resultantG);

  Serial.println("Gyroscope data in degrees/s: ");
  Serial.print(gyr.x);
  Serial.print("   ");
  Serial.print(gyr.y);
  Serial.print("   ");
  Serial.println(gyr.z);

  Serial.print("Temperature in °C: ");
  Serial.println(temp);

  Serial.println("********************************************");

  delay(25);
}
