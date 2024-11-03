#include "options_gui.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Options Pricing");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("OptionsTools");
    
    // Create and show the main window
    OptionsPricerGUI mainWindow;
    mainWindow.show();
    
    return app.exec();
}