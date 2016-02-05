char *model = "GRT_PIPELINE_FILE_V3.0\n\
PipelineMode: CLASSIFICATION_MODE\n\
NumPreprocessingModules: 1\n\
NumFeatureExtractionModules: 1\n\
NumPostprocessingModules: 0\n\
Trained: 1\n\
Info: \n\
PreProcessingModuleDatatypes:	MovingAverageFilter\n\
FeatureExtractionModuleDatatypes:	TimeDomainFeatures\n\
ClassificationModuleDatatype:	ANBC\n\
PostProcessingModuleDatatypes:\n\
PreProcessingModule_1\n\
GRT_MOVING_AVERAGE_FILTER_FILE_V1.0\n\
NumInputDimensions: 1\n\
NumOutputDimensions: 1\n\
FilterSize: 5\n\
FeatureExtractionModule_1\n\
GRT_TIME_DOMAIN_FEATURES_FILE_V1.0\n\
Trained: 0\n\
UseScaling: 0\n\
NumInputDimensions: 1\n\
NumOutputDimensions: 2\n\
NumTrainingIterationsToConverge: 3239951500\n\
MinNumEpochs: 0\n\
MaxNumEpochs: 100\n\
ValidationSetSize: 20\n\
LearningRate: 0.1\n\
MinChange: 1e-05\n\
UseValidationSet: 0\n\
RandomiseTrainingOrder: 1\n\
Initialized: 1\n\
BufferLength: 10\n\
NumFrames: 1\n\
OffsetInput: 0\n\
UseMean: 1\n\
UseStdDev: 1\n\
UseEuclideanNorm: 0\n\
UseRMS: 0\n\
GRT_ANBC_MODEL_FILE_V2.0\n\
Trained: 1\n\
UseScaling: 0\n\
NumInputDimensions: 2\n\
NumOutputDimensions: 0\n\
NumTrainingIterationsToConverge: 0\n\
MinNumEpochs: 0\n\
MaxNumEpochs: 100\n\
ValidationSetSize: 20\n\
LearningRate: 0.1\n\
MinChange: 1e-05\n\
UseValidationSet: 0\n\
RandomiseTrainingOrder: 1\n\
UseNullRejection: 0\n\
ClassifierMode: 0\n\
NullRejectionCoeff: 10\n\
NumClasses: 3\n\
NullRejectionThresholds:  0 0 0\n\
ClassLabels:  1 2 3\n\
*************_MODEL_*************\n\
Model_ID: 1\n\
N: 2\n\
ClassLabel: 1\n\
Threshold: -53.2888\n\
Gamma: 10\n\
TrainingMu: 4.91754\n\
TrainingSigma: 5.82063\n\
Mu:\n\
	0.601282	0.00441438\n\
Sigma:\n\
	0.0195889	0.0247883\n\
Weights:\n\
	1	1\n\
*************_MODEL_*************\n\
Model_ID: 2\n\
N: 2\n\
ClassLabel: 2\n\
Threshold: -28.2796\n\
Gamma: 10\n\
TrainingMu: 1.96227\n\
TrainingSigma: 3.02419\n\
Mu:\n\
	-0.0480029	0.0210273\n\
Sigma:\n\
	0.130697	0.0642641\n\
Weights:\n\
	1	1\n\
*************_MODEL_*************\n\
Model_ID: 3\n\
N: 2\n\
ClassLabel: 3\n\
Threshold: -29.9383\n\
Gamma: 10\n\
TrainingMu: 2.74099\n\
TrainingSigma: 3.26793\n\
Mu:\n\
	-0.561578	0.0149926\n\
Sigma:\n\
	0.0887719	0.0438518\n\
Weights:\n\
	1	1\n\
";
