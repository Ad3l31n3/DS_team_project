// ===== File: AirportGraph.cpp =====
#include "AirportGraph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
using namespace std;


int airportCount = 1;

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
        string origin, destination, city1, state1, city2, state2, distStr, costStr;
        
        //sorts each category of the data in the file into its own string file
        getline(ss, origin, '|');
        getline(ss, destination, '|');
        getline(ss, city1, ',');
        getline(ss, state1, '|');
        getline(ss, city2, ',');
        getline(ss, state2, '|');
        getline(ss, distStr, '|');
        getline(ss, costStr, '|');

        //converts distance and cost to integers
        int distance = stoi(distStr);
        int cost = stoi(costStr);
        
        //checks if origin airport is already in the airports list and if it is not it is added
        int srcIdx = findAirportIndex(origin);
        if (srcIdx == -1) {
            Airport a = {origin, city1, state1, nullptr, (int)airports.size()};
            airports.push_back(a);
            srcIdx = a.index;
            airportIndexMap[origin] = srcIdx;
        }
        
        //checks if origin airport is already in the airports list and if it is not it is added
        int destIdx = findAirportIndex(destination);
        if (destIdx == -1) {
            Airport a = {destination, city2, state2, nullptr, (int)airports.size()};
            airports.push_back(a);
            destIdx = a.index;
            airportIndexMap[destination] = destIdx;
        }
        
        //adds the travel information as an edge
        addEdge(srcIdx, destIdx, distance, cost);
    }
    airportCount = airports.size();
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

// Finds and outputs the route from the start indexu inputted to the end index inputted
int* AirportGraph::findShortestRoute(string startCode, string destCode, bool showOutput, bool showNoResult) {
    int startIdx = findAirportIndex(startCode);
    int destIdx = findAirportIndex(destCode);
    static int output[2] = {0,0};


    if (startIdx == -1 || destIdx == -1) {
        cout << "Invalid airport codes." << endl;
        return output;
    }

    int dist[airportCount];
    int cost[airportCount];
    int prev[airportCount];
    bool visited[airportCount];
    //sets all visited indexes to false
    for (int i = 0; i < airportCount; ++i) visited[i] = false;

    for (int i = 0; i < airportCount; ++i) {
        dist[i] = numeric_limits<int>::max();
        cost[i] = numeric_limits<int>::max();
        prev[i] = -1;
    }

    dist[startIdx] = 0;
    cost[startIdx] = 0;

    for (int count = 0; count < airportCount; ++count) {
        //find unvisited node with smallest distance
        int minDist = numeric_limits<int>::max();
        int u = -1;
        for (int i = 0; i < airportCount; ++i) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        //if no node is found, ends loop
        if (u == -1) break;
        visited[u] = true;

        //checks every destination the airport has to find the shortest one 
        for (Edge* edge = airports[u].head; edge != nullptr; edge = edge->next) {
            int v = edge->dest;
            int newDist = dist[u] + edge->distance;
            int newCost = cost[u] + edge->cost;

            if (newDist < dist[v] || (newDist == dist[v] && newCost < cost[v])) {
                dist[v] = newDist;
                cost[v] = newCost;
                prev[v] = u;
            }
        }
    }

    //only prints no path available if requested
    if (dist[destIdx] == numeric_limits<int>::max()) {
        if(showNoResult){
            cout << "Shortest Path from " << startCode << " to " << destCode << ": None" << endl;
        }
        return output;
    }

    //reconstruct path
    string path[airportCount];
    int pathLen = 0;
    for (int at = destIdx; at != -1; at = prev[at]) {
        path[pathLen++] = airports[at].code;
    }

    //prints path
    cout << "Shortest Path from " << startCode << " to " << destCode << ":\n";
    for (int i = pathLen - 1; i >= 0; --i) {
        cout << path[i];
        if (i > 0) cout << " -> ";
    }
    
    cout << "\nTotal Distance: " << dist[destIdx] << ", Total Cost: " << cost[destIdx] << endl;
    output[0] = dist[destIdx];
    output[1] = cost[destIdx];
    return output;
}

//outputs all shortest routes that can be taken to aereas in the state
void AirportGraph::findAllShortestRoutesToState(string startCode, string state){
    state = " " + state;
    int startIdx = findAirportIndex(startCode);
    if (startIdx == -1) {
        cout << "Invalid starting airport code." << endl;
        return;
    }

    //goes through the airports array and checks for airports located in the clarified state
    bool found = false;
    for (const Airport& airport : airports) {
        if (airport.state == state && airport.code != startCode) {
            int* result = findShortestRoute(startCode, airport.code, true, false);
            if (result[0] != 0 || result[1] != 0) {
                found = true;
                cout << "To " << airport.code << " (" << airport.city << ", " << state << "): ";
                cout << "Distance = " << result[0] << ", Cost = " << result[1] << endl;
                cout << "-----------------------------" << endl;
            }
        }
    }
    
    //specifies if no airport paths can be found in the state
    if (!found) {
        cout << "No airports found in state " << state << " or no valid routes." << endl;
    }
}

void AirportGraph::findShortestRouteGivenStops(string startCode, string destCode, int numStops) {
    int startIdx = findAirportIndex(startCode);
    int destIdx = findAirportIndex(destCode);

    if (startIdx == -1 || destIdx == -1) {
        cout << "Invalid start or destination code." << endl;
        return;
    }

    const int MAX_PATH_LEN = 20; 
    const int MAX_STACK_SIZE = 1000;

    //creates a node that can keep track of the details as they are being made
    struct PathNode {
        int idx;
        int path[MAX_PATH_LEN]; // holds airport indices
        int pathLength;
        int totalDist;
        int totalCost;
    };

    PathNode stack[MAX_STACK_SIZE];
    int stackSize = 0;

    //initialize stack with start node
    stack[stackSize++] = {startIdx, {startIdx}, 1, 0, 0};

    int bestDistance = numeric_limits<int>::max();
    int bestCost = numeric_limits<int>::max();
    int bestPath[MAX_PATH_LEN];
    int bestPathLength = 0;

    while (stackSize > 0) {
        PathNode current = stack[--stackSize];

        //if we've made exactly numStops + 1 jumps, check if we reached the destination
        if (current.pathLength == numStops + 2) {
            if (current.idx == destIdx) {
                if (current.totalDist < bestDistance || 
                   (current.totalDist == bestDistance && current.totalCost < bestCost)) {
                    bestDistance = current.totalDist;
                    bestCost = current.totalCost;
                    bestPathLength = current.pathLength;
                    for (int i = 0; i < bestPathLength; ++i) {
                        bestPath[i] = current.path[i];
                    }
                }
            }
            continue;
        }

        //explore neighbors
        for (Edge* edge = airports[current.idx].head; edge != nullptr; edge = edge->next) {
            //avoid cycles
            bool alreadyVisited = false;
            for (int i = 0; i < current.pathLength; ++i) {
                if (current.path[i] == edge->dest) {
                    alreadyVisited = true;
                    break;
                }
            }
            if (alreadyVisited) continue;

            if (stackSize >= MAX_STACK_SIZE) continue; // avoid overflow

            //build next node
            PathNode next = current;
            next.idx = edge->dest;
            next.path[next.pathLength++] = edge->dest;
            next.totalDist += edge->distance;
            next.totalCost += edge->cost;

            stack[stackSize++] = next;
        }
    }

    //output result
    if (bestPathLength == 0) {
        cout << "Shortest route from " << startCode << " to " << destCode << " with " << numStops << " stops: None" << endl;
    } else {
        cout << "Shortest route from " << startCode << " to " << destCode << " with " << numStops << " stops:\n";
        for (int i = 0; i < bestPathLength; ++i) {
            cout << airports[bestPath[i]].code;
            if (i < bestPathLength - 1) cout << " -> ";
        }
        cout << "\nTotal Distance: " << bestDistance << ", Total Cost: " << bestCost << endl;
    }
}
