#ifndef _UTILS_H_
#define _UTILS_H_

// --- HELPER FUNCTIONS ---

int clampInt(int v, int lo, int hi) {
if (v < lo) return lo;
if (v > hi) return hi;
return v;
}



#endif
