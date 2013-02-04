#include "webwidget.h"
#include <QtWidgets/QAction>
#include <QAction>

WebWidget::WebWidget() : QMainWindow(){

    trayIcon = 0;
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









/********************\
|
| TRAYICON
|
\********************/

//TODO: Support submenus
//TODO: Support separators

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

void WebWidget::setTrayIconIcon(QString icon){
    if(trayIcon != 0){
        trayIcon->setIcon(QIcon(icon));
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
    if(trayIcon != 0){
        trayIcon->setToolTip(title);
    }
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

QAction *WebWidget::createTrayIconMenuItem(QString id, QString text, QString event, QString icon){
    QAction *action;

    if(icon == ""){
        action = new QAction(text, trayIconMenu);
    }else{
        action = new QAction(QIcon(icon), text, trayIconMenu);
    }

    action->setProperty("ID", id);
    action->setProperty("EventBus", event);
    return action;
}

void WebWidget::addTrayIconMenuItem(QString id, QString text, QString event, QString icon){
    if(trayIcon != 0){
        QAction *action = createTrayIconMenuItem(id, text, event, icon);;
        trayIconMenu->addAction(action);
    }
}

void WebWidget::addTrayIconMenuItemBefore(QString id_menu_item, QString id, QString text, QString event, QString icon){
    if(trayIcon != 0){
        QList<QAction*> actions = trayIconMenu->actions();
        for(QList<QAction*>::iterator it = actions.begin(); it != actions.end(); ++it){
            QAction *action = qobject_cast<QAction *>(*it);
            if(id_menu_item == action->property("ID").toString()){
                QAction *new_action = createTrayIconMenuItem(id, text, event, icon);
                trayIconMenu->insertAction(action, new_action);
                break;
            }
        }
    }
}

void WebWidget::addTrayIconMenuItemAfter(QString id_menu_item, QString id, QString text, QString event, QString icon){
    if(trayIcon != 0){
        bool found = false;
        QAction *action = 0;
        QList<QAction*> actions = trayIconMenu->actions();
        for(QList<QAction*>::iterator it = actions.begin(); it != actions.end(); ++it){
            action = qobject_cast<QAction *>(*it);
            if(id_menu_item == action->property("ID").toString()){
                found = true;
            }else if(found){
                break;
            }
            action = 0;
        }

        QAction *new_action = createTrayIconMenuItem(id, text, event, icon);;
        if(action != 0){
            trayIconMenu->insertAction(action, new_action);
        }else{
            trayIconMenu->addAction(new_action);
        }

    }
}

void WebWidget::removeTrayIconMenuItem(QString id){
    if(trayIcon != 0 && !trayIconMenu->isEmpty()){
        QList<QAction*> actions = trayIconMenu->actions();
        for(QList<QAction*>::iterator it = actions.begin(); it != actions.end(); ++it){
            QAction *action = qobject_cast<QAction *>(*it);
            if(id == action->property("ID").toString()){
                trayIconMenu->removeAction(action);
                break;
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
    wf->evaluateJavaScript(QString("EventBus.dispatch('%1')").arg(s));
}
