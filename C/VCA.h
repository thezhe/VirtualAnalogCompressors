/** Zhe Deng 2020
 *  Mini VCA Compressor Library
 */  

#include <math.h>
#include <stdlib.h>

#ifndef _VCA_
#define _VCA_

/** Internal Compressor Parameters
 * * Not all members may be used
 * Members: ctf exponent, linear threshold, attack G, release G, attack alpha, release alpha
 */
typedef struct
{
    float ctfExp, thrlin, G_a, G_r, a_a, a_r; 
}VCAparams;

/** Internal Compressor State
 * * Not all members may be used
 * Members: TPT LPF state, previous y_s, previous y
 */ 
typedef struct 
{
    float s, y_s, y;
}VCAstate;

#pragma region internal helper functions
/**
 * g(x), the ctf gain function
 */
float VCA_gofx(float x, VCAparams* prm);

/**
 * TPT BF
 */ 
float VCA_BF_TPT(float x, VCAstate* ste, VCAparams* prm);

/**
 * Difference Equation BF
 */ 
float VCA_BF_DE(float x, VCAstate* ste, VCAparams* prm);
#pragma endregion 

#pragma region setup/reset
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
void VCA_reset(VCAstate* ste, VCAparams* prm);

#pragma endregion

#pragma region tick functions

/**
 * Process one sample through an FFVCA with a difference equation BF
 * @param ste initialize with malloc and resuse per sample
 * @param prm initialize with getParams and resuse per sample
 * @return y
 */ 
float VCA_tickFFVCA_DE(float x, VCAstate* ste, VCAparams* prm);

/**
 * Process one sample through an FFVCA implemented with a TPT BF
 * @param ste initialize with malloc and resuse per sample
 * @param prm initialize with getParams and resuse per sample
 * @return y
 */ 
float VCA_tickFFVCA_TPT(float x, VCAstate* ste, VCAparams* prm);

/**
 * Process one sample through an FBVCA implemented with a TPT BF and unit delay
 * @param ste initialize with malloc and resuse per sample
 * @param prm initialize with getParams and resuse per sample
 * @return y
 */ 
float VCA_tickFBVCA_TPT_z(float x, VCAstate* ste, VCAparams* prm);

#pragma endregion

#endif