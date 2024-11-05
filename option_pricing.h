#ifndef OPTION_PRICING_H
#define OPTION_PRICING_H

#include "types.h"
#include "pricing_exceptions.h"
#include <memory>
#include <vector>

// Base model with template parameter
template<typename T = double>
class PricingModelBaseT {
public:
    virtual ~PricingModelBaseT() = default;
    virtual PricingResultT<T> calculate(const OptionParametersT<T>& params) const = 0;
};

// Black-Scholes model with template parameter
template<typename T = double>
class BlackScholesModelT : public PricingModelBaseT<T> {
public:
    PricingResultT<T> calculate(const OptionParametersT<T>& params) const override;
};

// Binomial model with template parameter
template<typename T = double>
class BinomialModelT : public PricingModelBaseT<T> {
    mutable std::vector<T> priceTree;
    int steps;

public:
    explicit BinomialModelT(int steps) : priceTree(steps + 1), steps(steps) {}
    PricingResultT<T> calculate(const OptionParametersT<T>& params) const override;
    
private:
    T calculatePrice(const OptionParametersT<T>& params) const;
    T calculateBinomialPrice(const OptionParametersT<T>& params) const;
};

// Type aliases for backward compatibility
using PricingModelBase = PricingModelBaseT<double>;
using BlackScholesModel = BlackScholesModelT<double>;
using BinomialModel = BinomialModelT<double>;

template<typename T = double>
using PricingModelPtr = std::unique_ptr<PricingModelBaseT<T>>;

// Factory function with template support
template<typename T = double>
PricingModelPtr<T> createPricingModelT(OptionType type, int steps = 500) {
    if (type == OptionType::European) {
        return std::make_unique<BlackScholesModelT<T>>();
    } else {
        return std::make_unique<BinomialModelT<T>>(steps);
    }
}

// Type alias for backward compatibility
inline std::unique_ptr<PricingModelBase> createPricingModel(OptionType type, int steps = 500) {
    return createPricingModelT<double>(type, steps);
}

// Template version of the pricing engine
template<typename T = double>
class PricingEngineT {
    std::shared_ptr<PricingModelBaseT<T>> model;

public:
    explicit PricingEngineT(std::shared_ptr<PricingModelBaseT<T>> model) 
        : model(std::move(model)) {}
    
    PricingResultT<T> price(const OptionParametersT<T>& params) const {
        if (!model) {
            throw OptionPricingError("Pricing model not initialized");
        }
        return model->calculate(params);
    }
};

// Type alias for backward compatibility
using PricingEngine = PricingEngineT<double>;

template<typename T>
void validateOptionParametersT(const OptionParametersT<T>& params);

// Type alias for backward compatibility
inline void validateOptionParameters(const OptionParameters& params) {
    validateOptionParametersT(params);
}

#endif // OPTION_PRICING_H