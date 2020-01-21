#ifndef _SANITY_CHECK_EI_SAMPLER_H
#define _SANITY_CHECK_EI_SAMPLER_H

class EdgeSampler {
public:
    /**
     * Writes sensor data to the context, does not clear the buffer passed in!
     */
    int write_sensor_data(float *values, size_t values_size) {
        return 0;
    }
};

#endif // _SANITY_CHECK_EI_SAMPLER_H