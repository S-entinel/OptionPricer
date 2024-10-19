#include "option_pricing.h"
#include "greek_calculations.h"
#include "user_interface.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <vector>

void saveResults(const std::vector<PricingResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    for (const auto& result : results) {
        file << "Price: " << result.price << ", Delta: " << result.greeks.delta << "\n";
    }
}

int main() {
    while (true) {
        clearScreen();
        displayHeader();

        try {
            OptionParameters params = getUserInput();
            PricingResult result;

            std::unique_ptr<PricingModelBase> model;
            if (params.type == OptionType::European) {
                model = std::make_unique<BlackScholesModel>();
            } else {
                model = std::make_unique<BinomialModel>(1000);
            }

            PricingEngine engine(std::move(model));
            result = engine.price(params);

            displayResults(params, result.price, result.greeks);

            // Save results to file
            std::vector<PricingResult> results = {result};
            saveResults(results, "option_prices.txt");

            std::cout << "Results saved to file: option_prices.txt\n";
        }
        catch (const InputValidationError& e) {
            std::cerr << "Input Error: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        }

        if (!continueCalculations()) {
            break;
        }
    }

    std::cout << "Thank you for using the Option Pricing Tool!\n";
    return 0;
}