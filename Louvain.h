#ifndef _LOUVAIN_HEADER
#define _LOUVAIN_HEADER

#include <vector>
#include <cassert>

#include "Graph.h"

using namespace std;

struct CommunityData
{
	WeightType in_weight;
	WeightType total_weight;
};

class Louvain
{
private:
	Graph _graph;
	vector<int> _n2c;

	vector<CommunityData> _communities;
	vector<int> _in_community;

	WeightType _m2;	// 2 * m
	int _pass;

	bool merge();	// The first phase of Louvain
	void rebuild();	// The second phase of Louvain

	void remove(int i, int community, WeightType in_weight, WeightType total_weight);
	void insert(int i, int community, WeightType in_weight, WeightType out_weight);

	void shrink_and_renumber_communities();

public:
	Louvain(const Graph& graph);

	void compute();
	void output_outline(ostream& out);

	size_t num_nodes() const;
	size_t num_communities() const;
	vector<vector<int>> communities() const;

	WeightType modularity() const;
};

inline size_t Louvain::num_nodes() const
{
	return _n2c.size();
}

inline size_t Louvain::num_communities() const
{
	return _communities.size();
}

inline void Louvain::insert(int i, int community, WeightType in_weight, WeightType total_weight)
{
	assert(_in_community[i] != community);
	_in_community[i] = community;
	_communities[community].in_weight += in_weight;
	_communities[community].total_weight += total_weight;
}

inline void Louvain::remove(int i, int community, WeightType in_weight, WeightType total_weight)
{
	assert(_in_community[i] == community);
	_in_community[i] = -1;
	_communities[community].in_weight -= in_weight;
	_communities[community].total_weight -= total_weight;
}

#endif
