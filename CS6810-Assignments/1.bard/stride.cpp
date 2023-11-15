class StridePrefetcher : public PrefetcherInterface {
private:
  // A reference prediction table (RPT), mapping load PC addresses to RPT entries.
  std::unordered_map<ADDRINT, RPTEntry> rpt_;

  // The aggressiveness of the prefetcher, i.e., the number of blocks to prefetch
  // when a strided access pattern is detected.
  int aggressiveness_;

public:
  StridePrefetcher(int aggressiveness = 1) : aggressiveness_(aggressiveness) {}

  void prefetch(ADDRINT addr, ADDRINT loadPC) override {
    // Get the RPT entry for the load PC.
    auto it = rpt_.find(loadPC);
    if (it == rpt_.end()) {
      // If there is no RPT entry for the load PC, create a new one.
      RPTEntry entry;
      entry.state = RPTEntry::State::Initial;
      entry.prev_access_addr = addr;
      rpt_[loadPC] = entry;
      return;
    }

    // Update the RPT entry with the new access address.
    RPTEntry& entry = it->second;
    entry.prev_access_addr = addr;

    // If the RPT entry is in the steady state, prefetch the next block.
    if (entry.state == RPTEntry::State::Steady) {
      for (int i = 0; i < aggressiveness_; i++) {
        ADDRINT prefetch_addr = entry.prev_access_addr + (i + 1) * entry.stride;
        Prefetcher::prefetch(prefetch_addr);
      }
    }
  }

  void train(ADDRINT addr, ADDRINT loadPC) override {
    // Get the RPT entry for the load PC.
    auto it = rpt_.find(loadPC);
    if (it == rpt_.end()) {
      // If there is no RPT entry for the load PC, create a new one.
      RPTEntry entry;
      entry.state = RPTEntry::State::Initial;
      entry.prev_access_addr = addr;
      entry.stride = 0;
      rpt_[loadPC] = entry;
      return;
    }

    // Calculate the stride.
    RPTEntry& entry = it->second;
    int stride = addr - entry.prev_access_addr;

    // Update the RPT entry with the new stride.
    entry.stride = stride;

    // Update the state of the RPT entry.
    switch (entry.state) {
      case RPTEntry::State::Initial:
        if (stride != 0) {
          entry.state = RPTEntry::State::Transient;
        }
        break;
      case RPTEntry::State::Transient:
        if (stride == entry.stride) {
          entry.state = RPTEntry::State::Steady;
        } else {
          entry.stride = stride;
        }
        break;
      case RPTEntry::State::Steady:
        if (stride != entry.stride) {
          entry.state = RPTEntry::State::Initial;
        }
        break;
      case RPTEntry::State::NoPrediction:
        break;
    }
  }
};

