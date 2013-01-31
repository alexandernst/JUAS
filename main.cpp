#include <QtWidgets/QApplication>
#include "webwidget.h"

int main(int argc, char *argv[])
{
    //TODO: make singleton
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);
    new WebWidget();

    return app.exec();
}
