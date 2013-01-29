#include "webwidget.h"
#include <QtWidgets/QAction>
#include <QAction>

WebWidget::WebWidget(QApplication *app){

    this->app = app;

    desktop = QApplication::desktop();

    gv = new WebGraphicsView();
    gv->loadFile(QLatin1String("html/index.html"));

    layout = new QVBoxLayout;
    layout->addWidget(gv);
    layout->setMargin(0);
    widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    // http://qt-project.org/doc/qt-5.0/qtcore/qt.html#WindowType-enum
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setGeometry(getCustomGeometry(WebWidget::MainWindow));
    setWindowTitle("JUAS");

    QWebFrame *wf = gv->webView()->page()->mainFrame();
    wf->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    wf->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    QWebSettings *ws = gv->webView()->page()->settings();
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

#ifdef QT_NO_DEBUG
    setStyleSheet("QWidget { background: transparent; }");
#else
    webInspector = new QWebInspector();
    webInspector->setPage(gv->webView()->page());
    webInspector->setGeometry(getCustomGeometry(WebWidget::DebugWindow));

    setStyleSheet("QWidget { background: green; }");
#endif

    QObject::connect(app, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));
    QObject::connect(wf, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(createJSBridge()));
}

void WebWidget::createJSBridge(){
    // http://qt-project.org/doc/qt-4.8/qtwebkit-bridge.html
    gv->webView()->page()->mainFrame()->addToJavaScriptWindowObject("App", this);

    show();
#ifndef QT_NO_DEBUG
    webInspector->show();
#endif
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

void WebWidget::cleanUp(){
    removeTrayIcon();
}

void WebWidget::quit(){
    QApplication::quit();
}









/********************\
|
| GEOMETRY
|
\********************/

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

QRect WebWidget::getCustomGeometry(WindowType wt){
    //TODO: save/restore geometry. As JS methods?
    /*

     //save
     QSettings settings("JUAS_COMPANY", "JUAS_APP");
     settings.setValue("geometry", saveGeometry());
     settings.setValue("windowState", saveState());

     //restore
     QSettings settings("JUAS_COMPANY", "JUAS_APP");
     restoreGeometry(settings.value("geometry").toByteArray());
     restoreState(settings.value("windowState").toByteArray());

    */

    QRect *r;
    float total_width = desktop->availableGeometry().width();
    float total_height = desktop->availableGeometry().height();
    float width, height;

    switch(wt){
        case WebWidget::MainWindow:
#ifndef QT_NO_DEBUG
            width = total_width * 0.7; //70% width
            height = total_height * 0.6; //60% height
            r = new QRect((total_width - width) / 2, total_height / 2 - height * 0.75, width, height);
#else
            width = total_width * 0.7; //70% width
            height = total_height * 0.7; //70% height
            r = new QRect((total_width - width) / 2, (total_height - height) / 2, width, height);
#endif
            break;

        case WebWidget::DebugWindow:
            width = total_width * 0.7; //70% width
            height = total_height * 0.3; //30% height
            r = new QRect((total_width - width) / 2, total_height - height * 1.05, width, height);
            break;

        default:
            r = new QRect(100, 100, 100, 100);
            break;
    }

    return *r;
}









/********************\
|
| TRAYICON
|
\********************/

//TODO: Support adding menu items before/after specific menu item
//TODO: Support submenus

void WebWidget::createTrayIcon(QString icon){
    if(trayIcon == 0){
        trayIcon = new QSystemTrayIcon(this);
        trayIconMenu = new QMenu(this);
        trayIcon->setIcon(QIcon(icon));
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->show();

        QObject::connect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(trayIconMenuClicked(QAction*)));
        QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));
    }
}

void WebWidget::showTrayIcon(){
    if(trayIcon != 0){
        trayIcon->show();
    }
}

void WebWidget::hideTrayIcon(){
    if(trayIcon != 0){
        trayIcon->hide();
    }
}

void WebWidget::setTrayIconTitle(QString title){
    trayIcon->setToolTip(title);
}

void WebWidget::showTrayIconMessage(QString title, QString msg, QString icon, int msecs){
    if(trayIcon != 0 && trayIcon->supportsMessages()){
        QSystemTrayIcon::MessageIcon mi;
        if(icon == "NoIcon"){
            mi = QSystemTrayIcon::NoIcon;
        }else if(icon == "Information"){
            mi = QSystemTrayIcon::Information;
        }else if(icon == "Warning"){
            mi = QSystemTrayIcon::Warning;
        }else if(icon == "Critical"){
            mi = QSystemTrayIcon::Critical;
        }else{
            mi = QSystemTrayIcon::NoIcon;
        }
        trayIcon->showMessage(title, msg, mi, msecs);
    }
}

void WebWidget::addTrayIconMenuItem(QString id, QString text, QString event, QString icon = ""){
    if(trayIcon != 0){
        QAction *action;

        if(icon == ""){
            action = new QAction(text, trayIconMenu);
        }else{
            action = new QAction(QIcon(icon), text, trayIconMenu);
        }

        action->setProperty("ID", id);
        action->setProperty("EventBus", event);
        trayIconMenu->addAction(action);
    }
}

void WebWidget::removeTrayIconMenuItem(QString id){
    if(trayIcon != 0 && !trayIconMenu->isEmpty()){
        QList<QAction*> actions = trayIconMenu->actions();
        for(QList<QAction*>::iterator it = actions.begin(); it != actions.end(); ++it){
            QAction *action = qobject_cast<QAction *>(*it);
            if(id == action->property("ID").toString()){
                trayIconMenu->removeAction(action);
            }
        }
    }
}

void WebWidget::removeTrayIconMenuItems(){
    if(trayIcon != 0 && !trayIconMenu->isEmpty()){
        trayIconMenu->clear();
    }
}

void WebWidget::removeTrayIcon(){
    if(trayIcon != 0){
        QObject::disconnect(trayIconMenu, SIGNAL(triggered(QAction*)), this, SLOT(trayIconMenuClicked(QAction*)));
        QObject::disconnect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));
        trayIconMenu->deleteLater();
        trayIcon->deleteLater();
        trayIcon = 0;
    }
}

void WebWidget::trayIconClicked(QSystemTrayIcon::ActivationReason reason){
    if(reason == QSystemTrayIcon::Trigger){
        setVisible(!isVisible());
    }
}

void WebWidget::trayIconMenuClicked(QAction *action){
    QString s = action->property("EventBus").toString();
    gv->webView()->page()->mainFrame()->evaluateJavaScript(QString("EventBus.dispatch('%1')").arg(s));
}
