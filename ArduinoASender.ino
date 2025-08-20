#include <Wire.h>
#include <SFM3X00.h>

#define SAMPLE_DELAY 100
#define FLOW_SENSOR_ADDRESS 0x40

SFM3X00 flowSensor(FLOW_SENSOR_ADDRESS);

void setup() {
  Wire.begin();
  Serial.begin(9600);  // UART to Arduino B

  flowSensor.begin();
  delay(100);
}

void loop() {
  float flow = flowSensor.readFlow();

  if (flowSensor.checkRange(flow)) {
    Serial.println("ERR");
  } else {
    Serial.println(flow);  // Send only the flow value
  }

  delay(SAMPLE_DELAY);
}


