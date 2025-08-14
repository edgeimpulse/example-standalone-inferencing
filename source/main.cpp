#include <stdio.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// Raw features copied from test sample
static const float features[] = {
    1, 2, 3, 4, 5, 6, 7, 8
};

int main(int argc, char **argv) {

    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result = { 0 }; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

    // alloc freeform outputs (one matrix_t per output tensor)
    std::vector<matrix_t> freeform_outputs;
    freeform_outputs.reserve(ei_default_impulse.impulse->freeform_outputs_size);
    for (size_t ix = 0; ix < ei_default_impulse.impulse->freeform_outputs_size; ++ix) {
        freeform_outputs.emplace_back(ei_default_impulse.impulse->freeform_outputs[ix], 1);
    }

    result.freeform_outputs = freeform_outputs.data();

    // Calculate the length of the buffer
    size_t buf_len = sizeof(features) / sizeof(features[0]);

    // Make sure that the length of the buffer matches expected input length
    if (buf_len != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_printf("ERROR: The size of the input buffer is not correct.\r\n");
        ei_printf("Expected %d items, but got %d\r\n",
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE,
                (int)buf_len);
        return 1;
    }

    run_classifier_init();

    numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    // Perform DSP pre-processing and inference
    res = run_classifier(&signal, &result, false);

    // Print return code and how long it took to perform inference
    ei_printf("run_classifier returned: %d\r\n", res);
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);

    for (size_t ix = 0; ix < freeform_outputs.size(); ix++) {
        printf("Freeform output index=%d\n", (int)ix);
        printf("  ");
        const matrix_t& freeform_output = freeform_outputs[ix];
        for (size_t jx = 0; jx < freeform_output.rows * freeform_output.cols; jx++) {
            printf("%f ", freeform_output.buffer[jx]);
        }
        printf("\n");
    }

    run_classifier_deinit();

    return 0;
}
