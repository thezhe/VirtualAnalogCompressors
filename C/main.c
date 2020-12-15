/**
 * Zhe Deng 2020
 * Example driver code for VCA using libsndfile
 * Given an infile and user parameters, outputs filtered sound with "_VCA" added to the name
 * * Sample Rate, Format, etc of output is the same as input
 * Compile (macOS): gcc -o main main.c VCA.c -lsndfile
 * Compile (Windows): gcc -o main main.c VCA.c -Iinclude -Llib -lsndfile
 * or (with libsndfile-1.dll): gcc -o main main.c VCA.c -Iinclude -Llib -lsndfile-1
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sndfile.h>
#include "VCA.h"

#define NFRAMES (1024) //frames per buffer

enum ARGS { ARG_NAME, ARG_INFILE, ARG_TYPE, ARG_THRDB, ARG_RATIO, ARG_ATTACK, ARG_RELEASE, ARG_NARGS};

/**
 * Concatenate the file name in a file path
 * @param file file path
 * @param cat string to concatenate
 * @return new file path
 */
char* catFileName(char* file, char* cat)
{
    char* ext = strrchr(file, '.'); //get pointer to last '.'
    if (ext == NULL) return NULL; //error
    else
    {
        //allocate newFile
        int newFileSize = strlen(file)+strlen(cat)+1;
        char* newFile = (char*) malloc(sizeof(char)*newFileSize);
        //add original file name
        int nameLen = ext-file;
        strncpy(newFile, file, nameLen);
        newFile[nameLen] = '\0';
        //add cat
        newFile = strncat(newFile, cat, strlen(cat));    
        //add ext
        newFile = strncat(newFile, ext, strlen(ext));
        return newFile;
    }
}


int main(int argc, char* argv[])
{
    //libsndfile
    SNDFILE * infile = NULL ;  // input sound file pointer
    SNDFILE * outfile = NULL;  // output sound file pointer
    SF_INFO   sfinfo;          // sound file info
    long nsamples;             // number of samples per block
    long readcount;            // no. of samples read
    float * buffer = NULL;     // buffer pointer (a dynamic array of floats)

    //VCA
    float (*VCA_tick)(float, VCAstate*, VCAparams*);

    //user parameters
    float thrdB, ratio, attack, release;
    int type;

    #pragma region user parameters, input validation
    //nargs
    if (argc!=ARG_NARGS)
    {
        printf("Usage: ./%s infile type thrdB ratio attack release\n", argv[ARG_NAME]);
        printf("type: 1=FFVCA_DE, 2=FFVCA_TPT, 2=FBVCA_TPT_z\n");
        return EXIT_FAILURE;
    }
    //number of non-narg errors
    int nErr = 0;
    //type
    type = atoi(argv[ARG_TYPE]);
    switch (type)
    {
    case 1:
        VCA_tick = VCA_tickFFVCA_DE;
        break;
    case 2:
        VCA_tick = VCA_tickFFVCA_TPT;
        break;
    case 3:
        VCA_tick = VCA_tickFBVCA_TPT_z;
        break;
    default:
        printf("Error: type must be in range [1,3]\n");
        ++nErr;
        break;
    }
    //thrdB
    thrdB = atof(argv[ARG_THRDB]);
    if (thrdB<-60.f || thrdB>0.f)
    {
        printf("Error: thrdB must be in range [-60, 0]\n");
        ++nErr;
    }
    //ratio
    ratio = atof(argv[ARG_RATIO]);
    if (ratio<1.f || ratio>1000.f)
    {
        printf("Error: ratio must be in range [1, 1000]\n");
        ++nErr;
    }
    //attack    
    attack = atof(argv[ARG_ATTACK]);
    if (attack<0.1f || attack>1000.f)
    {
        printf("Error: attack must be in range [0.1, 1000]\n");
        ++nErr;
    }
    //release
    release = atof(argv[ARG_RELEASE]);
    if (release<0.1f || release>1000.f)
    {
        printf("Error: release must be in range [0.1, 1000]\n");
        ++nErr;
    }
    //nErr
    if(nErr>0){ return EXIT_FAILURE; }
    #pragma endregion

    #pragma region infile, outfile, libsndfile
    //get infile and infile's sfinfo
    memset(&sfinfo, 0, sizeof(sfinfo));  // clear 
    if((infile = sf_open(argv[ARG_INFILE], SFM_READ, &sfinfo)) == NULL)
    {
        printf("Not able to open input file %s.\n", argv[ARG_INFILE]);
        puts(sf_strerror (NULL));
        return EXIT_FAILURE;
    }
    nsamples = sfinfo.channels * NFRAMES; // no. of samples per block
    buffer = (float *)malloc(nsamples * sizeof(float)); // used to save a block of samples
    
    char* outfilePath = catFileName(argv[ARG_INFILE], "_VCA");
    if((outfile = sf_open(outfilePath, SFM_WRITE, &sfinfo)) == NULL)
    {
        printf("Not able to open output file %s.\n", outfilePath);
        puts(sf_strerror(NULL));
        return EXIT_FAILURE;
    }

    #pragma endregion
    
    #pragma region VCA internals
    //Internal Parameters
    VCAparams* prm = VCA_getParams(thrdB, ratio, attack, release, 1);
    //Internal State
    VCAstate* ste = (VCAstate*) malloc(sizeof(VCAstate));
    #pragma endregion

    // processing
    while((readcount = sf_read_float(infile, buffer, nsamples)) > 0){
        for (int i=0; i<readcount; ++i)
            buffer[i]=VCA_tick(buffer[i], ste, prm);
        sf_write_float(outfile, buffer, readcount);
    }
    
    #pragma region free memory
    sf_close(infile);
    sf_close(outfile); 
    VCA_reset(ste, prm); 
    free(buffer);
    free(outfilePath);
    #pragma endregion free memory
    
    return EXIT_SUCCESS;
}