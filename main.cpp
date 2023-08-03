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

        void addAirline(std::string airlineName, int minutesDelayed) {
            // Check if the airline already exists in the map
            auto it = airlines.find(airlineName);
            if (it == airlines.end()) {
                // If the airline does not exist, create a new entry
                airlines.emplace(airlineName, Airline(airlineName));
                it = airlines.find(airlineName); // Update the iterator after emplacing the new entry
            }

            // Update the total minutes delayed for the airline
            it->second.addMinutesDelayed(minutesDelayed);
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

    void addAirlineToAirport(std::string airportCode, std::string airlineName, int minutesDelayed) {
        addAirlineToAirportRecursive(root, airportCode, airlineName, minutesDelayed);
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

    void addAirlineToAirportRecursive(std::shared_ptr<AirportNode> currentNode, std::string airportCode, std::string airlineName, int minutesDelayed) {
        if (currentNode != nullptr) {
            if (currentNode->getCode() == airportCode) {
                currentNode->addAirline(airlineName, minutesDelayed);
            } else if (airportCode < currentNode->getCode()) {
                addAirlineToAirportRecursive(currentNode->left, airportCode, airlineName, minutesDelayed);
            } else {
                addAirlineToAirportRecursive(currentNode->right, airportCode, airlineName, minutesDelayed);
            }
        }
    }

    void printInOrderRecursive(std::shared_ptr<AirportNode> currentNode) const {
        if (currentNode != nullptr) {
            printInOrderRecursive(currentNode->left);
            std::cout << "Airport: " << currentNode->getCode() << std::endl << " Airlines: ";
            for (const auto& pair : currentNode->getAirlines()) {
                std::cout << pair.first << " (Minutes Delayed: " << pair.second.getTotalMinutesDelayed() << "), ";
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

    std::string line;
    std::getline(inputFile, line); // Skip the header line

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string airportCode, carriers, lateAircraftDelays;

        // Read the columns from the CSV line
        if (std::getline(iss, airportCode, ',')) {
            std::getline(iss, carriers, ',');
            std::getline(iss, lateAircraftDelays, ',');

            // Trim leading and trailing spaces from the airline names
            carriers.erase(carriers.begin(), std::find_if(carriers.begin(), carriers.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            carriers.erase(std::find_if(carriers.rbegin(), carriers.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), carriers.end());

            airportBST.insertAirport(airportCode);

            std::istringstream airlinesStream(carriers);
            std::string airlineName;
            while (std::getline(airlinesStream, airlineName, ',')) {
                // Trim leading and trailing spaces from the airline name
                airlineName.erase(airlineName.begin(), std::find_if(airlineName.begin(), airlineName.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
                airlineName.erase(std::find_if(airlineName.rbegin(), airlineName.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), airlineName.end());

                if (!airlineName.empty()) {
                    int minutesDelayed = 0; // Default value for invalid input
                    try {
                        // Convert to integer with error handling
                        minutesDelayed = std::stoi(lateAircraftDelays);
                    } catch (const std::invalid_argument&) {
                        // Handle invalid input (e.g., non-numeric values)
                        std::cerr << "Invalid input for minutes delayed: " << lateAircraftDelays << std::endl;
                    }

                    airportBST.addAirlineToAirport(airportCode, airlineName, minutesDelayed);
                }
            }
        } else {
            std::cerr << "Error reading data from the CSV file." << std::endl;
            return 1;
        }
    }

    // Print all airports and their airlines
    airportBST.printInOrder();

    return 0;
}