#ifndef FNV_UTIL_H
#define FNV_UTIL_H

#include <string>
#include <iostream>
#include <sstream>

using namespace std;
   
// Utilites used by NVDIMM
uint64_t convert_uint64_t(string value);

uint divide_params(uint num, uint denom);
uint divide_params(uint64_t num, uint denom);
uint divide_params(uint num, uint64_t denom);
uint divide_params(uint64_t num, uint64_t denom);
uint divide_params(float num, float denom);

#endif
