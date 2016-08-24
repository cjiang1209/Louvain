#ifndef _GRAPHREADER_HEADER
#define _GRAPHREADER_HEADER

#include "Graph.h"

class GraphReader
{
public:
	Graph build_graph(const char* file_name);
};

#endif
