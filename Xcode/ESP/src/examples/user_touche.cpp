/** @example user_touche.cpp
 Touche example. See <a href="https://github.com/damellis/ESP/wiki/%5BExample%5D-Touché-swept-frequency-capacitive-sensing">documentation on the wiki</a>.
 */

#include <ESP.h>

BinaryIntArraySerialStream stream(115200, 160);
GestureRecognitionPipeline pipeline;

void setup()
{
    useStream(stream);
    
    pipeline.setClassifier(SVM(SVM::POLY_KERNEL, SVM::C_SVC, false, true, true, 0.1, 1.0, 0, 0.5, 2));
    usePipeline(pipeline);
    
    useLeaveOneOutScoring(false);
}