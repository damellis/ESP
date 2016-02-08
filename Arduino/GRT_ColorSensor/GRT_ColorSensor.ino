//#include <Adafruit_TCS34725.h>

#undef min
#undef max

#include <GRT.h>
#include "pipeline.h"

using namespace GRT;

istringstream iss(pipelineText);
GestureRecognitionPipeline pipeline;

//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//void setupPipeline(GestureRecognitionPipeline& pipeline) {
//    pipeline.addPreProcessingModule(MovingAverageFilter(5, 1));
//    pipeline.addFeatureExtractionModule(TimeDomainFeatures(10, 1, 1, false, true, true, false, false));
//    pipeline.setClassifier(ANBC());
//}

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

//  if (!tcs.begin()) {
//    Serial.println("No TCS34725 found ... check your connections");
//    while (1); // halt!
//  }
}

void loop() {
  uint16_t clear, red, green, blue;

  //tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  //tcs.getRawData(&red, &green, &blue, &clear);

  //tcs.setInterrupt(true);  // turn off LED
  
  Serial.print(red); Serial.print("\t");
  Serial.print(green); Serial.print("\t");
  Serial.print(blue); // Serial.print("\t");
  // Serial.print(clear);
  Serial.println();
    
  vector<double> sample(3);
  sample[0] = red; sample[1] = green; sample[2] = blue;
  if (!pipeline.predict(sample)) {
    Serial.println("Error in predict.");
  } else {
    Serial.println(pipeline.getPredictedClassLabel());
  }
  delay(10);
}
