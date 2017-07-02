#ifndef MAP_H_
#define MAP_H_

//includes
#include <string>
#include <list>
#include <iostream>
#include <ctime>

//using
using std::string;
using std::list;


namespace MAP
{
	class MyVertex;
	class MyEdge;
	class Packet;

/*------------------------------MyGraph class------------------------------*/

	class MyGraph
	{
	private:
	//Data
		int numVertexes;
		list <MyVertex*> nodes;
	//Singleton
		MyGraph() {};
		MyGraph(MyGraph const&) {};
		~MyGraph() { delete instanceFlag; nodes.~list(); };
		MyGraph & operator=(MyGraph const&) {};
		static MyGraph* instanceFlag;
	public:
		static MyGraph * getTheGraph() { if (!instanceFlag) instanceFlag = new MyGraph; return instanceFlag; };	//Singleton
	//Member functions
		int getNumVertexes() { return numVertexes; };
		int getVertexIndex(const string & str);
		string getVertexName(const int index);
		MyVertex * getVertex(const string & str);
		MyVertex * getVertex(const int index) { return getVertex(getVertexName(index)); };
		bool addVertex(MyVertex & node);
		void show();
	};


/*------------------------------MyVertex class------------------------------*/

	class MyVertex
	{
	protected:
	//Data
		string name;
		int numEdges;
		list <MyEdge*> arcs;
		list <Packet*> queue;
	//Method
		bool isEdgeInVertex(const MyEdge & arc);
	public:
	//Standard functions
		explicit MyVertex(const string & str = "none");
		virtual ~MyVertex() { arcs.~list();  queue.~list();};
		friend bool operator==(const MyVertex & node1, const MyVertex & node2);
		MyVertex & operator=(const MyVertex & node);
	//Member functions
		const string & getName() const { return name; };
		void addPacketToQueue(Packet & pack) { queue.push_back(&pack); };
		bool addEdge(MyEdge & arc);
		int updateTimeAndForward();
		int routingQueue();	
		inline void sendPacket(Packet & pack, int index);
		int clearPackets();
		void show();
	//Factory
		enum vertex_choice { Random = 1, First = 2, Dijkstra = 3 };
		static MyVertex* vertexFactory(const string & str, vertex_choice choice);
	//Virtuals
		virtual int calcIndexToSend(Packet & pack) = 0; //ABC
		virtual void setRowInMat(double** mat, int V, Packet & pack, const int row) {};
	};


//Inheritance
	//MyVertexRandom class
	class MyVertexRandom : public MyVertex
	{
	public:
		explicit MyVertexRandom(const string & str) : MyVertex(str) {};
		~MyVertexRandom() {};
	private:
		int calcIndexToSend(Packet & pack);
	};

	//MyVertexFirst class
	class MyVertexFirst : public MyVertex
	{
	public:
		explicit MyVertexFirst(const string & str) : MyVertex(str) {};
		~MyVertexFirst() {};
	private:
		int calcIndexToSend(Packet & pack);
	};

	//MyVertexDijkstra class
	class MyVertexDijkstra : public MyVertex
	{
	public:
		explicit MyVertexDijkstra(const string & str) : MyVertex(str) {};
		~MyVertexDijkstra() {};
	private:
		double** createMat(int V);
		void setMat(double** mat, int V, Packet & pack);
		virtual void setRowInMat(double** mat, int V, Packet & pack, const int row);
		int nextIndexDijkstra(double** mat, int V, int src, int dest);
		int calcIndex(const string & str);
		void freeMat(double** mat, int V);
		int calcIndexToSend(Packet & pack);
	};


/*------------------------------MyEdge class------------------------------*/

	class MyEdge
	{
	protected:
	//Data
		string start;
		string finish;
		int width;
		int numItems;
		list <Packet*> items;
	public:
	//Standard functions
		MyEdge(const string & name1, const string & name2, int w = 100);
		virtual ~MyEdge() { items.~list(); };
		friend bool operator==(const MyEdge & arc1, const MyEdge & arc2);
	//Member functions
		string & getStart() { return start; };
		string & getFinish() { return finish; };
		void pushPacket(Packet & pack) { items.push_back(&pack); numItems++; };
		Packet * getPacket(const int index);
		int updateTime();
		void calcPacketsProgress();
		int clearPacketsInProgress();
		void show();
	//Factory
		enum edge_choice { FIFO = 1, Smallest = 2, TTL = 3 };
		static MyEdge* edgeFactory(const string & name1, const string & name2, int w, edge_choice choice);
	//Virtuals
		virtual const int calcNextPacketIndex() = 0; //ABC
		virtual int calcHopTime(Packet & pack) = 0; //ABC
	};

//Inheritance
	//MyEdgeFIFO class
	class MyEdgeFIFO : public MyEdge
	{
	public:
		MyEdgeFIFO(const string & name1, const string & name2, int w) : MyEdge(name1, name2, w) {};
		~MyEdgeFIFO() {};
	private:
		const int calcNextPacketIndex();
		int calcHopTime(Packet & pack);
	};

	//MyEdgeSmallest class
	class MyEdgeSmallest : public MyEdge
	{
	public:
		MyEdgeSmallest(const string & name1, const string & name2, int w) : MyEdge(name1, name2, w) {};
		~MyEdgeSmallest() {};
	private:
		const int calcNextPacketIndex();
		int calcHopTime(Packet & pack);
	};

	//MyEdgeTTL class
	class MyEdgeTTL : public MyEdge
	{
	public:
		MyEdgeTTL(const string & name1, const string & name2, int w) : MyEdge(name1, name2, w) {};
		~MyEdgeTTL() {};
	private:
		const int calcNextPacketIndex();
		int calcHopTime(Packet & pack);
	}; 


/*------------------------------Packet class------------------------------*/

	class Packet
	{
	private:
	//Data
		int size;
		double progress;
		int TTL;
		int time;
		string destination;
		list <string> path;
	public:
	//Standard functions
		Packet(const string & src, const string & dest, const int n = 200, int t = 30);
		~Packet() { path.~list(); };
	//Member functions
		double getRemainProgress() { return (size - progress); };
		void addProgress(const double p) { progress += p; };
		void resetProgress() { progress = 0.0; };
		int getRemainTime() { return (TTL - time); };
		void addTime() { time++; };
		bool isTimeOut() { return time >= TTL; };
		string getDest() { return destination; };
		void updatePath(const string & str) { path.push_back(str); };
		bool isArrived() { return (path.back() == destination); };
		void deletePacketLog(bool isFinish);
		void show();
	};
}

#endif // !MAP_H_