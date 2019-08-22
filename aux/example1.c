#include <strings.h>

#include <sndfile.h>

#include "spectral.h"

#if 0

static void add_instrument_data(SNDFILE *file, const SF_INFO *info)
{
    SF_INSTRUMENT instr ;

    bzero (&instr, sizeof (instr)) ;

    instr.gain = 1 ;
    instr.basenote = 0 ;
    instr.detune = 0 ;
    instr.velocity_lo = 0 ;
    instr.velocity_hi = 0 ;
    instr.key_lo = 0 ;
    instr.key_hi = 0 ;
    instr.loop_count = 1 ;

    instr.loops [0].mode = SF_LOOP_FORWARD ;
    instr.loops [0].start = 0 ;
    instr.loops [0].end = info->frames ;
    instr.loops [0].count = 0 ;

    if (sf_command (file, SFC_SET_INSTRUMENT, &instr, sizeof (instr)) == SF_FALSE)
    {
        printf ("\n\nLine %d : sf_command (SFC_SET_INSTRUMENT) failed.\n\n", __LINE__) ;
        return;
    };
}

#endif

int main(int argc, char **argv) {
    char ifile[256];
    snprintf(ifile, sizeof(ifile), "/home/jsvirzi/Downloads/audio-eabec5f4c348a2c3-1592b15cfef8386f.wav");
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open (ifile, SFM_READ, &sfinfo);
//    SF_INSTRUMENT instrument;
//    bzero(&instrument, sizeof(SF_INSTRUMENT));
    if (sndfile != NULL) {
        printf("success opening file [%s]\n", ifile);
    } else {
        printf("error opening file [%s]\n", ifile);
        return 1;
    }
    // add_instrument_data(sndfile, &sfinfo);
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
