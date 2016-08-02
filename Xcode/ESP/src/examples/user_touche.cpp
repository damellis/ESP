/** @example user_touche.cpp
 Touche example.
 */

#include <ESP.h>

BinaryIntArraySerialStream stream(115200, 160);
GestureRecognitionPipeline pipeline;

void setup()
{
    useInputStream(stream);
    
    pipeline.setClassifier(SVM(SVM::POLY_KERNEL, SVM::C_SVC, false, true, true, 0.1, 1.0, 0, 0.5, 2));
    usePipeline(pipeline);
}