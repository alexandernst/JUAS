#ifndef WEBGRAPHICSVIEW_H
#define WEBGRAPHICSVIEW_H

#include <QUrl>
#include <QDir>
#include <QtWidgets>
#include <QWebFrame>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QCoreApplication>
#include <QGraphicsLinearLayout>

class WebGraphicsView : public QGraphicsView{
    Q_OBJECT

    public:
        explicit WebGraphicsView(QWidget *parent = 0);

        QGraphicsWebView *webView();
        QGraphicsScene *scene();
        void loadUrl(QUrl url);
        void resizeEvent(QResizeEvent *event);
        void loadFile(QString fileName);

    private:
        QGraphicsScene *obj_scene;
        QGraphicsWebView *obj_webView;
};

#endif // WEBGRAPHICSVIEW_H
