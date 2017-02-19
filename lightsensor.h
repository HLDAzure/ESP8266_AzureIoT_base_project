#define LIGHTSENSOR_PIN A0

void initLightSensor() {
  pinMode(A0, INPUT );
}


int getLightSensorRaw() {
  return analogRead(A0);
}

int getLightSensorPercent() {
  return int(getLightSensorRaw() / 1023.0 * 100);
}

