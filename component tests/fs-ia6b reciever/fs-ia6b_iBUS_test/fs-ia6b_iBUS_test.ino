// Include iBusBM Library
#include <IBusBM.h>
 
// Create iBus Object
IBusBM ibus;
 
// Read the number of a given channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}
 
/* Read the channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}
*/
 
void setup() {
  // Start serial monitor
  Serial.begin(115200);
 
  // Attach iBus object to serial port
  ibus.begin(Serial1);
}
 
void loop() {
 
  // Cycle through first 5 channels and determine values
  // Print values to serial monitor
  // Note IBusBM library labels channels starting with "0"
 
  for (byte i = 0; i < 6; i++) {
    int value = readChannel(i, 1000, 2000, 1500);
    Serial.print("Ch");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(value);
    Serial.println(" | ");
  }
 /*
  // Print channel 6 (switch) boolean value
  Serial.print("Ch6: ");
  Serial.print(readSwitch(5, false));
  Serial.println();
 
  delay(10);
  */
}
