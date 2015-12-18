/****************************************
 * Simple Arduino + Electret Microphone
 ****************************************/

// Sample window width in uS (200 uS = 5 kHz)
const int sampleWindow = 200;
unsigned int sample;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Start of sample window
  unsigned long startMicros = micros();

  // Collects data according to sampleWindow
  while (micros() - startMicros < sampleWindow) { }

  // sample will be [0, 1023]
  sample = analogRead(0);

  // sample / 4 will be [0, 256]
  sample = sample >> 2;

  // Writes the byte through serial
  byte b = sample;
  Serial.write(b);
}
