#include <QtWidgets/QApplication>
#include "option_pricing_gui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    OptionPricingGUI window;
    window.show();
    return app.exec();
}