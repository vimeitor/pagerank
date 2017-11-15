#include <cstdlib>
#include <fstream>
#include <iostream>
#include <tr1/unordered_map>
#include <sstream>
#include <string>
#include <vector>

struct Node {
	std::string code;
	std::tr1::unordered_map<std::string, int> incoming;
	int weight = 0;
};

void parse_input(std::vector<std::string> &fields, std::string &line)
{
	std::string field;
	int comma = 0;
	for (char c : line) {
		if (comma > 4)
			break;
		if (c != ',')
			field += c;
		else {
			fields.push_back(field);
			field.clear();
			comma++;
		}
	}
	fields.push_back(field);
}

void read_airports(std::vector<Node> &node_list,
				   std::tr1::unordered_map<std::string, int> &node_index)
{
	std::ifstream file("airports.txt");
	if (!file) {
		std::cout << "error: can't read airports" << std::endl;
		std::exit(1);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<std::string> fields;
		parse_input(fields, line);

		if (fields[4].length() != 5)
			continue;

		Node e;
		e.code = fields[4].substr(1, fields[4].length() - 2);
		node_list.push_back(e);
		node_index[e.code] = node_list.size() - 1;
	}
}

void read_routes(std::vector<Node> &node_list,
				 std::tr1::unordered_map<std::string, int> &node_index)
{
	std::ifstream file("routes.txt");
	if (!file) {
		std::cout << "error: can't read routes" << std::endl;
		std::exit(1);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<std::string> fields;
		std::string field;
		parse_input(fields, line);

		std::string from = fields[2];
		std::string to  = fields[4];

		if (from.size() != 3 || to.size() != 3 ||
			!node_index.count(from) || !node_index.count(to))
			continue;

		int index_to = node_index[to];
		int index_from = node_index[from];
		node_list[index_to].incoming[from]++;
		node_list[index_from].weight++;
	}
}

std::vector<double> pagerank(std::vector<Node> &node_list,
							 std::tr1::unordered_map<std::string, int> &node_index)
{
	int n = node_list.size();
	double dfactor = 0.85;
	double stop = 0.001;
	int max_it = 10;

	/* Any initialization would work, since they all converge to the same value. */
	std::vector<double> prev_pagerank(n);
	for (int i = 0; i < prev_pagerank.size(); i++)
		prev_pagerank[i] = 1.0 / n;

	/* Dangling nodes give their page rank to non-dangling ones. */
	double prev_extra = 0.0;
	for (int i = 0; i < node_list.size(); i++)
		if (!node_list[i].weight)
			prev_extra += prev_pagerank[i] / n;

	bool convergence = false;
	for (int i = 0; i < max_it && !convergence; i++) {
		convergence = true;
		std::vector<double> pagerank(n);
		double extra = 0.0;

		/* For each node in the graph, we compute its new page rank by adding
		 * the previous page rank of its incoming nodes divided by the weight of
		 * each incoming node. */
		for (int e = 0; e < node_list.size(); e++) {
			double pr = 0.0;
			for (auto k : node_list[e].incoming) {
				std::string from = k.first;
				int from_weight = k.second;
				int out = node_list[node_index[from]].weight;
				pr += prev_pagerank[node_index[from]] * from_weight / out;
			}

			pagerank[e] = dfactor * (pr + prev_extra) + (1.0 - dfactor) / n;
			if (std::abs(pagerank[e] - prev_pagerank[e]) > stop)
				convergence = false;
			if (node_list[e].weight == 0)
				extra += pagerank[e] / n;
		}

		prev_pagerank = pagerank;
		prev_extra = extra;
	}
	return prev_pagerank;
}

void print_pagerank(std::vector<Node> &node_list,
					std::vector<double> &pagerank)
{
	for (int i = 0; i < node_list.size(); i++) {
		std::cout << "\e[91m" << node_list[i].code <<
			":\e[0m " << pagerank[i] << std::endl;
	}
}

int main()
{
	std::vector<Node> node_list;
	std::tr1::unordered_map<std::string, int> node_index;

	read_airports(node_list, node_index);
	read_routes(node_list, node_index);

	auto pr = pagerank(node_list, node_index);
	print_pagerank(node_list, pr);

	return 0;
}
