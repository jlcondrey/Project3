#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <algorithm>
#include <map>

class Airline {
public:
    Airline(std::string name) : name(name), totalMinutesDelayed(0) {}

    std::string getName() const {
        return name;
    }

    void addMinutesDelayed(int minutes) {
        totalMinutesDelayed += minutes;
    }

    int getTotalMinutesDelayed() const {
        return totalMinutesDelayed;
    }

private:
    std::string name;
    int totalMinutesDelayed;
};

class AirportBST {
private:
    struct AirportNode {
        AirportNode(std::string airportCode) : code(airportCode) {}

        std::string getCode() const {
            return code;
        }

        void addAirline(std::string airlineName, int statMinutesDelayedTotal) {
            // Check if the airline already exists in the map
            auto it = airlines.find(airlineName);
            if (it == airlines.end()) {
                // If the airline does not exist, create a new entry
                airlines.emplace(airlineName, Airline(airlineName));
                it = airlines.find(airlineName); // Update the iterator after emplacing the new entry
            }

            // Update the total minutes delayed for the airline
            it->second.addMinutesDelayed(statMinutesDelayedTotal);
        }

        int getTotalMinutesDelayed() const {
            int totalMinutesDelayed = 0;
            for (const auto& airlinePair : airlines) {
                totalMinutesDelayed += airlinePair.second.getTotalMinutesDelayed();
            }
            return totalMinutesDelayed;
        }

        const std::map<std::string, Airline>& getAirlines() const {
            return airlines;
        }


        std::shared_ptr<AirportNode> left;
        std::shared_ptr<AirportNode> right;

    private:
        std::string code;
        std::map<std::string, Airline> airlines;
    };

public:
    AirportBST() : root(nullptr) {}

    void insertAirport(std::string airportCode) {
        root = insertRecursive(root, airportCode);
    }

    void addAirlineToAirport(std::string airportCode, std::string airlineName, int statMinutesDelayedTotal) {
        addAirlineToAirportRecursive(root, airportCode, airlineName, statMinutesDelayedTotal);
    }

    void printInOrder() const {
        printInOrderRecursive(root);
    }

    std::map<std::string, int> getTotalMinutesDelayedByAirline() const {
        std::map<std::string, int> airlineTotalMinutes;

        getTotalMinutesDelayedRecursive(root, airlineTotalMinutes);

        return airlineTotalMinutes;
    }

private:
    std::shared_ptr<AirportNode> root;

    // Private helper functions for tree operations
    std::shared_ptr<AirportNode> insertRecursive(std::shared_ptr<AirportNode> currentNode, std::string airportCode) {
        if (currentNode == nullptr) {
            return std::make_shared<AirportNode>(airportCode);
        }

        if (airportCode < currentNode->getCode()) {
            currentNode->left = insertRecursive(currentNode->left, airportCode);
        } else if (airportCode > currentNode->getCode()) {
            currentNode->right = insertRecursive(currentNode->right, airportCode);
        }

        return currentNode;
    }

    void addAirlineToAirportRecursive(std::shared_ptr<AirportNode> currentNode, std::string airportCode, std::string airlineName, int statMinutesDelayedTotal) {
        if (currentNode != nullptr) {
            if (currentNode->getCode() == airportCode) {
                currentNode->addAirline(airlineName, statMinutesDelayedTotal);
            } else if (airportCode < currentNode->getCode()) {
                addAirlineToAirportRecursive(currentNode->left, airportCode, airlineName, statMinutesDelayedTotal);
            } else {
                addAirlineToAirportRecursive(currentNode->right, airportCode, airlineName, statMinutesDelayedTotal);
            }
        }
    }

    void printInOrderRecursive(std::shared_ptr<AirportNode> currentNode) const {
        if (currentNode != nullptr) {
            printInOrderRecursive(currentNode->left);
            std::cout << "Airport: " << currentNode->getCode() << std::endl << "Airlines: ";
            for (const auto& airlinePair : currentNode->getAirlines()) {
                std::cout << airlinePair.first << "\"" << ", ";
            }
            int airportTotalMinutedDelayed = 0;
            for (const auto& airlinePair : currentNode->getAirlines()) {
                airportTotalMinutedDelayed += airlinePair.second.getTotalMinutesDelayed();
            }
            std::cout << std::endl;
            std::cout << "Total Minutes Delayed for the Airport: " << airportTotalMinutedDelayed << std::endl;
            std::cout << std::endl;
            printInOrderRecursive(currentNode->right);
        }
    }

    void getTotalMinutesDelayedRecursive(std::shared_ptr<AirportNode> currentNode, std::map<std::string, int>& airlineTotalMinutes) const {
        if (currentNode != nullptr) {
            const std::map<std::string, Airline>& airlines = currentNode->getAirlines();
            for (const auto& airlinePair : airlines) {
                std::string airlineName = airlinePair.first;
                int minutesDelayed = airlinePair.second.getTotalMinutesDelayed();

                // Check if the airline already exists in the map
                auto it = airlineTotalMinutes.find(airlineName);
                if (it == airlineTotalMinutes.end()) {
                    // If the airline does not exist, create a new entry with total minutes delayed set to 0
                    airlineTotalMinutes[airlineName] = 0;
                    it = airlineTotalMinutes.find(airlineName); // Update the iterator after emplacing the new entry
                }

                // Update the total minutes delayed for the airline
                it->second += minutesDelayed;
            }

            getTotalMinutesDelayedRecursive(currentNode->left, airlineTotalMinutes);
            getTotalMinutesDelayedRecursive(currentNode->right, airlineTotalMinutes);
        }
    }
};

int main() {
    AirportBST airportBST;

    // Read data from CSV file and populate the AirportBST
    std::ifstream inputFile("airlines.csv");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the CSV file." << std::endl;
        return 1;
    }

    std::string line;
    std::getline(inputFile, line); // Skip the header line
    std::vector<std::string> elements;
    std::vector<int> late;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string airportCode, carriers, lateAircraftDelays, timeMonth;
        std::string timeMonthName, timeYear, statDelaysCarrier, statDelaysLateAircraft;
        std::string statDelaysNAS, statDelaysSecurity, statDelaysWeather, statCarriersName, statCarriersTotal;
        std::string statFlightsCancelled, statFlightsDelayed, statFlightsDiverted, statFlightsOnTime;
        std::string statFlightsTotal, statMinutesDelayedCarrier, statMinutesDelayedLateAircraft;
        std::string statMinutesDelayedNAS, statMinutesDelayedSecurity, statMinutesDelayedTotal, statMinDelayedWeather;

        // Read the columns from the CSV line
        while (std::getline(iss, airportCode, ',')) {
            while (std::getline(iss, carriers, ',')) {
                if (std::all_of(carriers.begin(), carriers.end(), ::isdigit)) {
                    break;
                } else {
                    elements.push_back(carriers);
                }
            }
            break;
        }
        std::getline(iss, timeMonth, ',');
        std::getline(iss, timeMonthName, ',');
        std::getline(iss, timeYear, ',');
        std::getline(iss, statDelaysCarrier, ',');
        std::getline(iss, statDelaysLateAircraft, ',');
        std::getline(iss, statDelaysNAS, ',');
        std::getline(iss, statDelaysSecurity, ',');
        std::getline(iss, statDelaysWeather, ',');
        while (std::getline(iss, statCarriersName, ',')) {
            if (std::all_of(statCarriersName.begin(), statCarriersName.end(), ::isdigit)) {
                break;
            }
        }
        std::getline(iss, statCarriersTotal, ',');
        std::getline(iss, statFlightsCancelled, ',');
        std::getline(iss, statFlightsDelayed, ',');
        std::getline(iss, statFlightsDiverted, ',');
        std::getline(iss, statFlightsOnTime, ',');
        std::getline(iss, statFlightsTotal, ',');
        std::getline(iss, statMinutesDelayedCarrier, ',');
        std::getline(iss, statMinutesDelayedLateAircraft, ',');
        std::getline(iss, statMinutesDelayedNAS, ',');
        std::getline(iss, statMinutesDelayedSecurity, ',');
        std::getline(iss, statMinutesDelayedTotal, ',');
        std::getline(iss, statMinDelayedWeather, ',');
        statMinutesDelayedTotal = statMinutesDelayedSecurity;

        airportBST.insertAirport(airportCode);
        for (size_t i = 0; i < elements.size(); ++i) {
            airportBST.addAirlineToAirport(airportCode, elements[i], std::stoi(statMinutesDelayedTotal));
        }

        // Clear the vectors for the next iteration
        elements.clear();
        late.clear();
    }

    // Print all airports and their airlines
    airportBST.printInOrder();

    return 0;
}
