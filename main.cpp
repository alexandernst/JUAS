#include <QtWidgets/QApplication>
#include "webwidget.h"

int main(int argc, char *argv[])
{
    //TODO: make singleton
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("JS Appchitect");
    QCoreApplication::setApplicationName("JS Appchitect");

    app.setQuitOnLastWindowClosed(false);
    new WebWidget(&app);

    return app.exec();
}
