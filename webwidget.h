#ifndef WEBWIDGET_H
#define WEBWIDGET_H

//Qt 5.0 won't minimize frameless windows. TODO: Remove when fixed
// https://bugreports.qt-project.org/browse/QTBUG-29109
#define MINIMIZE_PATCH

#include <QDebug>
#include <QAction>
#include <QCursor>
#include <QtWebEngineWidgets>
#include <QWebEngineView>
#include <QtWidgets>
#include <QWebEnginePage>
#include <QWebEngineSettings>

#include <webview.h>

class WebWidget : public QMainWindow
{
    Q_OBJECT

    public:
        WebWidget();

        //Main app
        Q_INVOKABLE QVariantList availableGeometry();
        Q_INVOKABLE void clearSavedWindowGeometry(QString company, QString app);
        Q_INVOKABLE bool isWindowGeometrySaveAvailable(QString company, QString app);
        Q_INVOKABLE void maximize();
        Q_INVOKABLE void minimize();
        Q_INVOKABLE void mouseMoveEvent(int dragX, int dragY);
        Q_INVOKABLE void mousePressEvent(int fromBorderX, int fromBorderY);
        Q_INVOKABLE void restoreWindowGeometry(QString company, QString app);
        Q_INVOKABLE void saveWindowGeometry(QString company, QString app);
        Q_INVOKABLE void setTitle(QString title);
        Q_INVOKABLE void quit();

        Q_PROPERTY(int x READ getX WRITE setX NOTIFY xChanged)
        Q_INVOKABLE int getX();
        Q_INVOKABLE void setX(int x);

        Q_PROPERTY(int y READ getY WRITE setY NOTIFY yChanged)
        Q_INVOKABLE int getY();
        Q_INVOKABLE void setY(int y);

        Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
        Q_INVOKABLE int getWidth();
        Q_INVOKABLE void setWidth(int width);

        Q_PROPERTY(int height READ getHeight WRITE setHeight NOTIFY heightChanged)
        Q_INVOKABLE int getHeight();
        Q_INVOKABLE void setHeight(int height);

        //Tray icon
        Q_INVOKABLE void addTrayIconMenuItem(QString id, QString text, QString event, QString icon = "");
        Q_INVOKABLE void addTrayIconMenuItemAfter(QString id_menu_item, QString id, QString text, QString event, QString icon = "");
        Q_INVOKABLE void addTrayIconMenuItemBefore(QString id_menu_item, QString id, QString text, QString event, QString icon = "");
        Q_INVOKABLE void createTrayIcon(QString icon);
        Q_INVOKABLE void hideTrayIcon();
        Q_INVOKABLE void removeTrayIcon();
        Q_INVOKABLE void removeTrayIconMenuItems();
        Q_INVOKABLE void removeTrayIconMenuItem(QString id);
        Q_INVOKABLE void resize(int x, int y, int w, int h);
        Q_INVOKABLE void setTrayIconIcon(QString icon);
        Q_INVOKABLE void setTrayIconTitle(QString title);
        Q_INVOKABLE void showTrayIcon();
        Q_INVOKABLE void showTrayIconMessage(QString title, QString msg, QString icon = "NoIcon", int msecs = 10000);

    signals:
        void xChanged(int x);
        void yChanged(int y);
        void widthChanged(int width);
        void heightChanged(int height);

    public slots:
        void xHasChanged(int x);
        void yHasChanged(int y);
        void widthHasChanged(int width);
        void heightHasChanged(int height);

    private:
        QApplication *app;
        QDesktopWidget *desktop;
        QWebChannel *channel;
        QPoint fromBorderPosition;
        QVBoxLayout *layout;
        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;
        QWidget *widget;
        QWebEnginePage *wp;
        QWebEngineSettings *ws;
        WebView *gv;

        QAction *createTrayIconMenuItem(QString id, QString text, QString event, QString icon);

    private slots:
        void cleanUp();
        void createJSBridge();
        void trayIconMenuClicked(QAction *action);
        void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
};

#endif // WEBWIDGET_H
