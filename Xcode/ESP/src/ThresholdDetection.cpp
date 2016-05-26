/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial 
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ThresholdDetection.h"

namespace GRT{
    
//Register the ThresholdDetection module with the FeatureExtraction base class
RegisterFeatureExtractionModule< ThresholdDetection > ThresholdDetection::registerModule("ThresholdDetection");
    
ThresholdDetection::ThresholdDetection(UINT bufferLength,UINT numDimensions,double alpha,double beta){

    classType = "ThresholdDetection";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG ThresholdDetection]");
    errorLog.setProceedingText("[ERROR ThresholdDetection]");
    warningLog.setProceedingText("[WARNING ThresholdDetection]");
    
    init(bufferLength,numDimensions,alpha,beta);
}
    
ThresholdDetection::ThresholdDetection(const ThresholdDetection &rhs){
    
    classType = "ThresholdDetection";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG ThresholdDetection]");
    errorLog.setProceedingText("[ERROR ThresholdDetection]");
    warningLog.setProceedingText("[WARNING ThresholdDetection]");
    
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

ThresholdDetection::~ThresholdDetection(){

}
    
ThresholdDetection& ThresholdDetection::operator=(const ThresholdDetection &rhs){
    if(this!=&rhs){
        this->bufferLength = rhs.bufferLength;
        this->alpha = rhs.alpha;
        this->beta = rhs.beta;
        this->dataBuffer = rhs.dataBuffer;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool ThresholdDetection::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(ThresholdDetection*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool ThresholdDetection::computeFeatures(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    featureVector = update( inputVector );
    
    return true;
}

bool ThresholdDetection::reset(){
    if( initialized ){
        return init(bufferLength,numInputDimensions,alpha,beta);
    }
    return false;
}
    
bool ThresholdDetection::saveModelToFile(string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool ThresholdDetection::loadModelFromFile(string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool ThresholdDetection::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_THRESHOLD_DETECTION_FILE_V1.0" << endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Write the time domain settings to the file
    file << "BufferLength: " << bufferLength << endl;
    file << "Alpha: " << alpha << endl;
    file << "Beta: " << beta << endl;
    
    return true;
}

bool ThresholdDetection::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_THRESHOLD_DETECTION_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    //Load the BufferLength
    file >> word;
    if( word != "BufferLength:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read BufferLength header!" << endl;
        return false;     
    }
    file >> bufferLength;
    
    //Load the NumFrames
    file >> word;
    if( word != "Alpha:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read Alpha header!" << endl;
        return false;     
    }
    file >> alpha;
    
    //Load the OffsetInput
    file >> word;
    if( word != "Beta:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read Beta header!" << endl;
        return false;     
    }
    file >> beta;
    
    //Init the ThresholdDetection module to ensure everything is initialized correctly
    return init(bufferLength,numInputDimensions,alpha,beta);
}
    
bool ThresholdDetection::init(UINT bufferLength,UINT numDimensions,double alpha,double beta){
    
    initialized = false;
    
    this->bufferLength = bufferLength;
    this->numInputDimensions = numDimensions;
    this->alpha = alpha;
    this->beta = beta;
    this->inNoise = true;
    featureDataReady = false;
    
    //Set the number of output dimensions
    numOutputDimensions = 5 * numInputDimensions + 1;
    
    //Resize the feature vector
    featureVector.resize(numOutputDimensions);
    
    //Resize the raw data buffer
    dataBuffer.resize( bufferLength, VectorDouble(numInputDimensions,0) );

    //Flag that the time domain features has been initialized
    initialized = true;
    
    return true;
}


VectorDouble ThresholdDetection::update(double x){
	return update(VectorDouble(1,x));
}
    
VectorDouble ThresholdDetection::update(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "update(const VectorDouble &x) - Not Initialized!" << endl;
        return vector<double>();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorDouble &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return vector<double>();
    }
    
    if (inNoise) {
        //Add the new data to the data buffer
        dataBuffer.push_back( x );
    }
    
    //Only flag that the feature data is ready if the data is full
    if( dataBuffer.getBufferFilled() ){
        featureDataReady = true;
    }else featureDataReady = false;
    
    VectorDouble meanFeatures(numInputDimensions,0);
    VectorDouble stdDevFeatures(numInputDimensions,0);
    
    for(UINT n=0; n<numInputDimensions; n++){
        for(UINT i=0; i<bufferLength; i++){
            //Update the mean
            meanFeatures[n] += dataBuffer[i][n];
        }
        
        meanFeatures[n] /= bufferLength;
        
        //Update the std dev
        for(UINT i=0; i<bufferLength; i++){
            stdDevFeatures[n] += (dataBuffer[i][n]-meanFeatures[n]) * (dataBuffer[i][n]-meanFeatures[n]);
        }
        double norm = bufferLength>1 ? bufferLength-1 : 1;
        stdDevFeatures[n] = sqrt( stdDevFeatures[n]/norm );
    }
    
    // TODO(damellis): do something intelligent when numInputDimensions > 1
    for(UINT n=0; n<numInputDimensions; n++){
        if( x[n] > meanFeatures[n] + alpha * stdDevFeatures[n] ){
            inNoise = false;
        }
        if (x[n] < meanFeatures[n] + beta * stdDevFeatures[n] ){
            inNoise = true;
        }
    }
    
    //Update the features
    UINT index = 0;
    featureVector[index++] = inNoise ? 0.0 : 1.0;
    for(UINT n=0; n<numInputDimensions; n++){
        featureVector[index++] = x[n];
        featureVector[index++] = meanFeatures[n];
        featureVector[index++] = stdDevFeatures[n];
        featureVector[index++] = meanFeatures[n] + alpha * stdDevFeatures[n];
        featureVector[index++] = meanFeatures[n] + beta * stdDevFeatures[n];
    }
    
    return featureVector;
}
    
CircularBuffer< VectorDouble > ThresholdDetection::getBufferData(){
    if( initialized ){
        return dataBuffer;
    }
    return CircularBuffer< VectorDouble >();
}
    
const CircularBuffer< VectorDouble > &ThresholdDetection::getBufferData() const {
    return dataBuffer;
}
    
}//End of namespace GRT