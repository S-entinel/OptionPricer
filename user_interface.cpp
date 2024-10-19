#include "user_interface.h"
#include <iostream>
#include <limits>
#include <iomanip>
#include "pricing_exceptions.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void displayHeader() {
    std::cout << "=== Option Pricing and Greek Calculator ===\n\n";
}

template<typename T>
T getInput(const std::string& prompt, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value < min || value > max) {
                throw InputValidationError("Input out of valid range: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
            }
            return value;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw InputValidationError("Invalid input. Please enter a number.");
        }
    }
}

OptionParameters getUserInput() {
    OptionParameters params;
    
    try {
        params.S = getInput<double>("Enter the current stock price: ", 0.0);
        params.K = getInput<double>("Enter the strike price: ", 0.0);
        params.r = getInput<double>("Enter the risk-free rate (as a decimal): ", 0.0, 1.0);
        params.sigma = getInput<double>("Enter the volatility (as a decimal): ", 0.0, 1.0);
        params.T = getInput<double>("Enter the time to expiration (in years): ", 0.0);
        params.q = getInput<double>("Enter the dividend yield (as a decimal): ", 0.0, 1.0);

        int optionTypeChoice = getInput<int>("Select option type (1: European, 2: American): ", 1, 2);
        params.type = (optionTypeChoice == 1) ? OptionType::European : OptionType::American;

        int optionStyleChoice = getInput<int>("Select option style (1: Call, 2: Put): ", 1, 2);
        params.style = (optionStyleChoice == 1) ? OptionStyle::Call : OptionStyle::Put;

        validateOptionParameters(params);
    } catch (const OptionPricingError& e) {
        throw InputValidationError(e.what());
    }

    return params;
}

void displayResults(const OptionParameters& params, double price, const Greeks& greeks) {
    std::cout << "\nResults:\n";
    std::cout << "Option Type: " << (params.type == OptionType::European ? "European" : "American") << " ";
    std::cout << (params.style == OptionStyle::Call ? "Call" : "Put") << "\n";
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Price: " << price << "\n";
    std::cout << "Delta: " << greeks.delta << "\n";
    std::cout << "Gamma: " << greeks.gamma << "\n";
    std::cout << "Theta: " << greeks.theta << "\n";
    std::cout << "Vega:  " << greeks.vega << "\n";
    std::cout << "Rho:   " << greeks.rho << "\n";
}

bool continueCalculations() {
    char choice;
    std::cout << "\nDo you want to calculate another option? (y/n): ";
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (choice == 'y' || choice == 'Y');
}