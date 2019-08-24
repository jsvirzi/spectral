#include <strings.h>

#include <sndfile.h>

#include <TApplication.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

#include "spectral.h"

constexpr unsigned int MaxFrames = 4096;
constexpr unsigned int MaxSamples = 1024 * 1024 * 4;
constexpr unsigned int frequencies = 8;

constexpr unsigned int MaxGraphColors = 6;
unsigned int graph_colors[MaxGraphColors] = { kBlack, kRed, kBlue, 38, 8, 29 };
constexpr unsigned int MaxGraphStyles = 6;
unsigned int graph_styles[MaxGraphStyles] = { kSolid, kDotted, kDashDotted, kDashed };

enum {
    AudioChannelL = 0,
    AudioChannelR,
    AudioChannels
};

float audio_data[AudioChannels][MaxSamples];
float audio_energy[AudioChannels][frequencies][MaxFrames];
float time_axis[MaxFrames];

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
    unsigned int read_count, sample_index = 0, frame_index = 0;
    float *p_data, *data = new float[sfinfo.channels * analysis_frame_size];
    TransformInfo transform_info[AudioChannels][frequencies], master_transform_info, *info;
    initialize_response(&master_transform_info, 1000, sfinfo.samplerate);
    for (int k = 0; k < frequencies; ++k) {
        copy_response(&transform_info[AudioChannelL][k], &master_transform_info);
        copy_response(&transform_info[AudioChannelR][k], &master_transform_info);
        transform_info[AudioChannelL][k].frequency = (k + 1) * 1000; /* override */
        transform_info[AudioChannelR][k].frequency = (k + 1) * 1000; /* override */
    }
    while ((read_count = sf_readf_float(sndfile, data, analysis_frame_size)) == analysis_frame_size) {
        unsigned int analysis_index = sample_index; /* snapshot of where we are */
        for (int k = 0; k < analysis_frame_size; ++k, ++sample_index) { /* buffer samples into large holding tank */
            if (sample_index >= MaxSamples) {
                printf("insufficient space for samples allocated. using %d/%d\n", sample_index, MaxSamples);
                return 1;
            }
            audio_data[AudioChannelL][sample_index] = data[2 * k + 1];
            audio_data[AudioChannelR][sample_index] = data[2 * k];
        }
        for (int k = 0; k < frequencies; ++k) {
            info = &transform_info[AudioChannelL][k]; /* left channel */
            info->acc_cos = info->acc_sin = 0; /* clear accumulators for this time slot */
            p_data = &audio_data[AudioChannelL][analysis_index];
            spectral_response_batch(p_data, analysis_frame_size, info);
            audio_energy[AudioChannelL][k][frame_index] = info->acc_sin * info->acc_sin + info->acc_cos * info->acc_cos;

            info = &transform_info[AudioChannelR][k]; /* right channel */
            info->acc_cos = info->acc_sin = 0; /* clear accumulators for this time slot */
            p_data = &audio_data[AudioChannelR][analysis_index];
            spectral_response_batch(p_data, analysis_frame_size, info);
            audio_energy[AudioChannelL][k][frame_index] = info->acc_sin * info->acc_sin + info->acc_cos * info->acc_cos;
        }

        time_axis[frame_index] = frame_index;

        if ((++frame_index & 15) == 15) { printf("\n"); }

        if (frame_index >= MaxFrames) {
            printf("insufficient space for frames allocated. using %d / %d\n", frame_index, MaxFrames);
            return 0;
        }
    }

    TApplication theApp("the_app", &argc, argv);
    TCanvas *canvas = new TCanvas("the_canvas", "the_canvas", 800, 600);
    TGraphErrors *graph_energy[AudioChannels][frequencies], *g;

    int color_index = 0, style_index = 0;
    for (int k = 0; k < frequencies; ++k) {
        graph_energy[AudioChannelL][k] = new TGraphErrors(frame_index, time_axis, audio_energy[AudioChannelL][k]);
        graph_energy[AudioChannelR][k] = new TGraphErrors(frame_index, time_axis, audio_energy[AudioChannelR][k]);
        g = graph_energy[AudioChannelL][k];
        g->SetLineColor(graph_colors[color_index]);
        color_index = (color_index + 1) % MaxGraphColors;
        g->SetLineStyle(graph_styles[style_index]);
        style_index = (style_index + 1) % MaxGraphStyles;
        g->SetLineWidth(3);
        g = graph_energy[AudioChannelR][k];
        g->SetLineColor(graph_colors[color_index]);
        color_index = (color_index + 1) % MaxGraphColors;
        g->SetLineStyle(graph_styles[style_index]);
        style_index = (style_index + 1) % MaxGraphStyles;
        if (k == 0) { graph_energy[AudioChannelL][k]->Draw("ALP"); }
        graph_energy[AudioChannelL][k]->Draw("LP");
        graph_energy[AudioChannelR][k]->Draw("LP");
    }
    canvas->Draw();
    canvas->Update();
    canvas->WaitPrimitive();

    printf("%d samples read. %d frames\n", sample_index, frame_index);
    sf_close(sndfile);
    return 0;
}
