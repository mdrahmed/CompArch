#include <iostream>
#include <unordered_map>
#include <vector>

struct RPTEntry {
    std::vector<int> predictedDistances;
};

class DistancePrefetcher {
public:
    DistancePrefetcher(int aggressiveness) : aggressiveness(aggressiveness) {
        // Initialize the RPT
        rpt.clear();
    }

    void handleCacheMiss(int address) {
        if (previousMissAddress != -1) {
            int distance = address - previousMissAddress;

            // Search RPT for an entry with the calculated distance
            if (rpt.find(distance) != rpt.end()) {
                RPTEntry& entry = rpt[distance];

                // Issue prefetches based on predicted distances
                for (int i = 0; i < std::min(aggressiveness, static_cast<int>(entry.predictedDistances.size())); i++) {
                    int prefetchAddress = address + entry.predictedDistances[i];
                    std::cout << "Prefetch: " << prefetchAddress << std::endl;
                }

                // Train the DP by adding the new distance to the entry
                entry.predictedDistances.push_back(distance);

                // If the entry is over-aggressive, remove the oldest prediction
                if (entry.predictedDistances.size() > aggressiveness) {
                    entry.predictedDistances.erase(entry.predictedDistances.begin());
                }
            }
        }

        // Update the previous miss address
        previousMissAddress = address;
    }

private:
    int aggressiveness;
    int previousMissAddress = -1;
    std::unordered_map<int, RPTEntry> rpt;
};

int main() {
    DistancePrefetcher dp(3);  // Example with an aggressiveness of 3

    // Simulate cache misses
    dp.handleCacheMiss(10);
    dp.handleCacheMiss(11);
    dp.handleCacheMiss(13);
    dp.handleCacheMiss(14);

    return 0;
}

