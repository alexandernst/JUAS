#include "webwidget.h"

WebWidget::WebWidget() : QMainWindow(){

    trayIcon = 0;
    desktop = QApplication::desktop();

    gv = new WebGraphicsView();
    gv->loadFile(QLatin1String("html/index.html"));

    widget = new QWidget(this);
    layout = new QVBoxLayout(widget);
    layout->addWidget(gv);
    layout->setMargin(0);
    widget->setLayout(layout);
    setCentralWidget(widget);

    // http://qt-project.org/doc/qt-5.0/qtcore/qt.html#WindowType-enum
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    wf = gv->webView()->page()->mainFrame();
    wf->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    wf->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    ws = gv->webView()->page()->settings();
    ws->setAttribute(QWebSettings::LocalStorageEnabled, true);
    ws->setAttribute(QWebSettings::ScrollAnimatorEnabled, true);
    ws->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    ws->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    ws->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    ws->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    ws->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    ws->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    ws->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    ws->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);

#ifndef QT_NO_DEBUG
    webInspector = new QWebInspector();
    webInspector->setPage(gv->webView()->page());
    webInspector->setGeometry(2000, 600, 1300, 400);
#endif

    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));
    QObject::connect(wf, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(createJSBridge()));
}

void WebWidget::createJSBridge(){
    // http://qt-project.org/doc/qt-4.8/qtwebkit-bridge.html
    wf->addToJavaScriptWindowObject("App", this);

#ifndef QT_NO_DEBUG
    webInspector->show();
#endif
}

void WebWidget::setTitle(QString title){
    setWindowTitle(title);
    QCoreApplication::setOrganizationName(title);
    QCoreApplication::setApplicationName(title);
}

void WebWidget::mousePressEvent(int fromBorderX, int fromBorderY){
    fromBorderPosition = QPoint(fromBorderX, fromBorderY);
}

void WebWidget::mouseMoveEvent(int dragX, int dragY){
    //If the window is maximized do nothing
    if(!isMaximized()){
        move(QPoint(dragX, dragY) - fromBorderPosition);
    }
}

void WebWidget::resize(int x, int y, int w, int h){
    setGeometry(x, y, w, h);
}

void WebWidget::minimize(){
#ifdef MINIMIZE_PATCH
    hide();
#else
    this->showMinimized();
#endif
}

void WebWidget::maximize(){
    if(isMaximized()){
        showNormal();
    }else{
        showMaximized();

        if(windowFlags() & Qt::FramelessWindowHint){
            //Don't overlap the taskbar
            QRect ag = desktop->availableGeometry();
            setGeometry(ag.x(), ag.y(), ag.width(), ag.height());
        }
    }
}

QVariantList WebWidget::availableGeometry(){
    QVariantList geometry;
    QRect ag = desktop->availableGeometry();
    geometry << ag.x() << ag.y() << ag.width() << ag.height();
    return geometry;
}

void WebWidget::clearSavedWindowGeometry(QString company, QString app){
    QSettings settings(company, app);
    settings.clear();
}

bool WebWidget::isWindowGeometrySaveAvailable(QString company, QString app){
    QSettings settings(company, app);
    return settings.contains("geometry") && settings.contains("windowState");
}

void WebWidget::saveWindowGeometry(QString company, QString app){
    QSettings settings(company, app);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void WebWidget::restoreWindowGeometry(QString company, QString app){
    QSettings settings(company, app);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void WebWidget::cleanUp(){
    removeTrayIcon();
}

void WebWidget::quit(){
    QApplication::quit();
}
