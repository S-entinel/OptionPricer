#include "greek_calculations.h"
#include "math_utils.h"
#include <cmath>

Greeks calculateGreeksBS(const OptionParameters& params) {
    long double S = params.S;
    long double K = params.K;
    long double r = params.r;
    long double q = params.q;
    long double sigma = params.sigma;
    long double time = params.expiry;

    long double sqrtT = std::sqrt(time);
    long double sigmaSqrtT = sigma * sqrtT;

    long double d1 = (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * time) / sigmaSqrtT;
    long double d2 = d1 - sigmaSqrtT;

    long double nd1 = normalCDF(d1);
    long double nd2 = normalCDF(d2);
    long double pd1 = normalPDF(d1);

    long double expQT = std::exp(-q * time);
    long double expRT = std::exp(-r * time);

    Greeks greeks;

    // Delta calculation
    if (params.style == OptionStyle::Call) {
        greeks.delta = static_cast<double>(expQT * nd1);
    } else {
        greeks.delta = static_cast<double>(-expQT * (1.0L - nd1));
    }

    // Gamma calculation - common for both calls and puts
    // Improved gamma formula
    greeks.gamma = static_cast<double>(expQT * pd1 / (S * sigmaSqrtT));

    // Theta calculation - corrected formulas
    if (params.style == OptionStyle::Call) {
        greeks.theta = static_cast<double>(
            -(S * sigma * expQT * pd1) / (2.0L * sqrtT)
            - r * K * expRT * nd2
            + q * S * expQT * nd1
        );
    } else {
        greeks.theta = static_cast<double>(
            -(S * sigma * expQT * pd1) / (2.0L * sqrtT)
            + r * K * expRT * (1.0L - nd2)
            - q * S * expQT * (1.0L - nd1)
        );
    }
    // Make theta positive
    greeks.theta = -greeks.theta;

    // Vega calculation - common for both calls and puts
    greeks.vega = static_cast<double>(S * expQT * pd1 * sqrtT);

    // Rho calculation
    if (params.style == OptionStyle::Call) {
        greeks.rho = static_cast<double>(K * time * expRT * nd2);
    } else {
        greeks.rho = static_cast<double>(-K * time * expRT * (1.0L - nd2));
    }

    return greeks;
}

Greeks calculateGreeksFD(const OptionParameters& params, 
                        const std::function<double(const OptionParameters&)>& pricingFunction) {
    Greeks greeks;
    const double h = params.S * 0.0001;    // Smaller step for delta/gamma
    const double dt = 1.0 / 365.0;         // One day for theta
    const double dvol = 0.0001;            // Smaller step for vega
    const double dr = 0.0001;              // 1 basis point for rho

    // Delta and Gamma calculations
    OptionParameters upParams = params;
    OptionParameters downParams = params;
    
    upParams.S += h;
    downParams.S -= h;
    
    double priceUp = pricingFunction(upParams);
    double priceDown = pricingFunction(downParams);
    double priceMiddle = pricingFunction(params);
    
    greeks.delta = (priceUp - priceDown) / (2.0 * h);
    greeks.gamma = (priceUp - 2.0 * priceMiddle + priceDown) / (h * h);

    // Theta calculation
    OptionParameters thetaParams = params;
    thetaParams.expiry -= dt;
    if (thetaParams.expiry > 0.0) {
        greeks.theta = -(pricingFunction(thetaParams) - priceMiddle) / dt;
    } else {
        greeks.theta = 0.0;
    }
    
    // Vega calculation
    OptionParameters vegaParams = params;
    vegaParams.sigma += dvol;
    greeks.vega = (pricingFunction(vegaParams) - priceMiddle) / dvol;

    // Rho calculation
    OptionParameters rhoParams = params;
    rhoParams.r += dr;
    greeks.rho = (pricingFunction(rhoParams) - priceMiddle) / dr;

    return greeks;
}