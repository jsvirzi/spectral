#include <strings.h>

#include <sndfile.h>

#include <TGraphErrors.h>

#include "spectral.h"

int main(int argc, char **argv) {
    char ifile[256];
    snprintf(ifile, sizeof(ifile), "/home/jsvirzi/Downloads/audio-eabec5f4c348a2c3-1592b15cfef8386f.wav");
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open (ifile, SFM_READ, &sfinfo);
    printf("sound file info: %d channels sampled at %d Hz\n", sfinfo.channels, sfinfo.samplerate);
    if (sndfile != NULL) {
        printf("success opening file [%s]\n", ifile);
    } else {
        printf("error opening file [%s]\n", ifile);
        return 1;
    }
    int read_count;
#define BUFFER_LEN (512)
    float data[BUFFER_LEN];
    int channels = 1;
    int frames = 1;
    int idx = 0;
    while ((read_count = sf_readf_float(sndfile, data, 1))) {
        printf("%10f ", data[0]);
        ++idx;
        if ((idx & 15) == 15) { printf("\n"); }
    }
    printf("%d samples read\n", idx);
    sf_close(sndfile);
    return 0;
}
