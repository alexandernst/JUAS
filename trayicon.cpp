#include "webwidget.h"

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
    wp->runJavaScript(QString("EventBus.dispatch('%1')").arg(s));
}
