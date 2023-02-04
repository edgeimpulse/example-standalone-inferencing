#include <stdio.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/classifier/ei_aligned_malloc.h"
#include "edge-impulse-sdk/classifier/inferencing_engines/tflite_helper.h"

// Raw features copied from test sample
static const float features[] = {
    4.7166, -0.2304, -0.6355, 2.3096, -1.0533, 2.9051, 1.8819, 1.0123, 1.4356, 1.6334, 1.4141, 1.3300, 1.7379, 1.5096, 1.3240, 1.3746, 1.1512, 0.9478, 3.6480, -0.3054, -1.1880, 2.6599, 2.2823, 1.3543, 0.7280, 0.6851, -0.0135, 0.5501, 1.3194, 0.9561, 1.5727, 1.4272, 0.9956, 0.7409, -0.2793, 1.2962, 2.0682, 0.6380, 2.1843, 0.9328, 1.5005, 0.0667, 0.9264, 1.3629, 1.4076, 1.3384, 0.7976, 1.3545, 1.2524, 1.0228, 1.0042, 1.2554, -0.7833, 1.3597
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
    int8 *data = output.data.int8;
    auto zero_point = output.params.zero_point;
    auto scale = output.params.scale;
    for (uint32_t ix = 0; ix < 4; ix++) {
        float value = static_cast<float>(data[ix] - zero_point) * scale;
        printf("%f ", value);
    }
    printf("\n");

    return EIDSP_OK;
}
