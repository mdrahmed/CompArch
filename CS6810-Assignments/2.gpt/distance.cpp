class DistancePrefetcher : public PrefetcherInterface {
private:
    struct RPTEntry {
        ADDRINT tag;
        ADDRINT prevAccessAddr;
	ADDRINT prevDistance;
        //std::vector<ADDRINT> predictedDistances;
	int predictedDistances[aggression] = {};
    };

    std::unordered_map<ADDRINT, RPTEntry> rpt;
    UINT8 aggression;

public:
    DistancePrefetcher(UINT8 _aggression) : aggression(_aggression) {
        // Initialize your data structures and members here
    }

void prefetch(ADDRINT addr, ADDRINT loadPC) {
    // Calculate distance between the current address and the previous address
    ADDRINT distance = addr - rpt[loadPC].prevAccessAddr;

    // Search the RPT for an entry that corresponds to the distance
    if (rpt.find(distance) != rpt.end()) {
        RPTEntry &entry = rpt[distance];

        // Calculate prefetch addresses based on predicted distances
        for (int i = 0; i < aggression; i++) {
            ADDRINT predictedDistance = entry.predictedDistances[i];
            ADDRINT prefetchAddr = addr + predictedDistance;

            // Check if the prefetchAddr is not in the cache
            if (!cache->exists(prefetchAddr)) {
                // Issue prefetch for prefetchAddr (assuming your cache supports prefetch)
                cache->prefetchFillLine(prefetchAddr);
                prefetches++;
            }
        }
    }
	else {
        // Entry does not exist, allocate a new entry
        if (rpt.size() >= RPT_SIZE) {
            // If the RPT is full, evict an entry according to RPT replacement policy
            // You'll need to implement this replacement policy
            // For simplicity, we'll use a random replacement policy here
            rpt.erase(rpt.begin());
        }
	// Create a new entry for the distance
        RPTEntry newEntry;
        newEntry.tag = distance;
        newEntry.prevAccessAddr = addr;
        
        // Initialize predicted distances
        for (int i = 0; i < aggression; i++) {
            newEntry.predictedDistances[i] = 0;
        }
        
        rpt[distance] = newEntry;
}

    void train(ADDRINT addr, ADDRINT loadPC) {
        // Calculate the distance between the current address and the previous address
        ADDRINT distance = addr - rpt[loadPC].prevAccessAddr;

        // Check if an entry exists in RPT for this distance
        if (rpt.find(distance) != rpt.end()) {
            RPTEntry &entry = rpt[distance];

            // Add the newly observed distance to the entry's predicted distances
            entry.predictedDistances.push_back(distance);

            // Check if the entry has more predicted distances than allowed by the aggression
            while (entry.predictedDistances.size() > aggression) {
                // Evict the oldest predicted distance to make space for the new distance
                entry.predictedDistances.erase(entry.predictedDistances.begin());
            }
        } 

        // Update the previous address
        rpt[loadPC].prevAccessAddr = addr;
	rpt[loadPC].prevDistance = distance;
    }

2 1 2 3
5


// else of train, no need but kept here for safety,

	else { // This will never execute as the prefetch will do this already. 
	
	// Entry does not exist, allocate a new entry
        if (rpt.size() >= RPT_SIZE) {
            // If the RPT is full, evict an entry according to RPT replacement policy
            // You'll need to implement this replacement policy
            // For simplicity, we'll use a random replacement policy here
            rpt.erase(rpt.begin());
        }
            // Create a new entry for this distance
            RPTEntry newEntry;
            newEntry.tag = distance;
            newEntry.prevAccessAddr = addr;
            newEntry.predictedDistances.push_back(distance);
            rpt[distance] = newEntry;
        }
