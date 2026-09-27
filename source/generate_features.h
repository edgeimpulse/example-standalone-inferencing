#pragma once
#if defined(EI_VECTOR_OUT)
#include "../pybind/dsp/numpy_types.h"
#else
#include "edge-impulse-sdk/dsp/numpy_types.h"
#include "ei_out.h"
#endif
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstring>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace custom_spectral {
constexpr double custom_spectral_pi = 3.14159265358979323846;
using custom_spectral_section = std::array<double, 5>; // b0, b1, b2, a1, a2

inline double custom_spectral_mean(const std::vector<double>& x) {
    return std::accumulate(x.begin(), x.end(), 0.0) / x.size();
}

inline std::vector<custom_spectral_section> custom_spectral_butter(double fs, double cutoff, int order) {
    // Bilinear transform of the normalized analog Butterworth poles.
    const double w = std::tan(custom_spectral_pi * cutoff / fs);
    std::vector<std::complex<double>> poles;
    double gain = 1.0;
    for (int k = 0; k < order; ++k) {
        const auto analog = std::polar(w, custom_spectral_pi * (2*k + order + 1) / (2*order));
        const auto pole = (1.0 + analog) / (1.0 - analog);
        gain *= std::abs((1.0 - pole) / 2.0);
        if (k < (order + 1) / 2) poles.push_back(pole);
    }
    std::sort(poles.begin(), poles.end(), [](const std::complex<double>& a, const std::complex<double>& b) { return std::abs(a) < std::abs(b); });
    std::vector<custom_spectral_section> sections;
    for (size_t i = 0; i < poles.size(); ++i) {
        const bool real = std::abs(poles[i].imag()) < 1e-14;
        const bool single_zero = (order % 2) && i == poles.size() - 1;
        sections.push_back({1.0, single_zero ? 1.0 : 2.0, single_zero ? 0.0 : 1.0,
                            real ? -poles[i].real() : -2.0*poles[i].real(), real ? 0.0 : std::norm(poles[i])});
    }
    for (int i = 0; i < 3; ++i) sections[0][i] *= gain;
    return sections;
}

inline void custom_spectral_filter_pass(std::vector<double>& x, const std::vector<custom_spectral_section>& sos) {
    const double endpoint = x.front();
    double scale = 1.0;
    for (const auto& s : sos) {
        const double dc = (s[0]+s[1]+s[2]) / (1.0+s[3]+s[4]);
        double z1 = (dc-s[0])*scale*endpoint;
        double z2 = (s[2]-s[4]*dc)*scale*endpoint;
        scale *= dc;
        for (double& value : x) {
            const double y = s[0]*value + z1;
            z1 = s[1]*value - s[3]*y + z2;
            z2 = s[2]*value - s[4]*y;
            value = y;
        }
    }
}

inline std::vector<double> custom_spectral_filter(const std::vector<double>& x, double fs, double cutoff, int order) {
    if (x.size() < 12) return x;
    const auto sos = custom_spectral_butter(fs, cutoff, order);
    const size_t pad = std::min(x.size()-1, 3*(2*sos.size()+1));
    std::vector<double> extended;
    extended.reserve(x.size()+2*pad);
    for (size_t i = pad; i > 0; --i) extended.push_back(2*x.front()-x[i]);
    extended.insert(extended.end(), x.begin(), x.end());
    for (size_t i = 1; i <= pad; ++i) extended.push_back(2*x.back()-x[x.size()-1-i]);
    custom_spectral_filter_pass(extended, sos);
    std::reverse(extended.begin(), extended.end());
    custom_spectral_filter_pass(extended, sos);
    std::reverse(extended.begin(), extended.end());
    return std::vector<double>(extended.begin()+pad, extended.end()-pad);
}

inline double custom_spectral_percentile(const std::vector<double>& sorted, double q) {
    const double index = q*(sorted.size()-1);
    const size_t lower = static_cast<size_t>(index);
    return sorted[lower] + (index-lower)*(sorted[std::min(lower+1, sorted.size()-1)]-sorted[lower]);
}

inline void custom_spectral_statistics(const std::vector<double>& x, ei::out_t* output) {
    const double mean = custom_spectral_mean(x);
    double variance = 0, square = 0, fourth = 0, mad = 0;
    for (double v : x) {
        const double d = v-mean;
        variance += d*d; square += v*v; fourth += d*d*d*d; mad += std::abs(d);
    }
    variance /= x.size();
    auto sorted = x;
    std::sort(sorted.begin(), sorted.end());
    const double values[] = {mean, std::sqrt(variance), std::sqrt(square/x.size()), sorted.front(), sorted.back(),
        custom_spectral_percentile(sorted, .5), custom_spectral_percentile(sorted, .1), custom_spectral_percentile(sorted, .9),
        custom_spectral_percentile(sorted, .75)-custom_spectral_percentile(sorted, .25), mad/x.size(),
        std::sqrt(fourth/x.size())/(variance+1e-12)};
    for (double v : values) output->push_back(static_cast<float>(v));
}

inline void custom_spectral_fft(std::vector<std::complex<double>>& x) {
    const size_t n = x.size();
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(x[i], x[j]);
    }
    for (size_t length = 2; length <= n; length *= 2) {
        const auto step = std::polar(1.0, -2*custom_spectral_pi/length);
        for (size_t i = 0; i < n; i += length) {
            std::complex<double> w(1, 0);
            for (size_t j = 0; j < length/2; ++j) {
                const auto a = x[i+j], b = w*x[i+j+length/2];
                x[i+j] = a+b; x[i+j+length/2] = a-b; w *= step;
            }
        }
    }
}

inline void custom_spectral_spectrum(const std::vector<double>& x, double fs, const char* window, ei::out_t* output) {
    size_t n = 1;
    while (n < x.size()) n *= 2;
    std::vector<std::complex<double>> fft(n);
    const double mean = custom_spectral_mean(x);
    for (size_t i = 0; i < x.size(); ++i) {
        double w = 1.0;
        if (x.size() > 1) {
            const double angle = 2*custom_spectral_pi*i/(x.size()-1);
            if (std::strcmp(window, "hanning") == 0) w = .5-.5*std::cos(angle);
            else if (std::strcmp(window, "hamming") == 0) w = .54-.46*std::cos(angle);
            else if (std::strcmp(window, "blackman") == 0) w = .42-.5*std::cos(angle)+.08*std::cos(2*angle);
        }
        fft[i] = (x[i]-mean)*w;
    }
    custom_spectral_fft(fft);
    const size_t start = n > 1 ? 1 : 0, count = n/2+1-start;
    double total = 1e-12;
    for (size_t i = start; i <= n/2; ++i) total += std::norm(fft[i]);
    double bands[5] = {}, centroid = 0, entropy = 0, peak = -1, dominant = 0;
    const double edges[] = {0, .5, 2, 5, 10, 20};
    for (size_t i = start; i <= n/2; ++i) {
        const double p = std::norm(fft[i])/total, f = i*fs/n;
        for (size_t b = 0; b < 5; ++b) if (f >= edges[b] && f < edges[b+1]) bands[b] += p;
        centroid += f*p; entropy -= p*std::log(p+1e-12);
        if (p > peak) { peak = p; dominant = f; }
    }
    for (double b : bands) output->push_back(static_cast<float>(b));
    output->push_back(static_cast<float>(dominant));
    output->push_back(static_cast<float>(centroid));
    output->push_back(static_cast<float>(count <= 1 ? 0 : entropy/std::log(count)));
    output->push_back(static_cast<float>(peak));
}
} // namespace custom_spectral

inline void generate_features_cpp(const ei::matrix_t* input, ei::out_t* output, float sampling_freq, float scale_axes, float gravity_cutoff, int filter_order, const char* spectral_window) {
    using namespace custom_spectral;
    if (!input || !input->rows || !input->cols) throw std::invalid_argument("raw_data must contain complete samples for every axis");
    if (sampling_freq <= 0) throw std::invalid_argument("sampling_freq must be greater than zero");
    if (gravity_cutoff <= 0 || gravity_cutoff >= sampling_freq/2) throw std::invalid_argument("gravity_cutoff must be between zero and the Nyquist frequency");
    if (filter_order < 1 || filter_order > 4) throw std::invalid_argument("filter_order must be between 1 and 4");
    if (!spectral_window || (std::strcmp(spectral_window,"hanning") && std::strcmp(spectral_window,"hamming") && std::strcmp(spectral_window,"blackman") && std::strcmp(spectral_window,"rectangular")))
        throw std::invalid_argument("spectral_window must be hanning, hamming, blackman, or rectangular");
    const size_t axes = input->rows, samples = input->cols;
    std::vector<std::vector<double>> motion(axes, std::vector<double>(samples));
    std::vector<double> magnitude(samples, 0), motion_magnitude(samples, 0), means(axes), deviations(axes);
    for (size_t a = 0; a < axes; ++a) {
        std::vector<double> values(samples);
        for (size_t i = 0; i < samples; ++i) values[i] = double(input->buffer[a*samples+i])*scale_axes;
        const auto gravity = custom_spectral_filter(values, sampling_freq, gravity_cutoff, filter_order);
        for (size_t i = 0; i < samples; ++i) {
            motion[a][i] = values[i]-gravity[i];
            magnitude[i] += values[i]*values[i];
            motion_magnitude[i] += motion[a][i]*motion[a][i];
        }
        custom_spectral_statistics(gravity, output);
        custom_spectral_statistics(motion[a], output);
        means[a] = custom_spectral_mean(motion[a]);
        double variance = 0;
        for (double v : motion[a]) variance += (v-means[a])*(v-means[a]);
        deviations[a] = std::sqrt(variance/samples);
    }
    for (size_t i = 0; i < samples; ++i) { magnitude[i] = std::sqrt(magnitude[i]); motion_magnitude[i] = std::sqrt(motion_magnitude[i]); }
    custom_spectral_statistics(magnitude, output);
    custom_spectral_statistics(motion_magnitude, output);
    custom_spectral_spectrum(motion_magnitude, sampling_freq, spectral_window, output);
    custom_spectral_spectrum(magnitude, sampling_freq, spectral_window, output);
    for (size_t a = 0; a < axes; ++a) for (size_t b = a+1; b < axes; ++b) {
        double corr = 0;
        if (deviations[a] > 1e-12 && deviations[b] > 1e-12) {
            for (size_t i = 0; i < samples; ++i) corr += (motion[a][i]-means[a])*(motion[b][i]-means[b]);
            corr /= samples*deviations[a]*deviations[b];
        }
        output->push_back(static_cast<float>(corr));
    }
}
