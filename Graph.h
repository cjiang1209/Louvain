#ifndef _NETWORK_HEADER
#define _NETWORK_HEADER

#include <vector>

using namespace std;

typedef float WeightType;

struct EdgeData
{
	int node;
	WeightType weight;
};

class Graph
{
private:
	vector<WeightType> _selfs;
	vector<vector<EdgeData>> _incidences;

public:
	Graph(size_t size);

	size_t num_nodes() const;

	void add_self_edge(int i, WeightType weight = 0.0);
	void add_undirected_edge(int i, int j, WeightType weight = 1.0);
	void add_directed_edge(int i, int j, WeightType weight = 1.0);

	const vector<EdgeData>& get_incident_edges(int i) const;
	WeightType get_self_weight(int i) const;

	void swap(Graph& other);
};

inline size_t Graph::num_nodes() const
{
	return _incidences.size();
}

inline const vector<EdgeData>& Graph::get_incident_edges(int i) const
{
	return _incidences[i];
}

inline WeightType Graph::get_self_weight(int i) const
{
	return _selfs[i];
}

inline void Graph::swap(Graph& other)
{
	other._selfs.swap(_selfs);
	other._incidences.swap(_incidences);
}

#endif
