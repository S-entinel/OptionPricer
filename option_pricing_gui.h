#ifndef OPTION_PRICING_GUI_H
#define OPTION_PRICING_GUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include "option_pricing.h"

class OptionPricingGUI : public QMainWindow {
    Q_OBJECT

public:
    OptionPricingGUI(QWidget *parent = nullptr);

private slots:
    void calculateOption();
    void updateOptionType(int index);

private:
    // Input widgets
    QDoubleSpinBox *stockPriceInput;
    QDoubleSpinBox *strikePriceInput;
    QDoubleSpinBox *riskFreeRateInput;
    QDoubleSpinBox *volatilityInput;
    QDoubleSpinBox *timeToExpiryInput;
    QDoubleSpinBox *dividendYieldInput;
    QComboBox *optionTypeCombo;
    QComboBox *optionStyleCombo;

    // Output labels
    QLabel *priceLabel;
    QLabel *deltaLabel;
    QLabel *gammaLabel;
    QLabel *thetaLabel;
    QLabel *vegaLabel;
    QLabel *rhoLabel;
    QLabel *modelLabel;

    // Setup methods
    void setupUI();
    QDoubleSpinBox* createDoubleSpinBox(double min, double max, double step, int decimals);
    QGroupBox* createInputGroup();
    QGroupBox* createOutputGroup();
    void displayResults(const OptionParameters& params, double price, const Greeks& greeks);
};

#endif // OPTION_PRICING_GUI_H