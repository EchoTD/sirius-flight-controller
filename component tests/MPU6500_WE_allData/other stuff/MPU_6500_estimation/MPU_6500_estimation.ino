#include <Wire.h>

const int MPU_Address = 0x68;

int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, accAngleZ, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float accErrorX, accErrorY, accErrorZ, gyroErrorX, gyroErrorY, gyroErrorZ;

float elapsedTime, currentTime, previousTime;
int c = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();                             // Initialize comunication
  Wire.beginTransmission(MPU_Address);      // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                         // Talk to the register 6B
  Wire.write(0x00);                         // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);               // end the transmission
  // Yukarıdaki 6 satır kod MPU6500 sensörünü uyandırıyor 

  Wire.beginTransmission(MPU_Address);
  Wire.write(0x1B);                         // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                         // Set the register bits as 00010000 [00 = ±250dps 01= ±500dps 10 = ±1000dps 11 = ±2000dps]
  Wire.endTransmission(true);
  // Yukarıdaki 4 satır kod MPU6500 sensörünün jiroskop çözünürlük ayarını belirtiyor 
  
  Wire.beginTransmission(MPU_Address);
  Wire.write(0x1C);                         // Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                         // Set the register bits as 00010000 [±2g (00), ±4g (01), ±8g (10), ±16g (11)]
  Wire.endTransmission(true);
  // Yukarıdaki 4 satır kod MPU6500 sensörünün ivmeölçer çözünürlük ayarını belirtiyor // 
  
  calculate_IMU_error();
  delay(20);

}

void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU_Address);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_Address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  
  //For a range of +-8g, we need to divide the raw values by 4096, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 4096.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 4096.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 4096.0; // Z-axis value
  
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - accErrorX; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + accErrorY; // AccErrorY ~(-1.58)
  
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  
  Wire.beginTransmission(MPU_Address);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_Address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  
  GyroX = (Wire.read() << 8 | Wire.read()) / 32.8; // For a 1000deg/s range we have to divide first the raw value by 32.8, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 32.8;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 32.8;
  
  // Correct the outputs with the calculated error values
  GyroX = GyroX + gyroErrorX; // GyroErrorX ~(-0.56)
  GyroY = GyroY - gyroErrorY; // GyroErrorY ~(2)
  GyroZ = GyroZ + gyroErrorZ; // GyroErrorZ ~ (-0.8)
  
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);

}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 2000 times
  while (c < 2000) {
    Wire.beginTransmission(MPU_Address);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_Address, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 4096.0 ;
    
    // Sum all readings
    accErrorX = accErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    accErrorY = accErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  
  //Divide the sum by 200 to get the error value
  accErrorX = accErrorX / 2000;
  accErrorY = accErrorY / 2000;
  c = 0;
  
  // Read gyro values 200 times
  while (c < 2000) {
    Wire.beginTransmission(MPU_Address);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_Address, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    
    // Sum all readings
    gyroErrorX = gyroErrorX + (GyroX / 32.8);
    gyroErrorY = gyroErrorY + (GyroY / 32.8);
    gyroErrorZ = gyroErrorZ + (GyroZ / 32.8);
    c++;
  }
  
  //Divide the sum by 200 to get the error value
  gyroErrorX = gyroErrorX / 2000;
  gyroErrorY = gyroErrorY / 2000;
  gyroErrorZ = gyroErrorZ / 2000;
  
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(accErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(accErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(gyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(gyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(gyroErrorZ);
}
