#undef min
#undef max

#include <GRT.h>

#include "data.h"

using namespace GRT;

const int WINDOW = 32;

int vinpin = A0;
int voutpin = A1;
int gndpin = A2;
int zpin = A3;
int ypin = A4;
int xpin = A5;

int xvals[WINDOW], yvals[WINDOW], zvals[WINDOW], valIndex = 0;
vector<double> sample(6);

KNN knn(1);

float mean(int vals[], int num)
{
  float total = 0;
  for (int i = 0; i < num; i++) total += vals[i];
  return total / num;
}

float stddev(int vals[], int num)
{
  float m = mean(vals, num);
  float s = 0;

  for (int i = 0; i < num; i++) s += (vals[i] - m) * (vals[i] - m);

  return sqrt(s / num);
}

void setup() {
  Serial.begin(115200);

  ClassificationData cd(DIM);
  knn.enableNullRejection( false );
  vector<double> s(DIM);
  for (int i = 0; i < NUM; i++) {
    for (int j = 0; j < DIM; j++) s[j] = data[i][j];
    cd.addSample(classes[i], s);
  }
  knn.train(cd);
  knn.saveModelToStream(cout);
  
  pinMode(vinpin, OUTPUT); digitalWrite(vinpin, HIGH);
  pinMode(gndpin, OUTPUT); digitalWrite(gndpin, LOW);
  pinMode(voutpin, INPUT);
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);
}

void loop() {
  xvals[valIndex] = analogRead(xpin) / 5.0 * 3.3; // data in data.h was collected at 5V, we're running at 3.3V
  yvals[valIndex] = analogRead(ypin) / 5.0 * 3.3;
  zvals[valIndex] = analogRead(zpin) / 5.0 * 3.3;
  valIndex++;

  if (valIndex == WINDOW) {
    sample[0] = mean(xvals, WINDOW); sample[1] = mean(yvals, WINDOW); sample[2] = mean(zvals, WINDOW);
    sample[3] = stddev(xvals, WINDOW); sample[4] = stddev(yvals, WINDOW); sample[5] = stddev(zvals, WINDOW);

    for (int i = 0; i < sample.size(); i++) {
      Serial.print(sample[i]);
      Serial.print(" ");
    }

    knn.predict(sample);

    Serial.println(knn.getPredictedClassLabel());
    
    valIndex = 0;
  }
}

