#include "options_gui.h"

// Add Qt includes
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QGridLayout>
#include <QApplication>

OptionsPricerGUI::OptionsPricerGUI(QWidget *parent) 
    : QMainWindow(parent)
{
    setWindowTitle("Options Pricing Calculator");
    setupUI();
    connectSignals();
}

void OptionsPricerGUI::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create input and results sections
    createInputSection();
    createResultsSection();
    
    // Set window size
    resize(800, 600);
}

void OptionsPricerGUI::createInputSection() {
    QGroupBox *inputGroup = new QGroupBox("Input Parameters");
    inputGroup->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid gray;
            border-radius: 8px;
            margin-top: 1em;
            padding-top: 0.5em;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px;
            background-color: palette(window);
        }
    )");
    QGridLayout *inputLayout = new QGridLayout;

    // Create input fields with labels
    auto createLabeledInput = [&](const QString &label, int row) -> QLineEdit* {
        inputLayout->addWidget(new QLabel(label), row, 0);
        QLineEdit *input = new QLineEdit;
        inputLayout->addWidget(input, row, 1);
        return input;
    };

    spotPriceInput = createLabeledInput("Spot Price:", 0);
    strikePriceInput = createLabeledInput("Strike Price:", 1);
    rateInput = createLabeledInput("Risk-free Rate (%):", 2);
    volatilityInput = createLabeledInput("Volatility (%):", 3);
    timeInput = createLabeledInput("Time to Expiry (years):", 4);
    dividendInput = createLabeledInput("Dividend Yield (%):", 5);

    // Create combo boxes
    inputLayout->addWidget(new QLabel("Option Type:"), 6, 0);
    optionTypeCombo = new QComboBox;
    optionTypeCombo->addItems({"European", "American"});
    inputLayout->addWidget(optionTypeCombo, 6, 1);

    inputLayout->addWidget(new QLabel("Option Style:"), 7, 0);
    optionStyleCombo = new QComboBox;
    optionStyleCombo->addItems({"Call", "Put"});
    inputLayout->addWidget(optionStyleCombo, 7, 1);

    inputLayout->addWidget(new QLabel("Pricing Model:"), 8, 0);
    modelTypeCombo = new QComboBox;
    modelTypeCombo->addItems({"Black-Scholes", "Binomial"});
    inputLayout->addWidget(modelTypeCombo, 8, 1);

    // Calculate button
    QPushButton *calculateButton = new QPushButton("Calculate");
    inputLayout->addWidget(calculateButton, 9, 0, 1, 2);

    inputGroup->setLayout(inputLayout);
    centralWidget()->layout()->addWidget(inputGroup);

    connect(calculateButton, &QPushButton::clicked, this, &OptionsPricerGUI::calculateOption);
}

void OptionsPricerGUI::createResultsSection() {
    QGroupBox *resultsGroup = new QGroupBox("Results");
    resultsGroup->setStyleSheet(R"(
        QGroupBox {
            border: 1px solid gray;
            border-radius: 8px;
            margin-top: 1em;
            padding-top: 0.5em;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px;
            background-color: palette(window);
        }
    )");
    QVBoxLayout *resultsLayout = new QVBoxLayout;

    resultsTable = new QTableWidget(6, 2);
    resultsTable->setHorizontalHeaderLabels({"Metric", "Value"});
    resultsTable->verticalHeader()->setVisible(false);
    resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Set row labels
    QTableWidgetItem *priceLabel = new QTableWidgetItem("Option Price");
    QTableWidgetItem *deltaLabel = new QTableWidgetItem("Delta");
    QTableWidgetItem *gammaLabel = new QTableWidgetItem("Gamma");
    QTableWidgetItem *thetaLabel = new QTableWidgetItem("Theta");
    QTableWidgetItem *vegaLabel = new QTableWidgetItem("Vega");
    QTableWidgetItem *rhoLabel = new QTableWidgetItem("Rho");

    resultsTable->setItem(0, 0, priceLabel);
    resultsTable->setItem(1, 0, deltaLabel);
    resultsTable->setItem(2, 0, gammaLabel);
    resultsTable->setItem(3, 0, thetaLabel);
    resultsTable->setItem(4, 0, vegaLabel);
    resultsTable->setItem(5, 0, rhoLabel);

    resultsLayout->addWidget(resultsTable);
    resultsGroup->setLayout(resultsLayout);
    centralWidget()->layout()->addWidget(resultsGroup);
}

void OptionsPricerGUI::connectSignals() {
    connect(modelTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OptionsPricerGUI::modelTypeChanged);
}

OptionParameters OptionsPricerGUI::getParameters() const {
    OptionParameters params;
    
    params.S = spotPriceInput->text().toDouble();
    params.K = strikePriceInput->text().toDouble();
    params.r = rateInput->text().toDouble() / 100.0;  // Convert from percentage
    params.sigma = volatilityInput->text().toDouble() / 100.0;  // Convert from percentage
    params.T = timeInput->text().toDouble();
    params.q = dividendInput->text().toDouble() / 100.0;  // Convert from percentage
    
    params.type = (optionTypeCombo->currentText() == "European") ? 
                   OptionType::European : OptionType::American;
    params.style = (optionStyleCombo->currentText() == "Call") ? 
                   OptionStyle::Call : OptionStyle::Put;
    
    return params;
}

void OptionsPricerGUI::displayResults(const PricingResult& result) {
    auto setTableValue = [this](int row, double value) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(value, 'f', 6));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        resultsTable->setItem(row, 1, item);
    };

    setTableValue(0, result.price);
    setTableValue(1, result.greeks.delta);
    setTableValue(2, result.greeks.gamma);
    setTableValue(3, result.greeks.theta);
    setTableValue(4, result.greeks.vega);
    setTableValue(5, result.greeks.rho);
}

void OptionsPricerGUI::clearResults() {
    for (int i = 0; i < resultsTable->rowCount(); ++i) {
        resultsTable->setItem(i, 1, new QTableWidgetItem(""));
    }
}

void OptionsPricerGUI::handleError(const std::string& message) {
    QMessageBox::warning(this, "Error", QString::fromStdString(message));
    clearResults();
}

void OptionsPricerGUI::calculateOption() {
    try {
        OptionParameters params = getParameters();
        validateOptionParameters(params);

        std::unique_ptr<PricingModelBase> model;
        if (modelTypeCombo->currentText() == "Black-Scholes") {
            model = std::make_unique<BlackScholesModel>();
        } else {
            model = std::make_unique<BinomialModel>(1000);
        }

        PricingEngine engine(std::move(model));
        PricingResult result = engine.price(params);
        
        displayResults(result);
    }
    catch (const OptionPricingError& e) {
        handleError(e.what());
    }
    catch (const std::exception& e) {
        handleError("An unexpected error occurred: " + std::string(e.what()));
    }
}

void OptionsPricerGUI::modelTypeChanged(int index) {
    optionTypeCombo->setEnabled(index == 1);  // Enable American options only for Binomial model
    if (index == 0) {  // Black-Scholes
        optionTypeCombo->setCurrentText("European");
    }
}