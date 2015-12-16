#define DIM 3

void setupPipeline()
{
	pipeline.addFeatureExtractionModule(TimeDomainFeatures(100, 1, DIM, false, true, true, false, false));
	pipeline.setClassifier(KNN(1));
}