#include <stdio.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/classifier/ei_aligned_malloc.h"
#include "edge-impulse-sdk/classifier/inferencing_engines/tflite_helper.h"

// Raw features copied from test sample
static const float features[] = {
};

int main(int argc, char **argv) {

    TfLiteStatus status;

    status = trained_model_init(ei_aligned_calloc);
    if (status != kTfLiteOk) {
        printf("err %d\n", status);
        return 1;
    }

    signal_t signal;
    int err = numpy::signal_from_buffer(features, sizeof(features) / sizeof(features[0]), &signal);
    if (err != 0) {
        printf("err2 %d\n", err);
        return 1;
    }

    TfLiteTensor input;
    trained_model_input(0, &input);
    EI_IMPULSE_ERROR input_res = fill_input_tensor_from_signal(&signal, &input);
    if (input_res != 0) {
        printf("err %d\n", input_res);
        return 1;
    }

    status = trained_model_invoke();
    if (status != kTfLiteOk) {
        printf("err %d\n", status);
        return 1;
    }

    TfLiteTensor output;
    trained_model_output(0, &output);

    if (output.type == kTfLiteInt8) {
        int8 *data = output.data.int8;
        auto zero_point = output.params.zero_point;
        auto scale = output.params.scale;
        for (uint32_t ix = 0; ix < 2; ix++) {
            float value = static_cast<float>(data[ix] - zero_point) * scale;
            printf("%f ", value);
        }
        printf("\n");
    }
    else {
        float *data = output.data.f;
        for (uint32_t ix = 0; ix < 2; ix++) {
            printf("%f ", data[ix]);
        }
        printf("\n");
    }

    return EIDSP_OK;
}
