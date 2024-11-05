#ifndef PRICING_EXCEPTIONS_H
#define PRICING_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class OptionPricingError : public std::runtime_error {
public:
    OptionPricingError(const std::string& message) : std::runtime_error(message) {}
};

class NumericalError : public OptionPricingError {
public:
    NumericalError(const std::string& message) : OptionPricingError("Numerical Error: " + message) {}
};

class InputValidationError : public std::runtime_error {
public:
    InputValidationError(const std::string& message) : std::runtime_error(message) {}
};

#endif // PRICING_EXCEPTIONS_H