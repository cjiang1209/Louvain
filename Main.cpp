#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <utility>

#include "Graph.h"
#include "GraphReader.h"
#include "Louvain.h"

using namespace std;

double get_cpu_time()
{
	rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	return static_cast<double>(ru.ru_utime.tv_sec) + static_cast<double>(ru.ru_utime.tv_usec) / 1e6;
}

int main(int argc, char* argv[])
{
	GraphReader gr;
	Graph graph = gr.build_graph(argv[1]);

	double start = get_cpu_time();

	Louvain lou(graph);
	lou.compute();

	double end = get_cpu_time();

	lou.output_outline(cout);
	cout << "Completed" << endl;
	cout << "Time: " << end - start << " s" << endl;

	return 0;
}
