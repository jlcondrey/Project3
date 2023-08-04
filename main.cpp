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

        void addAirline(std::string airlineName) {
            // Check if the airline already exists in the map
            auto it = airlines.find(airlineName);
            if (it == airlines.end()) {
                // If the airline does not exist, create a new entry
                airlines.emplace(airlineName, Airline(airlineName));
                it = airlines.find(airlineName); // Update the iterator after emplacing the new entry
            }

            // Update the total minutes delayed for the airline
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

    void addAirlineToAirport(std::string airportCode, std::string airlineName) {
        addAirlineToAirportRecursive(root, airportCode, airlineName);
    }

    void printInOrder() const {
        printInOrderRecursive(root);
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

    void addAirlineToAirportRecursive(std::shared_ptr<AirportNode> currentNode, std::string airportCode, std::string airlineName) {
        if (currentNode != nullptr) {
            if (currentNode->getCode() == airportCode) {
                currentNode->addAirline(airlineName);
            } else if (airportCode < currentNode->getCode()) {
                addAirlineToAirportRecursive(currentNode->left, airportCode, airlineName);
            } else {
                addAirlineToAirportRecursive(currentNode->right, airportCode, airlineName);
            }
        }
    }

    void printInOrderRecursive(std::shared_ptr<AirportNode> currentNode) const {
        if (currentNode != nullptr) {
            printInOrderRecursive(currentNode->left);
            std::cout << "Airport: " << currentNode->getCode() << std::endl << " Airlines: ";
            for (const auto& airlinePair : currentNode->getAirlines()) {
                std::cout << airlinePair.first << ", ";
            }
            std::cout << std::endl;
            printInOrderRecursive(currentNode->right);
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

    int minutesdelayed;

    std::string line;
    std::getline(inputFile, line); // Skip the header line

    std::vector<std::string> elements;
    std::vector<int> late;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string airportCode, carriers;

        // Read the columns from the CSV line
        while (std::getline(iss, airportCode, ',')) {
            while (std::getline(iss, carriers, ',')) {
                if (std::all_of(carriers.begin(), carriers.end(), ::isdigit)) {
                    minutesdelayed = std::stoi(carriers);
                    late.push_back(minutesdelayed);
                    break;
                } else {
                    elements.push_back(carriers);
                }
            }
            break;
        }

        airportBST.insertAirport(airportCode);
        for (size_t i = 0; i < elements.size(); ++i) {
            airportBST.addAirlineToAirport(airportCode, elements[i]);
        }

        // Clear the vectors for the next iteration
        elements.clear();
        late.clear();
    }

    // Print all airports and their airlines
    airportBST.printInOrder();

    return 0;
}
