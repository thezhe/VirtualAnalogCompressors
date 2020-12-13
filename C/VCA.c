/** Zhe Deng 2020
 *  VCA Compressors 
 */  

#include "VCA.h"
 
VCAparams* VCA_getParams(float thrdB, float ratio, float attack, float release, float sampleRate)
{
    VCAparams* p = (VCAparams*) malloc(sizeof(VCAparams));
    //thrlin
    p->thrlin = powf(10.f, thrdB/20.f);
    //ctfExp
    p->ctfExp = (1.f/ratio)-1;
    float halfT = 0.5f/sampleRate;
    //G_a
    float w_a = 1000.f/attack;
    float g_a = tan(w_a*halfT);
    p->G_a = g_a/(1.f+g_a); 
    //G_r
    float w_r = 1000.f/release;
    float g_r = tan(w_a*halfT);
    p->G_r = g_r/(1.f+g_r);
    return p;
}


void VCA_reset(VCAstate* ste, VCAparams* p)
{
    free(ste);
    free(p);
}

float VCA_gofx(float x, VCAparams* p)
{ 
    return (x>p->thrlin)? powf(x/p->thrlin, p->ctfExp) :1.f; 
}


float VCA_BF(float x, VCAstate* ste, VCAparams* p)
{
    float v = (x-ste->s)*(x<ste->y? p->G_r : p->G_a);
    ste->y = v + ste->s;
    ste->s = ste->y + v;
    return ste->y;
}

float VCA_tickFFVCA(float x, VCAstate* ste, VCAparams* prm)
{
    float x_rect = fabs(x); //rectifier
    float x_s = VCA_BF(x_rect, ste, prm); //BF
    return x*VCA_gofx(x_s, prm); //CTF
}
