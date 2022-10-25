#include "circuit_graph.h"
#include "iscas89_parser.h"
#include "../include/log.h"
#include "../include/timer.h"

#include <fstream>
#include <algorithm>

struct Config
{
	uint64_t total_time_limit_s = 0;

	bool write_faults = 0;
	bool write_solutions = 0;
	bool write_detectability = 0;
	bool do_solve = 1;
	bool write_stats = 1;
	bool short_stats = 0;
	float threshold_ratio = 0.4f;
} g_config;

int main(int argc, char* argv[])
{
    if (argc < 2) {
		log_error() << "no input file specified";
		return 1;
	}
	std::ifstream ifs(argv[1]);
	if (!ifs.good()) {
		log_error() << "can't open file" << argv[1];
		return 1;
	}

  	CircuitGraph graph;
	Iscas89Parser parser;
	if (!parser.parse(ifs, graph)) {
		log_error() << "can't parse file" << argv[1];
		return 1;
	}

	//graph.get_graph_stats();
	struct
	{
		uint64_t fault_generation = 0;
		uint64_t cnf_generation = 0;
		uint64_t cnf_solving = 0;
		uint64_t worst_solving = 0;
	} timing;

	



	return 0;
}