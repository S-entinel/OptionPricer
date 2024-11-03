#ifndef TYPES_H
#define TYPES_H

enum class OptionType { European, American };
enum class OptionStyle { Call, Put };

struct Greeks {
    double delta;
    double gamma;
    double theta;
    double vega;
    double rho;
};

struct OptionParameters {
    double S;     // Current stock price
    double K;     // Strike price
    double r;     // Risk-free interest rate (annualized)
    double sigma; // Volatility of the underlying stock (annualized)
    double T;     // Time to expiration (in years)
    double q;     // Continuous dividend yield (annualized)
    OptionType type;
    OptionStyle style;
};

struct PricingResult {
    double price;
    Greeks greeks;
};

#endif // TYPES_H