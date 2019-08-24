#ifndef SPECTRAL_H
#define SPECTRAL_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TransformInfo {
    unsigned int frequency;
    float acc_cos, acc_sin;
    unsigned int phase;
    float *sin_lut;
    unsigned int cos_phase;
    unsigned int sin_lut_mask;
    unsigned int sin_lut_size;
} TransformInfo;

void initialize_response(TransformInfo *info, unsigned int frequency, unsigned int size);
void spectral_response_batch(float *data, unsigned int batch_size, TransformInfo *info);
void spectral_response(float datum, TransformInfo *info);
void copy_response(TransformInfo *dst, TransformInfo *src);

#ifdef __cplusplus
}
#endif

#endif
