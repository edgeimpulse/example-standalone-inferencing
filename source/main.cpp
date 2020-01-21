#include <stdio.h>
#include <iostream>
#include <sstream>
#include "ei_run_classifier.h"

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Requires one parameter (a comma-separated list of raw features)\n");
        return 1;
    }

    std::string input = argv[1];
    std::istringstream ss(input);
    std::string token;

    std::vector<float> raw_features;

    while (std::getline(ss, token, ',')) {
        raw_features.push_back(std::stof(trim(token)));
    }

    ei_impulse_result_t result;

    signal_t signal;
    numpy::signal_from_buffer(&raw_features[0], raw_features.size(), &signal);

    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
    printf("run_classifier returned: %d\n", res);

    printf("Begin output\n");

    // print the predictions
    printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        printf(", ");
#else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            printf(", ");
        }
#endif
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    printf("%.3f", result.anomaly);
#endif
    printf("]\n");

    printf("End output\n");
}
