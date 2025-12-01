#include <stdio.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample
static const float features[] = {
    // Copy raw features here (e.g. from the 'Live classification' page)
};

int main(int argc, char **argv) {

    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

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

#if EI_CLASSIFIER_FREEFORM_OUTPUT
    // for "freeform" outputs, the application needs to allocate the memory (one matrix_t per output tensor)
    std::vector<matrix_t> freeform_outputs;
    freeform_outputs.reserve(ei_default_impulse.impulse->freeform_outputs_size);
    for (size_t ix = 0; ix < ei_default_impulse.impulse->freeform_outputs_size; ++ix) {
        freeform_outputs.emplace_back(ei_default_impulse.impulse->freeform_outputs[ix], 1);
    }
    ei_set_freeform_output(freeform_outputs.data(), freeform_outputs.size());
#endif // EI_CLASSIFIER_FREEFORM_OUTPUT

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    // Perform DSP pre-processing and inference
    res = run_classifier(&signal, &result, false);

    // Print return code and how long it took to perform inference
    ei_printf("run_classifier returned: %d\r\n", res);
    if (res != 0) {
        return 1;
    }

    // See edge-impulse-sdk/classifier/ei_print_results.h
    ei_print_results(&ei_default_impulse, &result);

    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features + offset)[i];
    }

    return EIDSP_OK;
}
