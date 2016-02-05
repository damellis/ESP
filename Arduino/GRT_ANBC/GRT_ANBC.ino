#undef min
#undef max

#include <GRT.h>
#include "model.h"

using namespace GRT;

ANBC anbc;
istringstream iss(model);
GestureRecognitionPipeline pipeline;

//void setupPipeline(GestureRecognitionPipeline& pipeline) {
//    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
//    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 1, false, true, true, false, false));
//    pipeline.setClassifier(ANBC());
//}

float analogInputToVoltage(float val) {
    return val / 1024.0 * 3.3;
}

float voltageToAcceleration(float val) {
    return (val - 1.65) / 1.65 * 3.0;
}

float analogInputToAcceleration(float val) {
    return voltageToAcceleration(analogInputToVoltage(val));
}

void setup() {
  Serial.begin(115200);
  Serial.print("Starting");
  for (int i = 0; i < 20; i++) {
    Serial.print("."); delay(100);
  }
  Serial.println();

  pipeline.load(iss);
  Serial.println("Loaded pipeline.");
  
  vector<ANBC_Model> models = ((ANBC *)pipeline.getClassifier())->getModels();
  for (int i = 0; i < models.size(); i++) {
    Serial.print("N: "); Serial.print(models[i].N); Serial.println();
    Serial.print("classLabel: "); Serial.print(models[i].classLabel); Serial.println();
    Serial.print("trainingMu: "); Serial.print(models[i].trainingMu); Serial.println();
    Serial.print("trainingSigma: "); Serial.print(models[i].trainingSigma); Serial.println();
    Serial.print("mu: ");
    for (int j = 0; j < models[i].mu.size(); j++) {
      Serial.print(models[i].mu[j]); Serial.print(" ");
    }
    Serial.println();
    Serial.print("sigma: ");
    for (int j = 0; j < models[i].sigma.size(); j++) {
      Serial.print(models[i].sigma[j]); Serial.print(" ");
    }
    Serial.println();
    Serial.print("weights: ");
    for (int j = 0; j < models[i].weights.size(); j++) {
      Serial.print(models[i].weights[j]); Serial.print(" ");
    }
    Serial.println();
  }
}

void loop() {
  vector<double> sample(1);
  float val = analogRead(0);
  Serial.print(val); Serial.print("\t");
  val = analogInputToVoltage(val);
  Serial.print(val); Serial.print("\t");
  val = voltageToAcceleration(val);
  Serial.print(val); Serial.print("\t");
  sample[0] = val;
  if (!pipeline.predict(sample)) {
    Serial.println("Error in predict.");
  } else {
    Serial.println(pipeline.getPredictedClassLabel());
  }
  delay(10);
}
