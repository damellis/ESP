#undef min
#undef max

#include <GRT.h>

#include "data.h"

using namespace GRT;

vector<double> sample(1);

KNN knn(1);
int x = 0;

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
}

void loop() {
  x++;
  sample[0] = x;

  for (int i = 0; i < sample.size(); i++) {
    Serial.print(sample[i]);
    Serial.print(" ");
  }

  Serial.println(knn.predict(sample));

  x = x % 600;
}

