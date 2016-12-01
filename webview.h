#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QUrl>
#include <QDir>
#include <QtWidgets>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QGraphicsLinearLayout>

class WebView : public QWebEngineView{
    Q_OBJECT

    public:
        explicit WebView(QWidget *parent = 0);

        QWebEngineView *webView();
        void loadUrl(QUrl url);
        void resizeEvent(QResizeEvent *event);
        void loadFile(QString fileName);
};

#endif // WEBVIEW_H
