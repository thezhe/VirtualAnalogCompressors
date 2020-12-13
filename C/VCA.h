/** Zhe Deng 2020
 *  Mini VCA Compressor Library
 */  

#include <math.h>
#include <stdlib.h>

#ifndef _VCA_
#define _VCA_

/**
 * Internal Compressor Parameters
 * ctf exponent, linear threshold, attack G, release G 
 */
typedef struct
{
    float ctfExp, thrlin, G_a, G_r;    
}VCAparams;

/**
 * Internal Compressor VCAstate
 * LPF state, previous y_LP
 */ 
typedef struct 
{
    float s, y;
}VCAstate;

/**
 * Convert user params to internal params
 * @param attack attack time in ms
 * @param release release time in ms
 * @return internal parameters
 */ 
VCAparams* VCA_getParams(float thrdB, float ratio, float attack, float release, float sampleRate);

/**
 * Free memory 
 */
void VCA_reset(VCAstate* ste, VCAparams* p);

/**
 * g(x), the ctf gain function 
 */
float VCA_gofx(float x, VCAparams* p);

/**
 * TPT BF
 */ 
float VCA_BF(float x, VCAstate* ste, VCAparams* p);

/**
 * Process one sample through an FFVCA
 * @param ste initialize with malloc and resuse per sample
 * @param prm initialize with getParams and resuse per sample
 * @return FFVCA output
 */ 
float VCA_tickFFVCA(float x, VCAstate* ste, VCAparams* prm);

#endif