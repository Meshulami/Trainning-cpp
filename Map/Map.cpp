#include "Map.h"

namespace MAP
{

/*------------------------------MyGraph class------------------------------*/

	MyGraph * MyGraph::instanceFlag = 0; //singleton


	int MyGraph::getVertexIndex(const string & str)
	{
		list <MyVertex*>::iterator it = nodes.begin();
		int index = 0;
		while (it != nodes.end())
		{
			if ((*it)->getName() == str)
				return index;
			it++;
			index++;
		}
		return -1;
	}

	string MyGraph::getVertexName(const int index)
	{
		list <MyVertex*>::iterator it = nodes.begin();
		for (int i = 0; i < index; i++)
			it++;
		return ((*it)->getName());
	}

	MyVertex * MyGraph::getVertex(const string & str)
	{
		if (getVertexIndex(str) != -1)
			for (list <MyVertex*>::iterator it = nodes.begin(); it != nodes.end(); it++)
				if ((*it)->getName() == str)
					return *it;
		//return &MyVertex("none");
	}

	bool MyGraph::addVertex(MyVertex & node)
	{
		if (getVertexIndex(node.getName()) == -1)
		{
			numVertexes++;
			list<MyVertex*>::iterator it = nodes.end();
			nodes.insert(it, &node);
			return true;
		}
		return false;
	}

	void MyGraph::show()
	{
		std::cout << "#Vertexes: " << numVertexes << std::endl;
		for (list<MyVertex*>::iterator it = nodes.begin(); it != nodes.end(); it++)
			(*it)->show();
		std::cout << std::endl;
	}


/*------------------------------MyVertex class------------------------------*/

	MyVertex::MyVertex(const string & str)
	{
		name.assign(str);
		numEdges = 0;
		arcs.clear();
		queue.clear();
	}

	bool operator==(const MyVertex & node1, const MyVertex & node2)
	{
		if (node1.name == "none" || node2.name == "none")
			return false;
		return (node1.name == node2.name);
	}

	MyVertex & MyVertex::operator=(const MyVertex & node)
	{
		if (this == &node)
			return *this;
		name.assign(node.name);
		numEdges = node.numEdges;
		arcs.assign(node.arcs.begin(), node.arcs.end());
		queue.assign(node.queue.begin(), node.queue.end());
		return *this;
	}

	bool MyVertex::isEdgeInVertex(const MyEdge & arc)
	{
		if (numEdges == 0)
			return false;
		for (list<MyEdge*>::iterator it = arcs.begin(); it != arcs.end(); it++)
		{
			if (*it == &arc)
				return true;
		}
		return false;
	}

	bool MyVertex::addEdge(MyEdge & arc)
	{
		MyGraph * g = MyGraph::getTheGraph();
		if (g->getVertexIndex(arc.getStart()) == -1 || g->getVertexIndex(arc.getFinish()) == -1)
			return false;
		if (!(arc.getStart() == this->name))
			return false;
		if (!isEdgeInVertex(arc))
		{
			numEdges++;
			arcs.push_back(&arc);
			return true;
		}
		return false;
	}

	int MyVertex::updateTimeAndForward()
	{
		int unfinishedPacket = 0;
		for (list<MyEdge*>::iterator it = arcs.begin(); it != arcs.end(); it++)
		{
			unfinishedPacket += (*it)->updateTime();
			(*it)->calcPacketsProgress();
		}
		return unfinishedPacket;
	}

	int MyVertex::routingQueue()
	{
		int finishedPackets = 0;
		int index = 0;
		list<Packet*>::iterator it = queue.begin();
		while (it != queue.end())
		{
			if ((*it)->isArrived())
			{
				finishedPackets++;
				(*it)->deletePacketLog(true);
			}
			else
			{
				index = calcIndexToSend(**it);
				sendPacket(**it, index);
			}
			it = queue.erase(it);
		}
		return finishedPackets;
	}

	void MyVertex::sendPacket(Packet & pack, int index)
	{
		list <MyEdge*>::iterator it = arcs.begin();
		for (int i = 0; i < index; i++)
			it++;
		(*it)->pushPacket(pack);
	}

	int MyVertex::clearPackets()
	{
		int inProgressPacket = 0;
		for (list<MyEdge*>::iterator it = arcs.begin(); it != arcs.end(); it++)
			inProgressPacket += (*it)->clearPacketsInProgress();
		return inProgressPacket;
	}

	void MyVertex::show()
	{
		std::cout << "Vertex name: " << this->name << std::endl;
		std::cout << "    #Edges: " << this->numEdges << std::endl;
		for (list<MyEdge*>::iterator it = arcs.begin(); it != arcs.end(); it++)
			(*it)->show();
	}

	MyVertex* MyVertex::vertexFactory(const string & str, vertex_choice choice)
	{
		switch (choice)
		{
		case Random:
			return (new MyVertexRandom(str));
		case First:
			return (new MyVertexFirst(str));
		case Dijkstra:
			return (new MyVertexDijkstra(str));
		}
	}

//Inheritance
	//MyVertexRandom class
	int MyVertexRandom::calcIndexToSend(Packet & pack)
	{
		int index = rand() % numEdges;
		return index;
	}

	//MyVertexFirst class
	int MyVertexFirst::calcIndexToSend(Packet & pack)
	{
		return 0;
	}

	//MyVertexDijkstra class
	double** MyVertexDijkstra::createMat(int V)
	{
		double** mat = new double*[V];
		for (int i = 0; i < V; ++i)
		{
			mat[i] = new double[V];
			for (int j = 0; j < V; ++j)
				mat[i][j] = (i == j) ? 0 : INT_MAX;
		}
		return mat;
	}

	void MyVertexDijkstra::setMat(double** mat, int V, Packet & pack)
	{
		MyGraph * g = MyGraph::getTheGraph();
		MyVertex * startVertex;
		for (int i = 0; i < V; ++i)
		{
			startVertex = g->getVertex(i);
			startVertex->setRowInMat(mat, V, pack, i);
		}
	}

	void MyVertexDijkstra::setRowInMat(double** mat, int V, Packet & pack, const int row)
	{
		MyGraph * g = MyGraph::getTheGraph();
		list<MyEdge*>::iterator it;
		it = (this->arcs).begin();
		while (it != (this->arcs).end())
		{
			if (this->isEdgeInVertex(**it))
				mat[row][g->getVertexIndex((*it)->getFinish())] = (*it)->calcHopTime(pack);
			it++;
		}
	}

	int MyVertexDijkstra::nextIndexDijkstra(double** mat, int V, int src, int dest)
	{
		double * dist = new double[V];
		int * prev = new int[V];
		bool * visited = new bool[V];
		int minDist, minIndex, nextIndex = 0;
		double d;
		for (int i = 0; i< V; i++)
		{
			dist[i] = INT_MAX;
			prev[i] = -1;
			visited[i] = false;
		}
		int cur = src;
		dist[cur] = 0;
		visited[cur] = true;
		while (!visited[dest])
		{
			minDist = INT_MAX;
			minIndex = 0;
			for (int i = 1; i < V; i++)
			{
				d = dist[cur] + mat[cur][i];
				if (d < dist[i] && !visited[i])
				{
					dist[i] = d;
					prev[i] = cur;
				}
				if (minDist > dist[i] && !visited[i])
				{
					minDist = dist[i];
					minIndex = i;
				}
			}
			cur = minIndex;
			visited[cur] = true;
		}
		cur = dest;
		while (cur != -1)
		{
			if (prev[cur] == src)
				nextIndex = cur;
			cur = prev[cur];
		}
		delete dist;
		delete prev;
		delete visited;
		return nextIndex;
	}

	int MyVertexDijkstra::calcIndex(const string & str)
	{
		list <MyEdge*>::iterator it = arcs.begin();
		int index = 0;
		while (it != arcs.end())
		{
			if ((*it)->getFinish() == str)
				return index;
			it++;
			index++;
		}
		return -1;
	}

	void MyVertexDijkstra::freeMat(double** mat, int V)
	{
		for (int i = 0; i < V; ++i)
			delete[] mat[i];
		delete[] mat;
	}

	int MyVertexDijkstra::calcIndexToSend(Packet & pack)
	{
		MyGraph * g = MyGraph::getTheGraph();
		int V = g->getNumVertexes();
		double** mat = createMat(V);
		setMat(mat, V, pack);
		int nextIndex = nextIndexDijkstra(mat, V, g->getVertexIndex(name), g->getVertexIndex(pack.getDest()));
		int index = calcIndex(g->getVertexName(nextIndex));
		freeMat(mat, V);
		return index;
	}


/*------------------------------MyEdge class------------------------------*/

	MyEdge::MyEdge(const string & name1, const string & name2, int w)
	{
		start.assign(name1);
		finish.assign(name2);
		width = w;
		numItems = 0;
		items.clear();
	}

	bool operator==(const MyEdge & arc1, const MyEdge & arc2)
	{
		if (!(arc1.start == arc2.start && arc1.finish == arc2.finish))
			return false;
		return (arc1.width == arc2.width && arc1.numItems == arc2.numItems);
	}

	Packet * MyEdge::getPacket(const int index)
	{
		int num = index;
		list<Packet*>::iterator it = items.begin();
		while (it != items.end() && num != 0)
		{
			num--;
			it++;
		}
		return *it;
	}

	int MyEdge::updateTime()
	{
		int unfinishedPacket = 0;
		list<Packet*>::iterator it = items.begin();
		while ( it != items.end())
		{
			if ((*it)->isTimeOut())
			{
				unfinishedPacket++;
				(*it)->deletePacketLog(false);
				it = items.erase(it);
				numItems--;
			}
			else
			{
				(*it)->addTime();
				it++;
			}
		}
		return unfinishedPacket;
	}

	void MyEdge::calcPacketsProgress()
	{
		int bytesRemain = width;
		int packRemain;
		int curIndex;
		Packet * curPack;
		MyGraph * g = MyGraph::getTheGraph();
		MyVertex * destVertex = g->getVertex(finish);
		while (!items.empty())
		{
			curIndex = calcNextPacketIndex();
			curPack = getPacket(curIndex);
			packRemain = curPack->getRemainProgress();
			if (packRemain > bytesRemain)
			{
				curPack->addProgress(bytesRemain);
				break;
			}
			curPack->resetProgress();
			bytesRemain -= packRemain;
			curPack->updatePath(destVertex->getName());
			destVertex->addPacketToQueue(*curPack);
			items.remove(curPack);
			numItems--;
		}
	}

	int MyEdge::clearPacketsInProgress()
	{
		int inProgressPacket = 0;
		while (!items.empty())
		{
			items.front()->deletePacketLog(false);
			items.pop_front();
			numItems--;
			inProgressPacket++;
		}
		return inProgressPacket;
	}

	void MyEdge::show()
	{
		std::cout << "    " << start << "->" << finish << " ;   width = " << width << " , numItems = " << numItems << std::endl;
		for (list<Packet*>::iterator it = items.begin(); it != items.end(); it++)
		{
			std::cout << "        ";
			(*it)->show();
		}
	}

	MyEdge* MyEdge::edgeFactory(const string & name1, const string & name2, int w, edge_choice choice)
	{
		switch (choice)
		{
		case FIFO:
			return (new MyEdgeFIFO(name1, name2, w));
		case Smallest:
			return (new MyEdgeSmallest(name1, name2, w));
		case TTL:
			return (new MyEdgeTTL(name1, name2, w));
		}
	}

//Inheritance
	//MyEdgeFIFO class
	const int MyEdgeFIFO::calcNextPacketIndex()
	{
		return 0;
	}

	int MyEdgeFIFO::calcHopTime(Packet & pack)
	{
		double totalTime = pack.getRemainProgress();
		list<Packet*>::iterator it = items.begin();
		while (it != items.end())
		{
			totalTime += (*it)->getRemainProgress();
			it++;
		}
		return int(ceil(totalTime / width));
	}

	//MyEdgeSmallest class
	const int MyEdgeSmallest::calcNextPacketIndex()
	{
		list<Packet*>::iterator it = items.begin();
		int index = 0;
		int minProgress = (*it)->getRemainProgress();
		int minIndex = 0;
		while (it != items.end())
		{
			if ((*it)->getRemainProgress() < minProgress)
			{
				minProgress = (*it)->getRemainProgress();
				minIndex = index;
			}
			it++;
			index++;
		}
		return minIndex;
	}

	int MyEdgeSmallest::calcHopTime(Packet & pack)
	{
		double remainPack = pack.getRemainProgress();
		double totalTime = pack.getRemainProgress();
		list<Packet*>::iterator it = items.begin();
		while (it != items.end())
		{
			if ((*it)->getRemainProgress() <=remainPack)
				totalTime += (*it)->getRemainProgress();
			it++;
		}
		return int(ceil(totalTime / width));
	}

	//MyEdgeTTL class
	const int MyEdgeTTL::calcNextPacketIndex()
	{
		list<Packet*>::iterator it = items.begin();
		int index = 0;
		int minTime = (*it)->getRemainTime();
		int minIndex = 0;
		while (it != items.end())
		{
			if ((*it)->getRemainTime() < minTime)
			{
				minTime = (*it)->getRemainTime();
				minIndex = index;
			}
			it++;
			index++;
		}
		return minIndex;
	}

	int MyEdgeTTL::calcHopTime(Packet & pack)
	{
		double remainPack = pack.getRemainTime();
		double totalTime = pack.getRemainProgress();
		list<Packet*>::iterator it = items.begin();
		while (it != items.end())
		{
			if ((*it)->getRemainTime() <= remainPack)
				totalTime += (*it)->getRemainProgress();
			it++;
		}
		return int(ceil(totalTime / width));
	}


/*------------------------------Packet class------------------------------*/

	Packet::Packet(const string & src, const string & dest, const int n, int t)
	{
		size = n;
		progress = 0.0;
		TTL = t;
		time = 0;
		destination.assign(dest);
		path.push_back(src);
	}

	void Packet::deletePacketLog(bool isFinish)
	{
		std::cout << "| The packet from \"" << path.front() << "\" to \"" << destination << "\" " << (isFinish ? "arrived" : "didn't arrive"); 
		std::cout << " properly." << std::endl << "| Path: \"" << path.front() << "\"";
		list<string>::iterator it = path.begin();
		if (it != path.end())
			for (it++; it != path.end(); it++)
				std::cout << " -> \"" << *it << "\"";
		std::cout << std::endl << "| ";
		show();
		std::cout << std::endl;
	}

	void Packet::show()
	{
		std::cout << "Destination: " << destination << "    Time: " << time << "/" << TTL << "    Progress: " << int(progress) << "/" << size << std::endl;
	}

}