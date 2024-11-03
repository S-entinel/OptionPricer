# Options Pricing Calculator

A C++ application with Qt GUI for pricing financial options using Black-Scholes and Binomial models.

## Features

- Support for both European and American options
- Implementation of Black-Scholes and Binomial pricing models
- Calculation of option Greeks (Delta, Gamma, Theta, Vega, Rho)
- User-friendly Qt-based graphical interface
- Real-time calculation updates

## Requirements

- C++17 compatible compiler
- CMake 3.1 or higher
- Qt 5.x

### For Mac (using Homebrew)
```bash
brew install qt@5
```

## Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make
```

## Usage

Run the executable from the build directory:
```bash
./option_pricing
```

### Sample Input Parameters

European Call Option (Black-Scholes):
- Spot Price: 100
- Strike Price: 100
- Risk-free Rate: 5%
- Volatility: 20%
- Time to Expiry: 1 year
- Dividend Yield: 0%
- Option Type: European
- Option Style: Call
- Model: Black-Scholes

Expected outputs will include option price and Greeks (Delta, Gamma, Theta, Vega, Rho).

## Project Structure

```
option_pricing/
├── src/
│   ├── core/           # Core pricing engine
│   │   ├── greek_calculations.cpp/h
│   │   ├── option_pricing.cpp/h
│   │   ├── math_utils.h
│   │   └── types.h
│   ├── gui/            # Qt GUI implementation
│   │   ├── options_gui.cpp
│   │   └── options_gui.h
│   └── main.cpp
├── CMakeLists.txt
└── README.md
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.


## Contributing

Feel free to submit issues and enhancement requests!

## Author

S-entinel
