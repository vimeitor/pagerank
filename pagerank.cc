#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Node {
	std::vector<std::string> incoming;
	int weight;
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

		Node e;
		e.weight = 0;

		std::string code = fields[4].substr(1, fields[4].length() - 2);
		nodemap[code] = e;
		nodelist.push_back(code);
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

		if (fields[2].length() != 3 || fields[4].length() != 3)
			continue;

		std::string origin = fields[2];
		std::string dest  = fields[4];

		/* std::cout << origin << dest << std::endl; */

		nodemap[dest].incoming.push_back(origin);
		nodemap[origin].weight++;
	}
}

std::map<std::string, double> pagerank(std::vector<std::string> &nodelist,
									   std::map<std::string, Node> &nodemap)
{
	int n = nodelist.size();
	double dfactor = 0.85;
	int it = 100;

	std::map<std::string, double> prev_pagerank;
	for (auto e : nodelist)
		prev_pagerank[e] = 1 / n;
	std::map<std::string, double> pagerank;

	for (int i = 0; i < it; i++) {
		/* For each node in the graph, we compute its new page rank by adding
		 * the previous page rank of its incoming nodes divided by the weight of
		 * each incoming node. */
		for (int j = 0; j < nodelist.size(); j++) {
			double pr = 0.0;
			std::string code = nodelist[j];
			Node e = nodemap[code];
			for (int k = 0; k < e.incoming.size(); k++) {
				std::string inc = e.incoming[k];
				pr += prev_pagerank[inc] / nodemap[inc].weight;
			}
			pagerank[code] = dfactor * pr + (1 - dfactor) / n;
		}
		prev_pagerank = pagerank;
	}
	return pagerank;
}

int main()
{
	std::vector<std::string> nodelist;
	std::map<std::string, Node> nodemap;

	read_airports(nodelist, nodemap);
	read_routes(nodelist, nodemap);

	auto pr = pagerank(nodelist, nodemap);
	for (int i = 0; i < nodelist.size(); i++) {
		auto e = nodelist[i];
		std::cout << e << ": " << pr[e] << std::endl;
	}

	return 0;
}
