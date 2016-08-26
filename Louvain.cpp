#include <iostream>
#include <cassert>
#include <unordered_map>
#include <cmath>
#include <deque>

#include "Louvain.h"

Louvain::Louvain(const Graph& graph)
	: _graph(graph), _m2(0.0), _pass(0)
{
	_in_community.reserve(_graph.num_nodes());
	_communities.reserve(_graph.num_nodes());

	for (int i = 0; i < _graph.num_nodes(); i++) {
		_in_community.push_back(i);

		const vector<EdgeData>& edges = _graph.get_incident_edges(i);
		WeightType neigh = 0.0;
		for (const auto& edge : edges) {
			neigh += edge.weight;
		}
		WeightType self = _graph.get_self_weight(i);
		_communities.push_back({self, neigh + self});
		_m2 += (neigh + 2 * self);
	}

	cout << "Initial Q: " << modularity() << endl;

	_n2c = _in_community;
}

void Louvain::compute()
{
	while(merge()) {
		int num = _graph.num_nodes();
		rebuild();

		_pass++;
		cout << "Pass " << _pass << ": Q " << modularity() << ", "
			<< num << " Nodes, " << num_communities() << " Communities" << endl;

	}
}

/**
 * Return FALSE if no further improvement.
 */
bool Louvain::merge()
{
	// The sum of weights of edges from a node to nodes in a neighbor community
	unordered_map<int, WeightType> neigh_weights;
	bool improved = false;

	deque<int> q;
	for(int i = 0; i < _graph.num_nodes(); i++) {
		q.push_back(i);
	}

	vector<bool> mark(num_nodes(), true);
	while(!q.empty()) {
		int i = q.front();
		q.pop_front();
		assert(mark[i]);
		mark[i] = false;

		neigh_weights.clear();

		const vector<EdgeData>& edges = _graph.get_incident_edges(i);
		WeightType self = _graph.get_self_weight(i);
		WeightType total_weight = self;
		for (const auto& edge : edges) {
			assert(edge.node != i);
			neigh_weights[_in_community[edge.node]] += edge.weight;
			total_weight += edge.weight;
		}

		int prev_community = _in_community[i];
		WeightType prev_neigh_weight = neigh_weights[prev_community];
		remove(i, prev_community, 2 * prev_neigh_weight + self, total_weight);

		WeightType max_inc = 0.0;
		int best_community = prev_community;
		WeightType best_neigh_weight = prev_neigh_weight;
		for (const auto& comm : neigh_weights) {
			int community = comm.first;
			WeightType inc = comm.second - _communities[community].total_weight * total_weight / _m2;
			if (inc > max_inc) {
				max_inc = inc;
				best_community = community;
				best_neigh_weight = comm.second;
			}
		}

		insert(i, best_community, 2 * best_neigh_weight + self, total_weight);

		if (best_community != prev_community) {
			improved = true;

			for (const auto& edge : edges) {
				if (!mark[edge.node]) {
					q.push_back(edge.node);
					mark[edge.node] = true;
				}
			}
		}
	}

	return improved;
}

/**
 * Remove the empty communities and renumber the remaining ones.
 */
void Louvain::shrink_and_renumber_communities()
{
	unordered_map<int, int> renumbers;
	int num = 0;

	for (int i = 0; i < _in_community.size(); i++) {
		auto search = renumbers.find(_in_community[i]);
		if (search == renumbers.end()) {
			renumbers[_in_community[i]] = num;
			_in_community[i] = num;
			num++;
		}
		else {
			_in_community[i] = search->second;
		}
	}

	for (int i = 0; i < num_nodes(); i++) {
		_n2c[i] = _in_community[_n2c[i]];
	}

	vector<CommunityData> communities2(num);
	for (int i = 0; i < _communities.size(); i++) {
		auto search = renumbers.find(i);
		if (search != renumbers.end()) {
			communities2[search->second] = _communities[i];
		}
	}
	communities2.swap(_communities);
}

void Louvain::rebuild()
{
	shrink_and_renumber_communities();

	vector<vector<int>> community_nodes(num_communities());
	for (int i = 0; i < _graph.num_nodes(); i++) {
		community_nodes[_in_community[i]].push_back(i);
	}

	unordered_map<int, WeightType> edges2;
	Graph graph2(num_communities());
	for (int i = 0; i < graph2.num_nodes(); i++) {
		const vector<int>& nodes = community_nodes[i];
		WeightType self_weight = 0.0;
		for (const auto& node : nodes) {
			const vector<EdgeData>& edges = _graph.get_incident_edges(node);
			for (const auto& edge : edges) {
				edges2[_in_community[edge.node]] += edge.weight;
			}
			self_weight += _graph.get_self_weight(node);
		}

		graph2.add_self_edge(i, edges2[i] + self_weight);
		for (const auto& edge : edges2) {
			if (edge.first != i) {
				graph2.add_directed_edge(i, edge.first, edge.second);
			}
		}
		edges2.clear();
	}
	_graph.swap(graph2);

	_in_community.resize(_graph.num_nodes());
	for (int i = 0; i < _in_community.size(); i++) {
		_in_community[i] = i;
	}
}

WeightType Louvain::modularity() const
{
	WeightType q = 0.0;
	for (const auto& comm : _communities) {
		q += comm.in_weight / _m2 - (comm.total_weight / _m2) * (comm.total_weight / _m2);
	}
	return q;
}

void Louvain::output_outline(ostream& out)
{
	out << "#Pass: " << _pass << endl;
	out << "#Communities: " << num_communities() << endl;
	out << "Q: " << modularity() << endl;
}

vector<vector<int>> Louvain::communities() const
{
	vector<vector<int>> comms(num_communities(), vector<int>());
	for (int i = 0; i < _n2c.size(); i++) {
		comms[_n2c[i]].push_back(i);
	}
	return comms;
}
