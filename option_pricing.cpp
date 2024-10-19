#include "option_pricing.h"
#include "math_utils.h"
#include "pricing_exceptions.h"
#include "greek_calculations.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <functional>

#include "option_pricing.h"
#include "math_utils.h"
#include "greek_calculations.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

void validateOptionParameters(const OptionParameters& params) {
    if (params.S <= 0) throw OptionPricingError("Stock price must be positive");
    if (params.K <= 0) throw OptionPricingError("Strike price must be positive");
    if (params.r < 0) throw OptionPricingError("Risk-free rate cannot be negative");
    if (params.sigma <= 0) throw OptionPricingError("Volatility must be positive");
    if (params.T <= 0) throw OptionPricingError("Time to expiration must be positive");
    if (params.q < 0) throw OptionPricingError("Dividend yield cannot be negative");
    if (std::isnan(params.S) || std::isnan(params.K) || std::isnan(params.r) || std::isnan(params.sigma) || std::isnan(params.T) || std::isnan(params.q)) {
        throw OptionPricingError("Input parameters contain NaN values");
    }
    if (std::isinf(params.S) || std::isinf(params.K) || std::isinf(params.r) || std::isinf(params.sigma) || std::isinf(params.T) || std::isinf(params.q)) {
        throw OptionPricingError("Input parameters contain infinite values");
    }
}

PricingResult BlackScholesModel::calculate(const OptionParameters& params) const {
    validateOptionParameters(params);

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

    double expQT = std::exp(-q * T);
    double expRT = std::exp(-r * T);

    PricingResult result;

    if (params.style == OptionStyle::Call) {
        result.price = S * expQT * nd1 - K * expRT * nd2;
    } else {
        result.price = K * expRT * (1 - nd2) - S * expQT * (1 - nd1);
    }

    result.greeks = calculateGreeksBS(params);

    return result;
}

PricingResult BinomialModel::calculate(const OptionParameters& params) const {
    validateOptionParameters(params);

    // Resize the vector if necessary
    if (static_cast<int>(priceTree.size()) != steps + 1) {
        priceTree.resize(steps + 1);
    }

    double dt = params.T / steps;
    double u = std::exp(params.sigma * std::sqrt(dt));
    double d = 1 / u;
    double p = (std::exp((params.r - params.q) * dt) - d) / (u - d);
    double discount = std::exp(-params.r * dt);

    // Initialize the price tree at expiration
    for (int i = 0; i <= steps; ++i) {
        double ST = params.S * std::pow(u, steps - i) * std::pow(d, i);
        priceTree[i] = std::max(0.0, (params.style == OptionStyle::Call) ? (ST - params.K) : (params.K - ST));
    }

    // Work backwards through the tree
    for (int step = steps - 1; step >= 0; --step) {
        for (int i = 0; i <= step; ++i) {
            priceTree[i] = discount * (p * priceTree[i] + (1 - p) * priceTree[i + 1]);
            
            if (params.type == OptionType::American) {
                double St = params.S * std::pow(u, step - i) * std::pow(d, i);
                double intrinsicValue = (params.style == OptionStyle::Call) ? std::max(0.0, St - params.K) : std::max(0.0, params.K - St);
                priceTree[i] = std::max(priceTree[i], intrinsicValue);
            }
        }
    }

    PricingResult result;
    result.price = priceTree[0];
    
    result.greeks = calculateGreeksFD(params, 
        [this](const OptionParameters& p) -> double {
            return this->calculate(p).price;
        }
    );

    return result;
}

std::unique_ptr<PricingModelBase> createPricingModel(OptionType type, int steps) {
    if (type == OptionType::European) {
        return std::make_unique<BlackScholesModel>();
    } else {
        return std::make_unique<BinomialModel>(steps);
    }
}

PricingResult PricingEngine::price(const OptionParameters& params) const {
    return model->calculate(params);
}