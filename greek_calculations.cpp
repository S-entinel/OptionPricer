#include "greek_calculations.h"
#include "math_utils.h"
#include <cmath>

Greeks calculateGreeksBS(const OptionParameters& params) {
    double S = params.S;
    double K = params.K;
    double r = params.r;
    double q = params.q;
    double sigma = params.sigma;
    double T = params.T;

    double sqrtT = std::sqrt(T);
    double sigmaSqrtT = sigma * sqrtT;

    double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / sigmaSqrtT;
    double d2 = d1 - sigmaSqrtT;

    double nd1 = normalCDF(d1);
    double nd2 = normalCDF(d2);
    double pd1 = normalPDF(d1);

    double expQT = std::exp(-q * T);
    double expRT = std::exp(-r * T);

    Greeks greeks;

    if (params.style == OptionStyle::Call) {
        greeks.delta = expQT * nd1;
        greeks.theta = -(S * sigma * expQT * pd1) / (2 * sqrtT)
                       - r * K * expRT * nd2
                       + q * S * expQT * nd1;
        greeks.rho = K * T * expRT * nd2;
    } else { // Put
        greeks.delta = -expQT * (1 - nd1);
        greeks.theta = -(S * sigma * expQT * pd1) / (2 * sqrtT)
                       + r * K * expRT * (1 - nd2)
                       - q * S * expQT * (1 - nd1);
        greeks.rho = -K * T * expRT * (1 - nd2);
    }

    // Common for both Call and Put
    greeks.gamma = (expQT * pd1) / (S * sigmaSqrtT);
    greeks.vega = S * expQT * pd1 * sqrtT;

    return greeks;
}

Greeks calculateGreeksFD(const OptionParameters& params, const std::function<double(const OptionParameters&)>& pricingFunction) {
    const double h = 0.01; // Small change for finite difference
    const double dt = 1.0 / 365.0; // One day

    Greeks greeks;
    OptionParameters updatedParams = params;

    // Delta and Gamma
    updatedParams.S = params.S + h;
    double priceUp = pricingFunction(updatedParams);
    updatedParams.S = params.S - h;
    double priceDown = pricingFunction(updatedParams);
    double priceMiddle = pricingFunction(params);
    
    greeks.delta = (priceUp - priceDown) / (2 * h);
    greeks.gamma = (priceUp - 2 * priceMiddle + priceDown) / (h * h);

    // Theta
    updatedParams = params;
    updatedParams.T -= dt;
    double priceLater = pricingFunction(updatedParams);
    greeks.theta = (priceLater - priceMiddle) / dt;

    // Vega
    updatedParams = params;
    updatedParams.sigma += 0.01;
    double priceVolUp = pricingFunction(updatedParams);
    greeks.vega = (priceVolUp - priceMiddle) / 0.01;

    // Rho
    updatedParams = params;
    updatedParams.r += 0.0001;
    double priceRateUp = pricingFunction(updatedParams);
    greeks.rho = (priceRateUp - priceMiddle) / 0.0001;

    return greeks;
}