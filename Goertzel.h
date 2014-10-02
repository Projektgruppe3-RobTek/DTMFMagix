#include <cmath>
#include <vector>
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
float goertzel_mag(int TARGET_FREQUENCY,int SAMPLING_RATE, std::vector<float> &data);
