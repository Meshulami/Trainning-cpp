#include "Simulation.h"

//functions
void createAndAddVertex(const string & str, MyVertex::vertex_choice choice)
{
	MyGraph * g = MyGraph::getTheGraph();
	MyVertex * node = MyVertex::vertexFactory(str, choice);
	g->addVertex(*node);
}

void createAndAddEdge2Dir(const string & name1, const string & name2, MyEdge::edge_choice choice1, MyEdge::edge_choice choice2, int w)
{
	MyGraph * g = MyGraph::getTheGraph();
	MyEdge * arc = MyEdge::edgeFactory(name1, name2, w, choice1);
	g->getVertex(name1)->addEdge(*arc);
	MyEdge * arc2 = MyEdge::edgeFactory(name2, name1, w, choice2);
	g->getVertex(name2)->addEdge(*arc2);
}

void createMap(MyEdge::edge_choice edgeChoice, MyVertex::vertex_choice vertexChoice)
{
	createAndAddVertex("A", vertexChoice);
	createAndAddVertex("B", vertexChoice);
	createAndAddVertex("C", vertexChoice);
	createAndAddVertex("D", vertexChoice);
	createAndAddEdge2Dir("A", "B", edgeChoice, edgeChoice, 60);
	createAndAddEdge2Dir("B", "C", edgeChoice, edgeChoice, 160);
	createAndAddEdge2Dir("C", "D", edgeChoice, edgeChoice, 140);
	createAndAddEdge2Dir("D", "A", edgeChoice, edgeChoice, 100);
	/*createAndAddVertex("A", vertexChoice);
	createAndAddVertex("B", vertexChoice);
	createAndAddVertex("C", vertexChoice);
	createAndAddVertex("D", vertexChoice);
	createAndAddVertex("E", vertexChoice);
	createAndAddVertex("F", vertexChoice);
	createAndAddVertex("G", vertexChoice);
	createAndAddVertex("H", vertexChoice);
	createAndAddVertex("I", vertexChoice);
	createAndAddVertex("J", vertexChoice);
	createAndAddVertex("K", vertexChoice);
	createAndAddVertex("L", vertexChoice);
	createAndAddVertex("M", vertexChoice);
	createAndAddEdge2Dir("A", "B", edgeChoice, edgeChoice, 60);
	createAndAddEdge2Dir("A", "C", edgeChoice, edgeChoice);
	createAndAddEdge2Dir("B", "D", edgeChoice, edgeChoice, 120);
	createAndAddEdge2Dir("B", "E", edgeChoice, edgeChoice, 240);
	createAndAddEdge2Dir("C", "D", edgeChoice, edgeChoice);
	createAndAddEdge2Dir("C", "G", edgeChoice, edgeChoice, 40);
	createAndAddEdge2Dir("D", "F", edgeChoice, edgeChoice, 180);
	createAndAddEdge2Dir("E", "I", edgeChoice, edgeChoice, 140);
	createAndAddEdge2Dir("F", "H", edgeChoice, edgeChoice, 160);
	createAndAddEdge2Dir("F", "I", edgeChoice, edgeChoice, 60);
	createAndAddEdge2Dir("G", "H", edgeChoice, edgeChoice, 60);
	createAndAddEdge2Dir("H", "M", edgeChoice, edgeChoice);
	createAndAddEdge2Dir("I", "J", edgeChoice, edgeChoice, 200);
	createAndAddEdge2Dir("I", "K", edgeChoice, edgeChoice);
	createAndAddEdge2Dir("J", "L", edgeChoice, edgeChoice);
	createAndAddEdge2Dir("K", "L", edgeChoice, edgeChoice, 300);
	createAndAddEdge2Dir("L", "M", edgeChoice, edgeChoice, 80);*/
}

void showGraph()
{
	MyGraph * g = MyGraph::getTheGraph();
	g->show();
}

void printTime(int time)
{
	string s = "--------------------";
	std::cout << s << "Time = " << time << s << std::endl;
}

void createRandomPacket(int minTTL, int maxTTL, int minSize, int maxSize)
{
	MyGraph * g = MyGraph::getTheGraph();
	int numVertex1 = rand() % (g->getNumVertexes());
	int numVertex2 = (((rand() % (g->getNumVertexes() - 1)) + 1) + numVertex1) % (g->getNumVertexes());
	string name1 = g->getVertexName(numVertex1);
	string name2 = g->getVertexName(numVertex2);
	int size = (minSize + rand() % (maxSize - minSize + 1));
	int ttl = (minTTL + rand() % (maxTTL - minTTL + 1));
	Packet * pack = new Packet(name1, name2, size, ttl);
	std::cout << "  *** Send a new packet from \"" << name1 << "\" to \"" << name2 << "\" ***" << std::endl << std::endl;
	g->getVertex(name1)->addPacketToQueue(*pack);
}

int forwarding()
{
	MyGraph * g = MyGraph::getTheGraph();
	MyVertex * vertex;
	int unfinishedPackets = 0;
	for (int i = 0; i < g->getNumVertexes(); i++)
	{
		vertex = g->getVertex(i);
		unfinishedPackets += vertex->updateTimeAndForward();
	}
	return unfinishedPackets;
}

int routing()
{
	MyGraph * g = MyGraph::getTheGraph();
	MyVertex * vertex;
	int finishedPackets = 0;
	for (int i = 0; i < g->getNumVertexes(); i++)
	{
		vertex = g->getVertex(i);
		finishedPackets += vertex->routingQueue();
	}
	return finishedPackets;
}

int endProgress()
{
	std::cout << "Simulation time out!" << std::endl;
	MyGraph * g = MyGraph::getTheGraph();
	MyVertex * vertex;
	int inProgressPacket = 0;
	for (int i = 0; i < g->getNumVertexes(); i++)
	{
		vertex = g->getVertex(i);
		inProgressPacket += vertex->clearPackets();
	}
	return inProgressPacket;
}

void packetsLog(int numPackets, int finishedPackets, int unfinishedPackets, int inProgressPacket)
{
	std::cout << std::endl << "\t\t     PACKET SUMMARY" << std::endl << std::endl;
	std::cout << "\t*****************************************" << std::endl;
	std::cout << "\t*    Total number of packets:     " << numPackets << "\t*" << std::endl;
	std::cout << "\t*    Properly arrived packets:    " << finishedPackets << "\t*" << std::endl;
	std::cout << "\t*    Did not arrive packets:      " << unfinishedPackets << "\t*" << std::endl;
	std::cout << "\t*    Stoped packets:              " << inProgressPacket << "\t*" << std::endl;
	std::cout << "\t*****************************************" << std::endl << std::endl;
}

void createSimulation(int maxTime, int maxPackets, double propability, int minTTL, int maxTTL, int minSize, int maxSize)
{
	srand(time(NULL));
	int numPackets = 0;
	int finishedPackets = 0;
	int unfinishedPackets = 0;
	int countTime = 0;
	double p = 0.0;
	showGraph();
	while (countTime < maxTime)
	{
		printTime(countTime + 1);
		p = ((double)rand() / (RAND_MAX));
		if (p < propability && numPackets < maxPackets)
		{
			createRandomPacket(minTTL, maxTTL, minSize, maxSize);
			numPackets++;
		}
		if (numPackets - finishedPackets - unfinishedPackets > 0)
		{
			unfinishedPackets += forwarding();
			finishedPackets += routing();
		}
		countTime++;
		showGraph();
	}
	int inProgressPacket = endProgress();
	packetsLog(numPackets, finishedPackets, unfinishedPackets, inProgressPacket);
}