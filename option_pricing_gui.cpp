#include "option_pricing_gui.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>

OptionPricingGUI::OptionPricingGUI(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Option Pricing Calculator");
    setupUI();
}

void OptionPricingGUI::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QGroupBox* inputGroup = createInputGroup();
    QGroupBox* outputGroup = createOutputGroup();
    
    // Create calculate button
    QPushButton *calculateButton = new QPushButton("Calculate", this);
    connect(calculateButton, &QPushButton::clicked, this, &OptionPricingGUI::calculateOption);
    
    // Add widgets to main layout
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(outputGroup);
    mainLayout->addWidget(calculateButton);
    
    setCentralWidget(centralWidget);
    setFixedSize(400, 600);
}

QDoubleSpinBox* OptionPricingGUI::createDoubleSpinBox(double min, double max, double step, int decimals) {
    QDoubleSpinBox *spinBox = new QDoubleSpinBox(this);
    spinBox->setRange(min, max);
    spinBox->setSingleStep(step);
    spinBox->setDecimals(decimals);
    return spinBox;
}

QGroupBox* OptionPricingGUI::createInputGroup() {
    QGroupBox *groupBox = new QGroupBox("Input Parameters");
    QGridLayout *layout = new QGridLayout;

    // Create spin boxes with appropriate ranges
    stockPriceInput = createDoubleSpinBox(0.01, 10000.0, 1.0, 2);
    strikePriceInput = createDoubleSpinBox(0.01, 10000.0, 1.0, 2);
    riskFreeRateInput = createDoubleSpinBox(0.0, 1.0, 0.001, 4);
    volatilityInput = createDoubleSpinBox(0.0, 1.0, 0.01, 4);
    timeToExpiryInput = createDoubleSpinBox(0.01, 30.0, 0.1, 2);
    dividendYieldInput = createDoubleSpinBox(0.0, 1.0, 0.001, 4);

    // Option type combo box
    optionTypeCombo = new QComboBox;
    optionTypeCombo->addItem("European");
    optionTypeCombo->addItem("American");
    connect(optionTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OptionPricingGUI::updateOptionType);

    // Option style combo box
    optionStyleCombo = new QComboBox;
    optionStyleCombo->addItem("Call");
    optionStyleCombo->addItem("Put");

    // Add widgets to layout
    int row = 0;
    layout->addWidget(new QLabel("Stock Price:"), row, 0);
    layout->addWidget(stockPriceInput, row++, 1);
    layout->addWidget(new QLabel("Strike Price:"), row, 0);
    layout->addWidget(strikePriceInput, row++, 1);
    layout->addWidget(new QLabel("Risk-free Rate:"), row, 0);
    layout->addWidget(riskFreeRateInput, row++, 1);
    layout->addWidget(new QLabel("Volatility:"), row, 0);
    layout->addWidget(volatilityInput, row++, 1);
    layout->addWidget(new QLabel("Time to Expiry (years):"), row, 0);
    layout->addWidget(timeToExpiryInput, row++, 1);
    layout->addWidget(new QLabel("Dividend Yield:"), row, 0);
    layout->addWidget(dividendYieldInput, row++, 1);
    layout->addWidget(new QLabel("Option Type:"), row, 0);
    layout->addWidget(optionTypeCombo, row++, 1);
    layout->addWidget(new QLabel("Option Style:"), row, 0);
    layout->addWidget(optionStyleCombo, row++, 1);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox* OptionPricingGUI::createOutputGroup() {
    QGroupBox *groupBox = new QGroupBox("Results");
    QGridLayout *layout = new QGridLayout;

    // Initialize output labels with a fixed width font for better alignment
    QFont fixedFont("Monospace");
    fixedFont.setStyleHint(QFont::Monospace);

    priceLabel = new QLabel("-");
    deltaLabel = new QLabel("-");
    gammaLabel = new QLabel("-");
    thetaLabel = new QLabel("-");
    vegaLabel = new QLabel("-");
    rhoLabel = new QLabel("-");
    modelLabel = new QLabel("-");

    // Set the fixed-width font for all labels
    for (QLabel* label : {priceLabel, deltaLabel, gammaLabel, thetaLabel, vegaLabel, rhoLabel, modelLabel}) {
        label->setFont(fixedFont);
        label->setMinimumWidth(100);
        label->setAlignment(Qt::AlignRight);
    }

    // Add labels to layout
    int row = 0;
    layout->addWidget(new QLabel("Model Used:"), row, 0);
    layout->addWidget(modelLabel, row++, 1);
    layout->addWidget(new QLabel("Price:"), row, 0);
    layout->addWidget(priceLabel, row++, 1);
    layout->addWidget(new QLabel("Delta:"), row, 0);
    layout->addWidget(deltaLabel, row++, 1);
    layout->addWidget(new QLabel("Gamma:"), row, 0);
    layout->addWidget(gammaLabel, row++, 1);
    layout->addWidget(new QLabel("Theta:"), row, 0);
    layout->addWidget(thetaLabel, row++, 1);
    layout->addWidget(new QLabel("Vega:"), row, 0);
    layout->addWidget(vegaLabel, row++, 1);
    layout->addWidget(new QLabel("Rho:"), row, 0);
    layout->addWidget(rhoLabel, row++, 1);

    groupBox->setLayout(layout);
    return groupBox;
}

void OptionPricingGUI::updateOptionType(int index) {
    modelLabel->setText(index == 0 ? "Black-Scholes" : "Binomial");
}

void OptionPricingGUI::calculateOption() {
    try {
        OptionParameters params;
        params.S = stockPriceInput->value();
        params.K = strikePriceInput->value();
        params.r = riskFreeRateInput->value();
        params.sigma = volatilityInput->value();
        params.expiry = timeToExpiryInput->value();
        params.q = dividendYieldInput->value();
        params.type = optionTypeCombo->currentIndex() == 0 ? OptionType::European : OptionType::American;
        params.style = optionStyleCombo->currentIndex() == 0 ? OptionStyle::Call : OptionStyle::Put;

        std::unique_ptr<PricingModelBase> model;
        if (params.type == OptionType::European) {
            model = std::make_unique<BlackScholesModel>();
        } else {
            model = std::make_unique<BinomialModel>(500);
        }

        PricingEngine engine(std::move(model));
        PricingResult result = engine.price(params);

        displayResults(params, result.price, result.greeks);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Calculation error: %1").arg(e.what()));
    }
}

void OptionPricingGUI::displayResults(const OptionParameters& params, double price, const Greeks& greeks) {
    priceLabel->setText(QString::number(price, 'f', 4));
    deltaLabel->setText(QString::number(greeks.delta, 'f', 4));
    gammaLabel->setText(QString::number(greeks.gamma, 'f', 4));
    thetaLabel->setText(QString::number(greeks.theta, 'f', 4));
    vegaLabel->setText(QString::number(greeks.vega, 'f', 4));
    rhoLabel->setText(QString::number(greeks.rho, 'f', 4));
    modelLabel->setText(params.type == OptionType::European ? "Black-Scholes" : "Binomial");
}