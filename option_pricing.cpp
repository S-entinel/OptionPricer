#include "option_pricing.h"
#include "math_utils.h"
#include "pricing_exceptions.h"
#include "greek_calculations.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <functional>

template<typename T>
void validateOptionParametersT(const OptionParametersT<T>& params) {
    // Lambda for checking positive values
    auto checkPositive = [](T value, const std::string& name) {
        if (value <= T(0)) {
            throw OptionPricingError(name + " must be positive");
        }
    };

    // Lambda for checking non-negative values
    auto checkNonNegative = [](T value, const std::string& name) {
        if (value < T(0)) {
            throw OptionPricingError(name + " cannot be negative");
        }
    };

    // Lambda for checking for invalid numbers
    auto checkValidNumber = [](T value, const std::string& name) {
        if (std::isnan(value) || std::isinf(value)) {
            throw OptionPricingError(name + " contains invalid value");
        }
    };

    // Apply validation checks
    checkPositive(params.S, "Stock price");
    checkPositive(params.K, "Strike price");
    checkNonNegative(params.r, "Risk-free rate");
    checkPositive(params.sigma, "Volatility");
    checkPositive(params.expiry, "Time to expiration");
    checkNonNegative(params.q, "Dividend yield");

    // Check for invalid numbers
    std::array<std::pair<T, std::string>, 6> checks = {{
        {params.S, "Stock price"},
        {params.K, "Strike price"},
        {params.r, "Risk-free rate"},
        {params.sigma, "Volatility"},
        {params.expiry, "Time to expiration"},
        {params.q, "Dividend yield"}
    }};

    for (const auto& [value, name] : checks) {
        checkValidNumber(value, name);
    }
}

template<typename T>
PricingResultT<T> BlackScholesModelT<T>::calculate(const OptionParametersT<T>& params) const {
    validateOptionParametersT(params);
    PricingResultT<T> result;
    
    T S = params.S;
    T K = params.K;
    T r = params.r;
    T q = params.q;
    T sigma = params.sigma;
    T time = params.expiry;

    T sqrtT = std::sqrt(time);
    T sigmaSqrtT = sigma * sqrtT;

    T d1 = (std::log(S / K) + (r - q + T(0.5) * sigma * sigma) * time) / sigmaSqrtT;
    T d2 = d1 - sigmaSqrtT;

    T nd1 = normalCDF(d1);
    T nd2 = normalCDF(d2);

    T expQT = std::exp(-q * time);
    T expRT = std::exp(-r * time);

    if (params.style == OptionStyle::Call) {
        result.price = S * expQT * nd1 - K * expRT * nd2;
    } else {
        result.price = K * expRT * (T(1) - nd2) - S * expQT * (T(1) - nd1);
    }

    // Calculate Greeks
    T pd1 = normalPDF(d1);

    if (params.style == OptionStyle::Call) {
        result.greeks.delta = expQT * nd1;
        result.greeks.theta = -(S * sigma * expQT * pd1) / (T(2) * sqrtT)
                           - r * K * expRT * nd2
                           + q * S * expQT * nd1;
        result.greeks.rho = K * time * expRT * nd2;
    } else {
        result.greeks.delta = -expQT * (T(1) - nd1);
        result.greeks.theta = -(S * sigma * expQT * pd1) / (T(2) * sqrtT)
                           + r * K * expRT * (T(1) - nd2)
                           - q * S * expQT * (T(1) - nd1);
        result.greeks.rho = -K * time * expRT * (T(1) - nd2);
    }

    // Make theta positive
    result.greeks.theta = -result.greeks.theta;

    // Common Greeks for both calls and puts
    result.greeks.gamma = expQT * pd1 / (S * sigmaSqrtT);
    result.greeks.vega = S * expQT * pd1 * sqrtT;

    return result;
}

template<typename T>
T BinomialModelT<T>::calculateBinomialPrice(const OptionParametersT<T>& params) const {
    priceTree.clear();
    priceTree.resize(steps + 1);

    T dt = params.expiry / T(steps);
    T u = std::exp(params.sigma * std::sqrt(dt));
    T d = T(1) / u;
    T p = (std::exp((params.r - params.q) * dt) - d) / (u - d);
    T discount = std::exp(-params.r * dt);

    if (p < T(0) || p > T(1)) {
        throw NumericalError("Invalid probability in binomial model");
    }

    // Initialize terminal nodes
    for (int i = 0; i <= steps; ++i) {
        T St = params.S * std::pow(u, T(steps - i)) * std::pow(d, T(i));
        if (params.style == OptionStyle::Call) {
            priceTree[i] = std::max(T(0), St - params.K);
        } else {
            priceTree[i] = std::max(T(0), params.K - St);
        }
    }

    // Backward induction
    for (int step = steps - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            T St = params.S * std::pow(u, T(step - i)) * std::pow(d, T(i));
            T continuation = discount * (p * priceTree[i] + (T(1) - p) * priceTree[i + 1]);

            if (params.type == OptionType::American) {
                T intrinsic = (params.style == OptionStyle::Call) ? 
                    std::max(T(0), St - params.K) : 
                    std::max(T(0), params.K - St);
                priceTree[i] = std::max(continuation, intrinsic);
            } else {
                priceTree[i] = continuation;
            }
        }
    }

    return priceTree[0];
}

template<typename T>
T BinomialModelT<T>::calculatePrice(const OptionParametersT<T>& params) const {
    validateOptionParametersT(params);

    // For call options with no dividends, American = European
    if (params.style == OptionStyle::Call && params.q == T(0)) {
        OptionParametersT<T> europeanParams = params;
        europeanParams.type = OptionType::European;
        return calculateBinomialPrice(europeanParams);
    }

    return calculateBinomialPrice(params);
}

template<typename T>
PricingResultT<T> BinomialModelT<T>::calculate(const OptionParametersT<T>& params) const {
    try {
        PricingResultT<T> result;
        result.price = calculatePrice(params);

        // Improved step sizes for more accurate Greeks
        T h = params.S * T(0.0001);    // Smaller step for delta/gamma
        T dt = T(1) / T(365);          // One day for theta
        T dvol = T(0.0001);            // Smaller step for vega
        T dr = T(0.0001);              // 1 basis point for rho

        // Delta and Gamma calculations
        OptionParametersT<T> upParams = params;
        OptionParametersT<T> downParams = params;
        upParams.S += h;
        downParams.S -= h;
        
        T priceUp = calculatePrice(upParams);
        T priceDown = calculatePrice(downParams);
        
        result.greeks.delta = (priceUp - priceDown) / (T(2) * h);
        
        T priceMiddle = result.price;
        result.greeks.gamma = (priceUp - T(2) * priceMiddle + priceDown) / (h * h);

        // Theta calculation
        OptionParametersT<T> thetaParams = params;
        thetaParams.expiry -= dt;
        if (thetaParams.expiry > T(0)) {
            result.greeks.theta = -(calculatePrice(thetaParams) - priceMiddle) / dt;
        } else {
            result.greeks.theta = T(0);
        }
        
        // Vega calculation
        OptionParametersT<T> vegaParams = params;
        vegaParams.sigma += dvol;
        result.greeks.vega = (calculatePrice(vegaParams) - priceMiddle) / dvol;

        // Rho calculation
        OptionParametersT<T> rhoParams = params;
        rhoParams.r += dr;
        result.greeks.rho = (calculatePrice(rhoParams) - priceMiddle) / dr;

        return result;
    } catch (const std::exception& e) {
        throw NumericalError("Error in binomial calculation: " + std::string(e.what()));
    }
}

// Explicit instantiations
template class BlackScholesModelT<double>;
template class BinomialModelT<double>;
template void validateOptionParametersT<double>(const OptionParametersT<double>&);
