#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "GraphReader.h"

using namespace std;

Graph GraphReader::build_graph(const char* file_name)
{
	ifstream in(file_name, ifstream::in);
	if (!in.is_open()) {
		cerr << "Cannot open file: " << file_name << endl;
		exit(0);
	}

	vector<tuple<int, int, WeightType>> edges;
	int i, j;
	WeightType weight;
	size_t num_var = 0;
	while (!in.eof()) {
		in >> i >> j >> weight;
		edges.emplace_back(i, j, weight);

		if (num_var < i + 1) {
			num_var = i + 1;
		}
		if (num_var < j + 1) {
			num_var = j + 1;
		}
	}

	Graph graph(num_var);
	for (const auto& edge : edges) {
		if (std::get<0>(edge) == std::get<1>(edge)) {
			graph.add_self_edge(std::get<0>(edge), std::get<2>(edge));
		}
		else {
			graph.add_undirected_edge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
		}
	}
	return graph;
}
