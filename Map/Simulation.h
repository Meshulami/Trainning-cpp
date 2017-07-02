#include "Map.h"
using namespace MAP;

//functions
void createAndAddVertex(const string & str, MyVertex::vertex_choice choice);
void createAndAddEdge2Dir(const string & name1, const string & name2, MyEdge::edge_choice choice1, MyEdge::edge_choice choice2, int w = 100);
void createMap(MyEdge::edge_choice edgeChoice = MyEdge::FIFO, MyVertex::vertex_choice vertexChoice = MyVertex::Dijkstra);
void showGraph();
void printTime(int time);
void createRandomPacket(int minTTL, int maxTTL, int minSize, int maxSize);
int forwarding();
int routing();
int endProgress();
void packetsLog(int numPackets, int finishedPackets, int unfinishedPackets, int inProgressPacket);
void createSimulation(int maxTime, int maxPackets, double p, int minTTL = 20, int maxTTL = 50, int minSize = 100, int maxSize = 500);