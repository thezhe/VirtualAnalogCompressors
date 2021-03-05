/** Zhe Deng 2020
 */  

#include "VCA.h"

#pragma region internal helper functions
float VCA_gofx(float x, VCAparams* prm)
{ 
    return (x>prm->thrlin)? powf(x/prm->thrlin, prm->ctfExp) :1.f; 
}

float VCA_BF_TPT(float x, VCAstate* ste, VCAparams* prm)
{
    float v = (x-ste->s)*(x<ste->y_s? prm->G_r : prm->G_a);
    ste->y_s = v + ste->s;
    ste->s = ste->y_s + v;
    return ste->y_s;
}

float VCA_BF_DE(float x, VCAstate* ste, VCAparams* prm)
{
    float a = (x<ste->y_s)?prm->a_r:prm->a_a;
    ste->y_s=a*ste->y_s+(1.f-a)*x;
    return ste->y_s;
}
#pragma endregion

#pragma region setup/reset
 
VCAparams* VCA_getParams(float thrdB, float ratio, float attack, float release, float sampleRate)
{
    VCAparams* prm = (VCAparams*) malloc(sizeof(VCAparams));
    //thrlin
    prm->thrlin = powf(10.f, thrdB/20.f);
    //ctfExp
    prm->ctfExp = (1.f/ratio)-1;
    float halfT = 0.5f/sampleRate;
    //G_a
    float w_a = 1000.f/attack;
    float g_a = tan(w_a*halfT);
    prm->G_a = g_a/(1.f+g_a); 
    //G_r
    float w_r = 1000.f/release;
    float g_r = tan(w_a*halfT);
    prm->G_r = g_r/(1.f+g_r);
    //a_a
    prm->a_a= exp(-w_a/sampleRate);
    //a_r
    prm->a_r=exp(-w_r/sampleRate);
    return prm;
}

void VCA_reset(VCAstate* ste, VCAparams* prm)
{
    free(ste);
    free(prm);
}
#pragma endregion

#pragma region tick functions
float VCA_tickFFVCA_DE(float x, VCAstate* ste, VCAparams* prm)
{
    float x_rect = fabs(x); //rectifier
    float x_s = VCA_BF_DE(x_rect, ste, prm); //BF_DE
    return x*VCA_gofx(x_s, prm); //CTF

}

float VCA_tickFFVCA_TPT(float x, VCAstate* ste, VCAparams* prm)
{
    float x_rect = fabs(x); //rectifier
    float x_s = VCA_BF_TPT(x_rect, ste, prm); //BF_TPT
    return x*VCA_gofx(x_s, prm); //CTF
}


float VCA_tickFBVCA_TPT_z(float x, VCAstate* ste, VCAparams* prm)
{
    float y_rect = fabs(ste->y); //rectifier
    float y_s = VCA_BF_TPT(y_rect, ste, prm); //BF_TPT
    ste->y = x*VCA_gofx(y_s, prm); //CTF
    return ste->y;
}
#pragma endregion