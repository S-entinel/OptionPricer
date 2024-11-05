#ifndef TYPES_H
#define TYPES_H

enum class OptionType { European, American };
enum class OptionStyle { Call, Put };

// Template for Greeks to allow different numeric types
template<typename T = double>
struct GreeksT {
    T delta{};
    T gamma{};
    T theta{};
    T vega{};
    T rho{};
};

// Template for option parameters
template<typename T = double>
struct OptionParametersT {
    T S{};        // Current stock price
    T K{};        // Strike price
    T r{};        // Risk-free interest rate
    T sigma{};    // Volatility
    T expiry{};   // Time to expiration (renamed from T)
    T q{};        // Dividend yield
    OptionType type{OptionType::European};
    OptionStyle style{OptionStyle::Call};
};

template<typename T = double>
struct PricingResultT {
    T price{};
    GreeksT<T> greeks{};
};

// Type aliases for backward compatibility
using Greeks = GreeksT<double>;
using OptionParameters = OptionParametersT<double>;
using PricingResult = PricingResultT<double>;

#endif // TYPES_H