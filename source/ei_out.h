#pragma once
#include "edge-impulse-sdk/porting/ei_logging.h"

namespace ei {
class out_t {
    ei::matrix_t* matrix;
    uint32_t count = 0;
public:
    out_t(ei::matrix_t* m) : matrix(m) {}
    void push_back(float value) {
        if (count >= matrix->rows * matrix->cols) {
            EI_LOGE("Output matrix overflow: count=%d, rows=%d, cols=%d", count, matrix->rows, matrix->cols);
        } else {
            matrix->buffer[count++] = value;
        }
    }
};
} // namespace ei
