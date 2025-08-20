#include <Wire.h>
#include <SFM3X00.h>


// Flow Sensor Settings
#define SAMPLE_DELAY 100
#define FLOW_SENSOR_ADDRESS 0x40
SFM3X00 flowSensor(FLOW_SENSOR_ADDRESS);

// Pressure Sensor Settings
uint8_t id = 0x28; // I2C address
uint8_t data[7];
uint8_t cmd[3] = {0xAA, 0x00, 0x00};
double press_counts = 0, temp_counts = 0, pressure = 0, temperature = 0;
double outputmax = 15099494, outputmin = 1677722;
double pmax = 60, pmin = -60;
double percentage = 0;

// UART and Output
String receivedData = "";
float seconds = 0;
float flow1 = 0.0, flow2 = 0.0;

void setup() {
  Wire.begin();
  Serial.begin(9600);  // From Arduino A

  flowSensor.begin();
  delay(100);

  Serial.println("Time (s), Flow 1 (slm), Flow 2 (slm), Pressure (units), Temp (°C)");
}

void loop() {
  flow2 = flowSensor.readFlow();

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      flow1 = receivedData.toFloat();

      readPressureSensor();

      // Print CSV line
      Serial.print(seconds, 2);
      Serial.print(", ");
      Serial.print(flow1);
      Serial.print(", ");
      Serial.print(flow2);
      Serial.print(", ");
      Serial.print(pressure, 3);
      Serial.print(", ");
      Serial.println(temperature, 2);

      receivedData = "";
      seconds += SAMPLE_DELAY / 1000.0;
    } else {
      receivedData += c;
    }
  }

  delay(SAMPLE_DELAY);
}

void readPressureSensor() {
  Wire.beginTransmission(id);
  Wire.write(cmd, 3);
  Wire.endTransmission();
  delay(20);

  Wire.requestFrom(id, 7);
  for (int i = 0; i < 7; i++) {
    data[i] = Wire.read();
  }

  press_counts = data[3] + data[2] * 256 + data[1] * 65536;
  temp_counts = data[6] + data[5] * 256 + data[4] * 65536;
  temperature = (temp_counts * 200.0 / 16777215.0) - 50.0;
  percentage = (press_counts / 16777215.0) * 100.0;
  pressure = ((press_counts - outputmin) * (pmax - pmin)) / (outputmax - outputmin) + pmin;
}
