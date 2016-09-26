#include <ESP.h>

GDPStream input("edu.berkeley.eecs.bid.mellis.arduino101");
GestureRecognitionPipeline pipeline;

void setup()
{
    useInputStream(input);
    usePipeline(pipeline);
}
