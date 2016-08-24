#include <cassert>

#include <iostream>

#include "Graph.h"

Graph::Graph(size_t size)
	: _selfs(size), _incidences(size)
{
}

void Graph::add_self_edge(int i, WeightType weight)
{
	_selfs[i] = weight;
}

void Graph::add_undirected_edge(int i, int j, WeightType weight)
{
	assert(i != j);
	// Do not check if an edge is added more than once
	_incidences[i].push_back({j, weight});
	_incidences[j].push_back({i, weight});
}

void Graph::add_directed_edge(int i, int j, WeightType weight)
{
	assert(i != j);
	// Do not check if an edge is added more than once
	_incidences[i].push_back({j, weight});
}
