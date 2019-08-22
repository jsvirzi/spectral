#ifndef SPECTRAL_H
#define SPECTRAL_H

typedef struct TransformInfo {
    float acc_cos, acc_sin;
    unsigned int phase;
    float *sin_lut;
    unsigned int cos_phase;
    unsigned int sin_lut_mask;
} TransformInfo;

void initialize_response(TransformInfo *info, unsigned int size);
void spectral_response_batch(float *data, unsigned int frequency, unsigned int batch_size, TransformInfo *info);
void spectral_response(float datum, unsigned int frequency, TransformInfo *info);

#endif
