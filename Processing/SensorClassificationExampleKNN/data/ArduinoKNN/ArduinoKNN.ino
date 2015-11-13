#include "data.h"

const int WINDOW = 32;

int vinpin = A0;
int voutpin = A1;
int gndpin = A2;
int zpin = A3;
int ypin = A4;
int xpin = A5;

int xvals[WINDOW], yvals[WINDOW], zvals[WINDOW], index = 0;
float sample[DIM];

void setup() {
  Serial.begin(115200);
  pinMode(vinpin, OUTPUT); digitalWrite(vinpin, HIGH);
  pinMode(gndpin, OUTPUT); digitalWrite(gndpin, LOW);
  pinMode(voutpin, INPUT);
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);
}

void loop() {
  xvals[index] = analogRead(xpin);
  yvals[index] = analogRead(ypin);
  zvals[index] = analogRead(zpin);
  index++;

  if (index == WINDOW) {
    sample[0] = mean(xvals, WINDOW); sample[1] = mean(yvals, WINDOW); sample[2] = mean(zvals, WINDOW);
    sample[3] = stddev(xvals, WINDOW); sample[4] = stddev(yvals, WINDOW); sample[5] = stddev(zvals, WINDOW);

    Serial.println(predict(sample));
    
    index = 0;
  }
}
