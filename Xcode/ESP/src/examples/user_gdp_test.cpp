#include <ESP.h>

GDPStream input("edu.berkeley.eecs.swarmlab.device.c098e5900019");
GestureRecognitionPipeline pipeline;

void setup()
{
    useInputStream(input);
    usePipeline(pipeline);
}