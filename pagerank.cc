#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Airport {
	int id;
	std::string name;
	std::string city;
	std::string country;
	std::string iata;
	std::string icao;
};

struct Route {
	std::string airline_code;
	int OF_code;
	std::string origin;
	int OF_origin;
	std::string destination;
};

Airport create_airport(std::string &line)
{
	Airport a;
	int index = 0;
	std::stringstream ss(line);
	std::string field;
	while (std::getline(ss, field, ',') && index < 6) {
		if (field == "\\N") {
			index++;
			continue;
		}
		if (index == 0) {
			int id = std::stoi(field);
			a.id = id;
		} else {
			/* We remove the quotes. */
			std::string sub = field.substr(1, field.length()-2);
			if (index == 1)
				a.name = sub;
			else if (index == 2)
				a.city = sub;
			else if (index == 3)
				a.country = sub;
			else if (index == 4)
				a.iata = sub;
			else
				a.icao = sub;
		}
		index++;
	}
	return a;
}

Route create_route(std::string &line)
{
	Route r;
	int index = 0;
	std::stringstream ss(line);
	std::string field;
	while (std::getline(ss, field, ',') && index < 5) {
		if (field == "\\N") {
			index++;
			continue;
		}
		if (index == 1 || index == 3) {
			int id = std::stoi(field);
			if (index == 1)
				r.OF_code = id;
			else
				r.OF_origin = id;
		} else {
			if (field.length() < 2) {
				index++;
				continue;
			}
			/* We remove the quotes. */
			std::string sub = field.substr(1, field.length()-2);
			if (index == 0)
				r.airline_code = sub;
			else if (index == 2)
				r.origin = sub;
			else if (index == 4)
				r.destination = sub;
		}
		index++;
	}
	return r;
}

int main()
{
	std::ifstream airport_file("airports.txt");
	std::ifstream route_file("routes.txt");
	if (!airport_file || !route_file) {
		std::cout << "error: can't read files" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<Airport> airports;
	std::vector<Route> routes;
	std::string line;
	while (std::getline(airport_file, line))
		airports.push_back(create_airport(line));
	while (std::getline(route_file, line))
		routes.push_back(create_route(line));

	return 0;
}
