/*
 * Alaaddin Can Gürsoy | Başlangıç tarihi: 04.11.2022 | Bitiş tarihi: -
 * MPU6500 sensörü için IMU kütüphanesi
 * Esinlenen kaynak: http://www.brokking.net/imu.html
 */

/*
 * Çıkan verileri doğru değil, nedeni bulunacak
 * MPU9250 hazır kütüphane yardımı ile kullanılabilinir.
 * raw datayı incele bi
 */

#include<Wire.h>

int16_t acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;      // Sensörden gelen raw data
long gyro_x_cal, gyro_y_cal, gyro_z_cal, acc_total_vector;
float angle_pitch, angle_roll, angle_pitch_acc, angle_roll_acc;
float angle_pitch_output, angle_roll_output;
int angle_pitch_buffer, angle_roll_buffer;
boolean set_gyro_angles;
long loop_timer;
int c = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();             // I2C protokolünü çalıştır
  Wire.setClock(250);       // I2C frekansını 250Hz yap. (TEST AMAÇLI, SONRA YÜKSELTİLMELİ)
  setup_mpu_6500();         // Kodun en altında işlevi belirtili
  delay(100);
  
  // Aşağıdaki while loop, sensörün ilk çalıştırıldığı durumuna göre offset verilerini çıkartıyor.
  while( c < 2000){         
    read_mpu_6500();
    gyro_x_cal += gyro_x;
    gyro_y_cal += gyro_y;
    gyro_z_cal += gyro_z;
    delay(3);
    c++;
    }
    
  gyro_x_cal /= 2000;
  gyro_y_cal /= 2000;
  gyro_z_cal /= 2000;

  loop_timer = micros();   //

}

void loop() {

  read_mpu_6500();

  gyro_x -= gyro_x_cal;
  gyro_y -= gyro_y_cal;
  gyro_z -= gyro_z_cal;

// Jiroskop açı hesaplamaları
  //0.0000122 = (1 / 250Hz) / 32.8
  angle_pitch += gyro_x * 0.0000122;                                   
  angle_roll += gyro_y * 0.0000122;
  //0.000002128 = 0.0000122 * (3.142(PI) / 180degr) The Arduino sin function is in radians
  angle_pitch += angle_roll * sin(gyro_z * 0.000002128);               
  angle_roll -= angle_pitch * sin(gyro_z * 0.000002128);

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));  //Calculate the total accelerometer vector
  //57.296 = 1 / (3.142 / 180) The Arduino asin function is in radians
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;       //Calculate the pitch angle
  angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;       //Calculate the roll angle
  
  //Place the MPU-6050 spirit level and note the values in the following two lines for calibration
  angle_pitch_acc -= 0.0;                                              //Accelerometer calibration value for pitch
  angle_roll_acc -= 0.0;                                               //Accelerometer calibration value for roll

   if(set_gyro_angles){                                                 //If the IMU is already started
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
  }
  else{                                                                //At first start
    angle_pitch = angle_pitch_acc;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    angle_roll = angle_roll_acc;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }

  //To dampen the pitch and roll angles a complementary filter is used
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;   //Take 90% of the output pitch value and add 10% of the raw pitch value
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;      //Take 90% of the output roll value and add 10% of the raw roll value

  Serial.print("Pitch = ");
  Serial.print(angle_pitch);
  Serial.print(" Roll = ");
  Serial.println(angle_roll);
   
  while(micros() - loop_timer < 4000);                                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
  loop_timer = micros(); 
}

void read_mpu_6500(){
  Wire.beginTransmission(0x68);     // Sensör ile iletişime başla
  Wire.write(0x3B);                 // İlk ivmeölçer adresini seç
  Wire.endTransmission();           // İletişimi kes
  Wire.requestFrom(0x68, 6);         // İlk ivmeölçer adresi ile önündeki toplam 6 adresin değerleini iste
  //while(Wire.available()<6);
  acc_x = Wire.read() << 8 | Wire.read();
  acc_y = Wire.read() << 8 | Wire.read();
  acc_z = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(0x68);     // Sensör ile iletişime başla
  Wire.write(0x43);                 // İlk jiroskop adresini seç
  Wire.endTransmission();           // İletişimi kes
  Wire.requestFrom(0x68, 6);         // İlk jiroskop adresi ile önündeki toplam 6 adresin değerleini iste
  //while(Wire.available()<6);
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();
}

// Alttaki komut(lar), setup kısmında bir kere çalıştırılarak sensörü uyandırıyor ve seçtiğimiz ayarları uyguluyor.
void setup_mpu_6500(){

// Alttaki 4 satır kod, sensörü kullanıma hazır hale getiriyor.
  Wire.beginTransmission(0x68);     // Sensör ile iletişime başla
  Wire.write(0x6B);                 // 0x6B adresi sensörü uyandırma adresi, bu adresi değiştirmek için hazırla
  Wire.write(0x00);                 // Üstteki adrese yaz
  Wire.endTransmission();           // İletişimi kes

// Alttaki 4 satır kod, ivmeölçer ayarlarını belirtiyor.
  Wire.beginTransmission(0x68);     // Sensör ile iletişime başla
  Wire.write(0x1C);                 // 0x1C adresi ivmeölçer ayarlama adresidir
  Wire.write(0x10);                 // ±2g (0x00), ±4g (0x01), ±8g (0x10), ±16g (0x11)
  Wire.endTransmission();           // İletişimi kes

// Alttaki 4 satır kod, jiroskop ayarlarını belirtiyor.
  Wire.beginTransmission(0x68);     // Sensör ile iletişime başla
  Wire.write(0x1C);                 // 0x1C adresi ivmeölçer ayarlama adresidir
  Wire.write(0x10);                 // ±250dps (0x00), ±500dps (0x01), ±1000dps (0x10), ±2000dps (0x11)
  Wire.endTransmission();           // İletişimi kes
  
}
