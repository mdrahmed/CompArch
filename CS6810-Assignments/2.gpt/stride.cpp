#include <iostream>
#include <unordered_map>
#include <vector>

// Define the RPT Entry structure
struct RPTEntry {
    ADDRINT tag;
    ADDRINT prevAccessAddr;
    ADDRINT stride;
    UINT8 state;
};

class StridePrefetcher : public PrefetcherInterface {
private:
    // Set up private members
    std::unordered_map<ADDRINT, RPTEntry> rpt;
    UINT8 aggression;

public:
    StridePrefetcher(UINT8 rptSize, UINT8 _aggression) : aggression(_aggression) {
        // Initialize the RPT with a given size and default values
        for (UINT8 i = 0; i < rptSize; i++) {
            RPTEntry entry;
            entry.tag = 0;
            entry.prevAccessAddr = 0;
            entry.stride = 0;
            entry.state = 0; // Initial state
            rpt[i] = entry;
        }
    }

    void prefetch(ADDRINT addr, ADDRINT loadPC) {
        // Prefetcher implementation
        if (rpt.find(loadPC) != rpt.end()) {
            // An entry exists in the RPT for this load instruction
            RPTEntry &entry = rpt[loadPC];

            // Check if the entry is in a steady state and the prediction is correct
            if (entry.state == 2 && (addr == entry.prevAccessAddr + entry.stride)) {
                for (int i = 1; i <= aggression; i++) {
                    ADDRINT nextAddr = addr + i * entry.stride;
                    if (!cache->exists(nextAddr)) {
                        cache->prefetchFillLine(nextAddr);
                        prefetches++;
                    }
                }
                // Update the previous address field
                entry.prevAccessAddr = addr;
            }
	else{
		// not a stride
		// global variable value, if 0 then it's start
		if(glo == 0){
			rpt[0].prevAccessAddr = addr;
			rpt[0].tag = loadPC;
		}				
		else{
			rpt[glo+1].stride = rpt[glo].prevAccessAddr - addr;
			if (rpt[glo+1].stride == rpt[glo].stride and rpt[glo].state < 2){
				rpt[glo+1].state += 1;
			}
		}
		glo++;
	}
        }
    }

    void train(ADDRINT addr, ADDRINT loadPC) {
         // Training implementation
        if (rpt.find(loadPC) != rpt.end()) {
            // An entry exists in the RPT for this load instruction
            RPTEntry &entry = rpt[loadPC];

            // Calculate the stride
            ADDRINT new_stride = addr - entry.prevAccessAddr;
	
            // Check the state and update the entry accordingly
            if (entry.state == 0 || entry.state == 1) {
                // Initial or Transient state
                if (new_stride == entry.stride) {
                    // Transition to Steady state if the stride remains the same
                    entry.state = 2;
                } else {
                    // Transition to no-prediction state if the stride changes
                    entry.state = 3;
                }
            } else if (entry.state == 2) {
                // Steady state
                if (new_stride != entry.stride) {
                    // Transition to Initial state if the stride changes
                    entry.state = 0;
                }
            } else if(entry.state == 3){
		if (new_stride == entry.stride){
			entry.state = 1;
		}	    	
	    }
            // Update the stride and previous address fields
            entry.stride = new_stride;
            entry.prevAccessAddr = addr;
        }
    }
};

