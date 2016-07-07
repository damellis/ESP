/** @example user_touche.cpp
 Touche example.
 */

#include <ESP.h>

BinaryIntArraySerialStream stream(0, 115200, 160);
GestureRecognitionPipeline pipeline;

void setup()
{
    useInputStream(stream);
    
    pipeline.addFeatureExtractionModule(TimeseriesBuffer(1, 160));
    usePipeline(pipeline);
}