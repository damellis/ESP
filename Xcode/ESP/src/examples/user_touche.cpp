/** @example user_touche.cpp
 Touche example. See <a href="https://github.com/damellis/ESP/wiki/%5BExample%5D-Touché-swept-frequency-capacitive-sensing">documentation on the wiki</a>.
 */

#include <ESP.h>

BinaryIntArraySerialStream stream(115200, 160);
TcpOStream oStream("localhost", 5204);
GestureRecognitionPipeline pipeline;

void setup()
{
    useStream(stream);
    useOutputStream(oStream);

    pipeline.setClassifier(SVM(SVM::POLY_KERNEL, SVM::C_SVC, false, true, true, 0.1, 1.0, 0, 0.5, 2));
    usePipeline(pipeline);
    
    setTruePositiveWarningThreshold(0.90);
    setFalseNegativeWarningThreshold(0.10);
  
    useLeaveOneOutScoring(false);
}