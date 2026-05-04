#include "delay.h"

#define PeriOdic(T)\
static uint32_t nxt=0;\
if(GetTick()<nxt) return;\
nxt +=(T);\

