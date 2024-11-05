#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

inline double normalPDF(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-0.5 * x * x);
}

// Normal CDF approximation (Abramowitz and Stegun method)
inline double normalCDF(double x) {
    if (x < -10.0) return 0.0;
    if (x > 10.0) return 1.0;
    
    if (x < 0.0) {
        return 1.0 - normalCDF(-x);
    }

    static const double p = 0.2316419;
    static const double b1 = 0.319381530;
    static const double b2 = -0.356563782;
    static const double b3 = 1.781477937;
    static const double b4 = -1.821255978;
    static const double b5 = 1.330274429;

    double t = 1.0 / (1.0 + p * x);
    double poly = t * (b1 + t * (b2 + t * (b3 + t * (b4 + t * b5))));
    
    return 1.0 - normalPDF(x) * poly;
}

#endif // MATH_UTILS_H