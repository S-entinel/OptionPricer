#ifndef OPTIONS_GUI_H
#define OPTIONS_GUI_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QGridLayout>
#include "option_pricing.h"
#include "types.h"

class OptionsPricerGUI : public QMainWindow {
    Q_OBJECT

private:
    // Input fields
    QLineEdit *spotPriceInput;
    QLineEdit *strikePriceInput;
    QLineEdit *rateInput;
    QLineEdit *volatilityInput;
    QLineEdit *timeInput;
    QLineEdit *dividendInput;
    QComboBox *optionTypeCombo;
    QComboBox *optionStyleCombo;
    QComboBox *modelTypeCombo;
    QTableWidget *resultsTable;

    // Methods
    void setupUI();
    void createInputSection();
    void createResultsSection();
    void connectSignals();
    OptionParameters getParameters() const;
    void displayResults(const PricingResult& result);
    void clearResults();
    void handleError(const std::string& message);

private slots:
    void calculateOption();
    void modelTypeChanged(int index);

public:
    explicit OptionsPricerGUI(QWidget *parent = nullptr);
};

#endif // OPTIONS_GUI_H