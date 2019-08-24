#include <strings.h>

#include <sndfile.h>

#include <TGraphErrors.h>

#include "spectral.h"

constexpr unsigned int MaxFrames = 1024 * 1024;
enum {
    AudioChannelL = 0,
    AudioChannelR,
    AudioChannels
};

float audio_data[AudioChannels][MaxFrames];

constexpr unsigned int frequencies = 8;

int main(int argc, char **argv) {
    char ifile[256];
    snprintf(ifile, sizeof(ifile), "/home/jsvirzi/Downloads/audio-eabec5f4c348a2c3-1592b15cfef8386f.wav");
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open (ifile, SFM_READ, &sfinfo);
    unsigned int analysis_frame_size = sfinfo.samplerate * 50 / 1000; /* ex: 48KHz * 50ms / 1000ms = 2400 samples */
    printf("sound file info: %d channels sampled at %d Hz. analysis frame size = %d\n",
        sfinfo.channels, sfinfo.samplerate, analysis_frame_size);
    if (sndfile != NULL) {
        printf("success opening file [%s]\n", ifile);
    } else {
        printf("error opening file [%s]\n", ifile);
        return 1;
    }
    unsigned int read_count, sample_index = 0, frame_index;
    float *p_data, *data = new float[sfinfo.channels * analysis_frame_size];
    TransformInfo transform_info[AudioChannels][frequencies], master_transform_info;
    initialize_response(&master_transform_info, 1000, sfinfo.samplerate);
    for (int k = 0; k < frequencies; ++k) {
        copy_response(&transform_info[AudioChannelL][k], &master_transform_info);
        copy_response(&transform_info[AudioChannelR][k], &master_transform_info);
        transform_info[AudioChannelL][k].frequency = (k + 1) * 1000; /* override */
        transform_info[AudioChannelR][k].frequency = (k + 1) * 1000; /* override */
    }
    while ((read_count = sf_readf_float(sndfile, data, analysis_frame_size)) == analysis_frame_size) {
        unsigned int analysis_index = sample_index; /* snapshot of where we are */
        for (int k = 0; k < analysis_frame_size; ++k, ++sample_index) {
            audio_data[AudioChannelL][sample_index] = data[2 * k + 1];
            audio_data[AudioChannelR][sample_index] = data[2 * k];
        }
        for (int k = 0; k < 4; ++k) {
            p_data = &audio_data[AudioChannelL][analysis_index];
            spectral_response_batch(p_data, analysis_frame_size, &transform_info[AudioChannelL][k]);
            p_data = &audio_data[AudioChannelR][analysis_index];
            spectral_response_batch(p_data, analysis_frame_size, &transform_info[AudioChannelR][k]);
        }
        if ((++frame_index & 15) == 15) { printf("\n"); }
    }
    printf("%d samples read\n", sample_index);
    sf_close(sndfile);
    return 0;
}
