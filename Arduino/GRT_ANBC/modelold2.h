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
NumTrainingIterationsToConverge: 0\n\
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
Threshold: -21.5284\n\
Gamma: 10\n\
TrainingMu: 6.64109\n\
TrainingSigma: 2.81695\n\
Mu:\n\
	1.0937	0.003349\n\
Sigma:\n\
	0.00821867	0.0153209\n\
Weights:\n\
	1	1\n\
*************_MODEL_*************\n\
Model_ID: 2\n\
N: 2\n\
ClassLabel: 2\n\
Threshold: -19.6397\n\
Gamma: 10\n\
TrainingMu: 3.35946\n\
TrainingSigma: 2.29992\n\
Mu:\n\
	0.823418	0.0125702\n\
Sigma:\n\
	0.0649069	0.0338062\n\
Weights:\n\
	1	1\n\
*************_MODEL_*************\n\
Model_ID: 3\n\
N: 2\n\
ClassLabel: 3\n\
Threshold: -17.6326\n\
Gamma: 10\n\
TrainingMu: 2.09368\n\
TrainingSigma: 1.97263\n\
Mu:\n\
	1.06599	0.264441\n\
Sigma:\n\
	0.0852988	0.0866646\n\
Weights:\n\
	1	1\n\
";
