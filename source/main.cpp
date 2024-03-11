#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample
static const float features[] = {
    // Copy raw features here (e.g. from the 'Model testing' page)
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

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

    // Perform DSP pre-processing and inference
    res = run_classifier(&signal, &result, false);

    // Print return code and how long it took to perform inference
    ei_printf("run_classifier returned: %d\r\n", res);
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);

    // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
        auto bb = result.bounding_boxes[ix];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }

    // Print the prediction results (classification)
#elif EI_CLASSIFIER_LABEL_COUNT > 1 // if there is only one label, this is an anomaly only
    ei_printf("Predictions:\r\n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: ", result->classification[ix].label);
        ei_printf_float(result->classification[ix].value);
        ei_printf("\n");
    }
#endif
#if EI_CLASSIFIER_HAS_ANOMALY == 3 // visual AD
    ei_printf("Visual anomaly grid results:\r\n");
    for (uint32_t i = 0; i < result.visual_ad_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.visual_ad_grid_cells[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("    %s (", bb.label);
        ei_printf_float(bb.value);
        ei_printf(") [ x: %u, y: %u, width: %u, height: %u ]\n", bb.x, bb.y, bb.width, bb.height);
    }
    ei_printf("Visual anomaly values: Mean %.3f Max %.3f\r\n", result.visual_ad_result.mean_value, result.visual_ad_result.max_value);
#elif (EI_CLASSIFIER_HAS_ANOMALY > 0) // except for visual AD
    ei_printf("Anomaly prediction: %.3f\r\n", result->anomaly);
#endif

    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features + offset)[i];
    }

    return EIDSP_OK;
}
