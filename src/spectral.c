#include <stdlib.h>
#include <math.h>
#include <strings.h>

#include "spectral.h"

void make_sin_lut(unsigned int size, TransformInfo *info) {
    info->sin_lut = malloc(size * sizeof(float));
    info->sin_lut_mask = size - 1;
    info->cos_phase = 3 * (size >> 2); /* add this to sine phase to get cosine */
    for (int i = 0; i < size; ++i) {
        double phase = ((double) i) / size;
        info->sin_lut[i] = sin(phase);
    }
}

void initialize_response(TransformInfo *info, unsigned int size) {
    bzero(info, sizeof(TransformInfo));
    if (size) { make_sin_lut(size, info); }
}

void spectral_response_batch(float *data, unsigned int frequency, unsigned int batch_size, TransformInfo *info) {
    unsigned int mask = info->sin_lut_mask;
    unsigned int s_phase = info->phase;
    unsigned int c_phase = (s_phase + info->cos_phase) & mask;
    float *slut = info->sin_lut;
    float acc_cos = info->acc_cos;
    float acc_sin = info->acc_sin;
    float datum, coeff;
    for (unsigned int i = 0; i < batch_size; ++i) {
        datum = *data++;
        coeff = slut[s_phase];
        acc_sin += datum * coeff;
        coeff = slut[c_phase];
        acc_cos += datum * coeff;
        s_phase = (s_phase + frequency) & mask;
        c_phase = (c_phase + frequency) & mask;
    }
    info->acc_sin = acc_sin;
    info->acc_cos = acc_cos;
    info->phase = s_phase;
}

void spectral_response(float datum, unsigned int frequency, TransformInfo *info) {
    float coeff = info->sin_lut[info->phase];
    info->acc_sin += datum * coeff;
    coeff = info->sin_lut[(info->phase + info->cos_phase) & info->sin_lut_mask];
    info->acc_cos += datum * coeff;
    info->phase = (info->phase + frequency) & info->sin_lut_mask;
}
