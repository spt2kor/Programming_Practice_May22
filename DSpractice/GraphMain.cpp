#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <stack>


using namespace std;

struct Edge
{
	int u;
	int v;
};

struct Graph
{
	int V = -1;
	int E = -1;
	vector <vector<int>> adjMatrix;
	vector<Edge> edges;
	vector <list<int>> adjList;

	Graph() {}

	template <int vertex>
	Graph(int adjMat[][vertex]) : V(vertex)
	{
		adjMatrix.resize(V);
		for (auto& v : adjMatrix)
			v.resize(V,0);

		for (int i = 0; i < V; ++i)
			for (int j = 0; j < V; ++j) {
				adjMatrix[i][j] = adjMat[i][j];
				if (adjMatrix[i][j])
					++E;
			}
		E = E / 2;
	}

	void Print()
	{
		cout << "\n Graph Content" ;
		cout << "\n\t no of Vertex = " << V;
		cout << "\n\t no of Edges = " << E;
		cout << "\n\t Matrix data  " ;
		for (auto& v : adjMatrix)
		{
			cout << endl;
			for (auto& x : v)
				cout << "\t" << x;
		}

		//cout << "\n\t Edges List= " << V;

	}
	//=======================================================
	void DFS_REC_UTIL(int v, vector<bool>& visited) {
		if (0 <= v && v < V && !visited[v])
		{
			cout << "\t" << v;
			visited[v] = true;

			auto &mat = adjMatrix[v];

			for (int i=0;i<V ; ++i)
				if (mat [i] >= 1 && !visited[i])
					DFS_REC_UTIL(i, visited);
		}
	}

	void DFS_Rec(int v) {
		vector<bool> visited(V, false);
		cout << "\n\n DFS REC from start node = " << v <<" \t\t : ";
		DFS_REC_UTIL(v, visited);
		cout << endl;
	}
	//=======================================================
	//=======================================================
	void DFS_ITERATIVE(int v) {
		vector<bool> visited(V, false);
		cout << "\n\n DFS ITERATIVE from start node = " << v << " \t\t : ";
		stack<int> st;

		visited[v] = true;
		st.push(v);
		cout << "\t" << v;
		while (!st.empty())
		{
			int u = st.top();

			auto& mat = adjMatrix[u];
			bool childs_done = true;

			for (int i = 0; i < V; ++i)
				if (mat[i] >= 1 && !visited[i])
				{
					cout << "\t" << i;
					st.push(i);
					visited[i] = true;
					childs_done = false;
					break;
				}
			if (childs_done)
			{
				st.pop();
			}
		}
		cout << endl;
	}
	//=======================================================
	//=======================================================
	void BFS(int v) 
	{
		vector<bool> visited(V, false);
		cout << "\n\n BFS REC from start node = " << v << " \t\t : ";

		queue<int> qu;
		qu.push(v);

		while (!qu.empty())
		{
			int u = qu.front(); 
			qu.pop();
			if (!visited[u])
			{
				cout << "\t" << u;
				visited[u] = true;

				auto& mat = adjMatrix[u];

				for (int i = 0; i < V; ++i) {
					if ((mat[i] >= 1) && !visited[i]) {
						qu.push(i);
					}
				}
			}
		}
		cout << endl;
	}
	//=======================================================
	void Dijkstra(int v)
	{

	}
};

void main()
{
	cout << "\n\nProgram started" << endl;

	int graphMatrix[7][7] = {	{0,0,6,7,0,0,0} ,
								{0,0,0,4,5,2,0} ,
								{6,0,0,5,0,0,0} ,
								{7,4,5,0,0,3,0} ,
								{0,5,0,0,0,5,4} ,
								{0,2,0,3,5,0,1} ,
								{0,0,0,0,4,1,0} };

	Graph gr(graphMatrix);
	gr.Print();

	gr.DFS_Rec(0);
	gr.DFS_ITERATIVE(0);

	gr.DFS_Rec(1);
	gr.DFS_ITERATIVE(1);

	gr.BFS(0);

	gr.Dijkstra(0);
	cout << "\n\nProgram ended" << endl;
}