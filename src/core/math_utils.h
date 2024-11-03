#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

inline double normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}

inline double normalCDF(double x) {
    return 0.5 * std::erfc(-x * M_SQRT1_2);
}

#endif // MATH_UTILS_H