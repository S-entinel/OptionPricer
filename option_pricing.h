#ifndef OPTION_PRICING_H
#define OPTION_PRICING_H

#include "types.h"
#include "pricing_exceptions.h"
#include <memory>
#include <vector>

class PricingModelBase {
public:
    virtual ~PricingModelBase() = default;
    virtual PricingResult calculate(const OptionParameters& params) const = 0;
};

class BlackScholesModel : public PricingModelBase {
public:
    PricingResult calculate(const OptionParameters& params) const override;
};

class BinomialModel : public PricingModelBase {
    mutable std::vector<double> priceTree;
    int steps;

public:
    explicit BinomialModel(int steps) : priceTree(steps + 1), steps(steps) {}
    PricingResult calculate(const OptionParameters& params) const override;
};

std::unique_ptr<PricingModelBase> createPricingModel(OptionType type, int steps = 100);

class PricingEngine {
    std::shared_ptr<PricingModelBase> model;

public:
    explicit PricingEngine(std::shared_ptr<PricingModelBase> model) : model(std::move(model)) {}
    PricingResult price(const OptionParameters& params) const;
};

void validateOptionParameters(const OptionParameters& params);

#endif // OPTION_PRICING_H