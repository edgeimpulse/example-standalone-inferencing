#include <stdio.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "generate_features.h"

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

// Raw features copied from test sample
static const float features[] = {
    2.2615, -1.2709, -1.5310, 2.1025, -1.4384, -1.4031, 1.8575, -2.0358, -0.9304, 1.7774, -2.7517, 0.3182, 1.8792, -3.0094, 1.8410, 1.9026, -2.8831, 2.3525, 1.8738, -2.8714, 2.2074, 2.0008, -2.9105, 2.8453, 1.9929, -2.4173, 3.5912, 1.5519, -1.2252, 3.0806, 1.1849, 0.1161, 2.3051, 1.4242, 0.9274, 2.5686, 1.9112, 0.8813, 3.4047, 1.9870, 0.1282, 4.3301, 1.5070, -0.7181, 5.4090, 0.9140, -1.1361, 6.3098, 0.7397, -1.2497, 6.8393, 0.8268, -1.3164, 7.0282, 0.6083, -1.1242, 6.6068, 0.3094, -0.5559, 6.1057, 0.5823, -0.0069, 6.7162, 1.2555, 0.2977, 8.3345, 1.8160, 0.5934, 9.8815, 2.0112, 0.9141, 10.4275, 1.4789, 0.8772, 9.4144, 0.4708, 0.2389, 8.4699, 0.1038, -0.6782, 10.0246, 0.4754, -1.2428, 12.3875, 0.6151, -1.3195, 12.7334, 0.5633, -1.2882, 12.7709, 0.9170, -1.1872, 14.0230, 1.2009, -0.7961, 14.0445, 0.8453, -0.4823, 12.3121, 0.3345, -0.6260, 11.2288, 0.1541, -0.8480, 11.3188, 0.0136, -0.8334, 12.4755, -0.3057, -0.6905, 15.1147, -0.4449, -0.2404, 17.2741, -0.2191, 0.5482, 17.3598, -0.0105, 1.0141, 17.1428, -0.0932, 0.8421, 17.2065, -0.3790, 0.5037, 15.7316, -0.7227, 0.4437, 13.2414, -0.9063, 0.7939, 12.1108, -0.7694, 1.3010, 12.8724, -0.5830, 1.4031, 14.3897, -0.5976, 1.2585, 15.4923, -0.6076, 1.5174, 15.5334, -0.4850, 1.8672, 14.6135, -0.4205, 1.7167, 13.5438, -0.4341, 1.4842, 13.1528, -0.4239, 1.5298, 13.2142, -0.5283, 1.4052, 13.0367, -0.8312, 1.1062, 12.7165, -0.9905, 1.1229, 12.8252, -0.6786, 1.2898, 13.5372, -0.1767, 1.2266, 14.3738, -0.0718, 1.2234, 14.2866, -0.4614, 1.5123, 13.1728, -0.9315, 1.4993, 12.9001, -1.2120, 0.8387, 14.2384, -1.3566, 0.3014, 14.9237, -1.3860, 0.3457, 14.5927, -1.2337, 0.0924, 16.0729, -1.0087, -1.1481, 19.0082, -0.9894, -2.7288, 20.1977, -1.2007, -3.5556, 19.8356, -1.2981, -3.1508, 19.8973, -1.1989, -2.1653, 20.1241, -1.2711, -1.8519, 19.9587, -1.5489, -2.2753, 19.9186, -1.6038, -2.3325, 20.0318, -1.4019, -1.9211, 19.9960, -1.3239, -1.8144, 19.9587, -1.4358, -1.9219, 19.9934, -1.5225, -1.7260, 19.9972, -1.5328, -1.2526, 19.8594, -1.4491, -0.4893, 19.2232, -1.1844, 0.6651, 18.1180, -0.9336, 1.6133, 16.9012, -1.0255, 1.6879, 15.3008, -1.1910, 1.5504, 13.4217, -0.9222, 2.2561, 12.3834, -0.5439, 3.2475, 12.2141, -0.5516, 3.4748, 12.0264, -0.4701, 3.3978, 12.2797, 0.0103, 3.4823, 13.5753, 0.1295, 2.9618, 14.6148, -0.3798, 1.9225, 13.9828, -0.7505, 1.4368, 12.2055, -0.6069, 1.5336, 10.3638, -0.1557, 1.8727, 8.5183, 0.4401, 2.4654, 6.9458, 0.9261, 2.6875, 6.6406, 1.1295, 2.3409, 7.3068, 1.2045, 2.2741, 7.5564, 1.1810, 2.2731, 7.5104, 0.9740, 1.4571, 8.3492, 0.8595, 0.5641, 9.6884, 1.0652, 0.5586, 10.0485, 1.1353, 0.8193, 8.8262, 0.6172, 0.7405, 6.2959, 0.0497, 0.6449, 3.4332, 0.0999, 0.6522, 1.5945, 0.3260, 0.5700, 1.0083, 0.2644, 0.5160, 0.8999, 0.4448, 0.5765, 1.0761, 0.9392, 0.4222, 1.4438, 0.9640, -0.0999, 1.4932, 0.5854, -0.6445, 1.2325, 0.5637, -1.1276, 1.1424, 0.7684, -1.8676, 0.9644, 0.7036, -2.6427, 0.0292, 0.6762, -2.8877, -1.2929, 1.0588, -2.6958, -1.9917, 1.4539, -2.4798, -2.1536, 1.4949, -2.3525, -2.3996, 1.5169, -2.4603, -2.1235, 1.8402, -2.8588, -0.4655, 2.2004, -3.0973, 1.8680, 2.3519, -2.8367, 3.3700, 2.4865, -2.3378, 3.5492, 2.6664, -1.9647, 2.7589, 2.6538, -1.8690, 1.3123, 2.4390, -2.0833, -0.1799, 2.2590, -2.5202, -0.8908, 2.1845, -2.9884, -0.9705, 2.1387, -3.2615, -1.1455, 2.1508, -3.1901, -1.0878, 2.3609, -2.7997, -0.1149, 2.7334, -2.2703, 1.2139, 2.9857, -1.7261, 2.0952, 2.9190, -1.1197, 2.5633, 2.5440, -0.5247, 2.4569, 1.8843, -0.3296, 1.6828, 1.1457, -0.5549, 1.3490, 0.8498, -0.4394, 2.3850, 1.1798, 0.2970, 3.9527, 1.5644, 0.8075, 4.7139, 1.5408, 0.7467, 4.6502, 1.4347, 0.8873, 4.6029, 1.5399, 1.4712, 4.7297, 1.3995, 1.7285, 4.6881, 0.6678, 1.4361, 4.7076, -0.0351, 1.2555, 5.3458, 0.0939, 1.3489, 6.9531, 0.8411, 1.1851, 9.1885, 1.3384, 0.8314, 10.8726, 1.2933, 0.6672, 11.1840, 1.1335, 0.4342, 10.7777, 1.1396, 0.2567, 10.7148, 1.0750, 0.8343, 10.7674, 0.6978, 1.5227, 10.1537, 0.3281, 1.1296, 9.9377, 0.4075, 0.3923, 11.6245, 0.7095, 0.6872, 13.8548, 0.7505, 1.7325, 14.9274, 0.6337, 2.5645, 16.0870, 0.5565, 2.8033, 17.4762, 0.2079, 2.3170, 16.4470, -0.4712, 1.3606, 13.5403, -0.7977, 0.8917, 12.5325, -0.3901, 1.5005, 13.5425, 0.1489, 2.5395, 14.0121, 0.2330, 3.2369, 14.2319, 0.1275, 3.6788, 15.4590, 0.1977, 3.8922, 16.5498, 0.1781, 3.5058, 16.2547, -0.2217, 2.9745, 15.2112, -0.6177, 3.2160, 14.7816, -0.5309, 3.9755, 15.2514, -0.1897, 4.3931, 15.5889, -0.0471, 4.4617, 15.4202, -0.1194, 4.4209, 15.5960, -0.4056, 3.8625, 16.0701, -0.8635, 2.8584, 16.0585, -0.8665, 2.6022, 16.0255, -0.2172, 3.4084, 16.5615, 0.0780, 4.0463, 16.8153, -0.3924, 4.1561, 16.7714, -0.6647, 4.2208, 17.8629, -0.3481, 3.5429, 19.6062, -0.0698, 2.0416, 20.1495, -0.0187, 1.4168, 19.8326, -0.0191, 1.5608, 19.9382, -0.1946, 0.7003, 20.1605, -0.4830, -0.5049, 19.8695, -0.5526, -0.3933, 19.4449, -0.3904, 0.3226, 19.3245, -0.2462, 0.4758, 19.3102, -0.2286, 0.3962, 19.3997, -0.2660, 0.4700, 19.7385, -0.2725, 0.5038, 19.4905, -0.2588, 0.4644, 18.3207, -0.2793, 0.5402, 17.6197, -0.3268, 0.9928, 17.7350, -0.3514, 1.8963, 17.2038, -0.3177, 2.7914, 15.9172, -0.2531, 3.2655, 15.0393, -0.1920, 3.4269, 14.6802
};

int extract_undefined_features(signal_t *signal, matrix_t *output_matrix, void *config_ptr, const float frequency) {
    // void generate_features_cpp(const ei::matrix_t* input,
    //      ei::out_t* output, float sampling_freq, float scale_axes, float gravity_cutoff, int filter_order, const char * spectral_window)

    // ei_dsp_config_undefined_t ei_dsp_config_1_5 = {
    //     5, // uint32_t blockId
    //     1, // int implementationVersion
    //     3, // int length of axes
    //     1.0f, // float scale-axes
    //     0.7f, // float gravity-cutoff
    //     2, // int filter-order
    //     "hanning" // select spectral-window
    // };

    ei_dsp_config_undefined_t *config = (ei_dsp_config_undefined_t*)config_ptr;

    printf("signal->total_length = %d\n", (int)signal->total_length);
    printf("config->axes = %d\n", config->axes);

    matrix_t input_matrix(signal->total_length / config->axes, config->axes);
    if (!input_matrix.buffer) {
        EIDSP_ERR(EIDSP_OUT_OF_MEM);
    }

    signal->get_data(0, signal->total_length, input_matrix.buffer);

    for (size_t ix = 0; ix < input_matrix.rows * input_matrix.cols; ix++) {
        printf("%f ", input_matrix.buffer[ix]);
    }
    printf("\n");

    ei::numpy::transpose_in_place(&input_matrix);

    ei::out_t output(output_matrix);

    generate_features_cpp(
        &input_matrix,
        &output,
        frequency,
        config->scale_axes,
        config->gravity_cutoff,
        config->filter_order,
        config->spectral_window
    );

    return EIDSP_OK;
}

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
    res = run_classifier(&signal, &result, true);

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
