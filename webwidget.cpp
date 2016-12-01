#include "webwidget.h"

WebWidget::WebWidget() : QMainWindow(){

    trayIcon = 0;
    desktop = QApplication::desktop();

    gv = new WebView();

    widget = new QWidget(this);
    layout = new QVBoxLayout(widget);
    layout->addWidget(gv);
    layout->setMargin(0);
    widget->setLayout(layout);
    setCentralWidget(widget);

    // http://qt-project.org/doc/qt-5.0/qtcore/qt.html#WindowType-enum
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    wp = gv->webView()->page();
    //wp->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    //wp->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    ws = gv->webView()->page()->settings();
    //ws->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    ws->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    ws->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    ws->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    ws->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ws->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    ws->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

    //Properties
    connect(this, SIGNAL(xChanged(int)), this, SLOT(xHasChanged(int)));
    connect(this, SIGNAL(yChanged(int)), this, SLOT(yHasChanged(int)));
    connect(this, SIGNAL(widthChanged(int)), this, SLOT(widthHasChanged(int)));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(heightHasChanged(int)));

    createJSBridge();
    gv->loadFile("html/index.html");
    wp->runJavaScript("document.documentElement.contentEditable = true");
}

void WebWidget::createJSBridge(){
    channel = new QWebChannel(this);
    wp->setWebChannel(channel);
    channel->registerObject(QStringLiteral("App"), this);
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
        QPoint p = QPoint(dragX, dragY) - fromBorderPosition;
        move(p);
        emit xChanged(p.x());
        emit yChanged(p.y());
    }
}

void WebWidget::resize(int x, int y, int w, int h){
    setGeometry(x, y, w, h);
    emit xChanged(x);
    emit yChanged(y);
    emit widthChanged(w);
    emit heightChanged(h);
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
            emit xChanged(ag.x());
            emit yChanged(ag.y());
            emit widthChanged(ag.width());
            emit heightChanged(ag.height());
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


////////////////////////////////

int WebWidget::getX(){
    return this->x();
}

void WebWidget::setX(int x){
    this->pos().setX(x);
    emit xChanged(x);
}

void WebWidget::xHasChanged(int x){
    //
}

int WebWidget::getY(){
    return this->y();
}

void WebWidget::setY(int y){
    this->pos().setY(y);
    emit yChanged(y);
}

void WebWidget::yHasChanged(int y){
    //
}

int WebWidget::getWidth(){
    return this->frameGeometry().width();
}

void WebWidget::setWidth(int width){
    QRect r = this->frameGeometry();
    r.setWidth(width);
    emit widthChanged(width);
}

void WebWidget::widthHasChanged(int width){
    //
}

int WebWidget::getHeight(){
    return this->frameGeometry().height();
}

void WebWidget::setHeight(int height){
    QRect r = this->frameGeometry();
    r.setHeight(height);
    emit heightChanged(height);
}

void WebWidget::heightHasChanged(int height){
    //
}
