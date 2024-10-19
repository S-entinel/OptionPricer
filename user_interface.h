#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "option_pricing.h"
#include "greek_calculations.h"
#include <stdexcept>
#include <string>

class InputValidationError : public std::runtime_error {
public:
    InputValidationError(const std::string& message) : std::runtime_error(message) {}
};

void clearScreen();
void displayHeader();
OptionParameters getUserInput();
void displayResults(const OptionParameters& params, double price, const Greeks& greeks);
bool continueCalculations();

#endif // USER_INTERFACE_H