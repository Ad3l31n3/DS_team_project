// ===== File: AirportGraph.cpp =====
#include "AirportGraph.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

//finds where an airport is located within the airports vector
int AirportGraph::findAirportIndex(const string& code) {
    if (airportIndexMap.find(code) != airportIndexMap.end()) {
        return airportIndexMap[code];
    }
    return -1;
}

//adds an edge tp the airport vector
void AirportGraph::addEdge(int srcIdx, int destIdx, int distance, int cost) {
    Edge* newEdge = new Edge{destIdx, distance, cost, airports[srcIdx].head};
    airports[srcIdx].head = newEdge;
}

//adds all unique airports from the provided file to the airports vector and creates an edge 
//for every possible way to travel
void AirportGraph::loadTXT(const string& filename) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string origin, destination, city1, city2, distStr, costStr;
        
        //sorts each category of the data in the file into its own string file
        getline(ss, origin, '|');
        getline(ss, destination, '|');
        getline(ss, city1, '|');
        getline(ss, city2, '|');
        getline(ss, distStr, '|');
        getline(ss, costStr, '|');

        //converts distance and cost to integers
        int distance = stoi(distStr);
        int cost = stoi(costStr);
        
        //checks if origin airport is already in the airports vector and if it is not it is added
        int srcIdx = findAirportIndex(origin);
        if (srcIdx == -1) {
            Airport a = {origin, city1, "", nullptr, (int)airports.size()};
            airports.push_back(a);
            srcIdx = a.index;
            airportIndexMap[origin] = srcIdx;
        }
        
        //checks if origin airport is already in the airports vector and if it is not it is added
        int destIdx = findAirportIndex(destination);
        if (destIdx == -1) {
            Airport a = {destination, city2, "", nullptr, (int)airports.size()};
            airports.push_back(a);
            destIdx = a.index;
            airportIndexMap[destination] = destIdx;
        }
        
        //adds the travel information as an edge
        addEdge(srcIdx, destIdx, distance, cost);
    }
    file.close();
}

//prints the index, code, and city of every item in airports
void AirportGraph::displayAirports() const {
    for (const auto& airport : airports) {
        cout << airport.index << ": " << airport.code << " - " << airport.city << endl;
    }
}

//prints an airport and every destination it can have along with the cost and distance for it
void AirportGraph::displayGraph() const {
    for (const auto& airport : airports) {
        cout << airport.code << " -> ";
        for (Edge* e = airport.head; e != nullptr; e = e->next) {
            cout << airports[e->dest].code << " (Dist: " << e->distance << ", Cost: " << e->cost << "), ";
        }
        cout << endl;
    }
}

//returns the vector for airports
const vector<Airport>& AirportGraph::getAirports() const {
    return airports;
}
