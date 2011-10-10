#ifndef FNVTBS_H
#define FNVTBS_H

#include "FastNV.h"

class test_obj{
public:
    void read_cb(uint, uint64_t, uint64_t, bool);
    void crit_cb(uint, uint64_t, uint64_t, bool);
    void write_cb(uint, uint64_t, uint64_t, bool);
    void power_cb(uint, vector<vector<double>>, uint64_t, bool);
    void run_test(void);
};
#endif
