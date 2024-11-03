#ifndef GREEK_CALCULATIONS_H
#define GREEK_CALCULATIONS_H

#include "types.h"
#include <functional>

Greeks calculateGreeksBS(const OptionParameters& params);

Greeks calculateGreeksFD(const OptionParameters& params, 
                         const std::function<double(const OptionParameters&)>& pricingFunction);

#endif // GREEK_CALCULATIONS_H