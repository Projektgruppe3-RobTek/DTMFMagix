#include <cmath>
#include <vector>
#include <iostream>
#ifndef M_PI
#define M_PI 3.1415926535f
#endif
/*Inspired by instructions at
**https://courses.cs.washington.edu/courses/cse466/12au/calendar/Goertzel-EETimes.pdf
*/

float doGoertzel(int sampleRate, float targetFreq, std::vector<float> &samples)
{
    //Define constants
    float k = (int) (0.5 + ((float(samples.size()) * targetFreq) / sampleRate));
    float w = ((2. * M_PI)  / (int)samples.size())*k ;
    float cosine=cos(w);
    float sine=sin(w);
    float coeff=2.*cosine;
    
    //declare sample variables
    float q0 = 0.; //this sample
    float q1 = 0.; //last sample
    float q2 = 0.; //sample two times ago
    
    //Compute goertzel transform
    for (int i = 0; i < samples.size(); i++){ 
        q0 = coeff * q1 - q2 + samples[i];
        q2 = q1;
        q1 = q0;
    } 
    
    return sqrt(q1*q1 + q2*q2 - q1 * q2 * coeff); //return power 
}
