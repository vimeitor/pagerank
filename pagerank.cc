#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Node {
	std::map<std::string, int> incoming;
	int weight = 0;
};

void read_airports(std::vector<std::string> &nodelist,
				   std::map<std::string, Node> &nodemap)
{
	std::ifstream file("airports.txt");
	if (!file) {
		std::cout << "error: can't read airports" << std::endl;
		std::exit(1);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<std::string> fields;
		std::stringstream ss(line);
		std::string field;
		while (std::getline(ss, field, ','))
			fields.push_back(field);

		if (fields[4].length() != 5)
			continue;

		std::string code = fields[4].substr(1, fields[4].length() - 2);
		nodelist.push_back(code);

		/* We initialize it so we can later make sure the edges have nodes
		 * existing in airports.txt. */
		nodemap[code] = Node();
	}
}

void read_routes(std::vector<std::string> &nodelist,
				 std::map<std::string, Node> &nodemap)
{
	std::ifstream file("routes.txt");
	if (!file) {
		std::cout << "error: can't read routes" << std::endl;
		std::exit(1);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<std::string> fields;
		std::stringstream ss(line);
		std::string field;
		while (std::getline(ss, field, ','))
			fields.push_back(field);

		std::string from = fields[2];
		std::string to  = fields[4];

		if (from.size() != 3 || to.size() != 3 ||
			!nodemap.count(from) || !nodemap.count(to))
			continue;

		nodemap[to].incoming[from]++;
		nodemap[from].weight++;
	}
}

std::map<std::string, double> pagerank(std::vector<std::string> &nodelist,
									   std::map<std::string, Node> &nodemap)
{
	int n = nodelist.size();
	double dfactor = 0.85;
	double stop = 0.001;
	int max_it = 10;

	/* Any initialization would work, since they all converge to the same value. */
	std::map<std::string, double> prev_pagerank;
	for (auto e : nodelist)
		prev_pagerank[e] = 1.0 / n;

	/* Dangling nodes give their page rank to non-dangling ones. */
	double prev_extra = 0.0;
	for (std::string e : nodelist) {
		if (!nodemap[e].weight)
			prev_extra += prev_pagerank[e] / n;
	}

	bool convergence = false;
	for (int i = 0; i < max_it && !convergence; i++) {
		convergence = true;
		std::map<std::string, double> pagerank;
		double extra = 0.0;

		/* For each node in the graph, we compute its new page rank by adding
		 * the previous page rank of its incoming nodes divided by the weight of
		 * each incoming node. */
		for (std::string code : nodelist) {
			double pr = 0.0;
			Node e = nodemap[code];

			for (auto k : e.incoming) {
				std::string from = k.first;
				int weight = k.second;
				pr += prev_pagerank[from] * weight / nodemap[from].weight;
			}

			pagerank[code] = dfactor * (pr + prev_extra) + (1.0 - dfactor) / n;
			if (std::abs(pagerank[code] - prev_pagerank[code]) > stop)
				convergence = false;
			if (e.weight == 0)
				extra += pagerank[code] / n;
		}

		prev_pagerank = pagerank;
		prev_extra = extra;
	}
	return prev_pagerank;
}

void print_pagerank(std::vector<std::string> &nodelist,
					std::map<std::string, double> &pagerank)
{
	for (std::string code : nodelist) {
		std::cout << "\e[91m" << code << ":\e[0m " << pagerank[code] << std::endl;
	}
}

int main()
{
	std::vector<std::string> nodelist;
	std::map<std::string, Node> nodemap;

	read_airports(nodelist, nodemap);
	read_routes(nodelist, nodemap);

	auto pr = pagerank(nodelist, nodemap);
	print_pagerank(nodelist, pr);

	return 0;
}
