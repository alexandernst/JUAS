#include <QtWidgets/QApplication>
#include "webwidget.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);
    WebWidget ww;
    ww.show();

    return app.exec();
}
