#include "webgraphicsview.h"

void WebGraphicsView::resizeEvent(QResizeEvent *event){
    obj_webView->resize(event->size());
}

WebGraphicsView::WebGraphicsView(QWidget *parent) : QGraphicsView(parent){
    obj_webView = new QGraphicsWebView;
    obj_webView->setAcceptTouchEvents(true);
    obj_webView->setAcceptHoverEvents(true);

    obj_scene = new QGraphicsScene;
    setScene(obj_scene);
    obj_scene->addItem(obj_webView);
    obj_scene->setActiveWindow(obj_webView);

    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setObjectName("WebGraphicsView");
#ifdef QT_NO_DEBUG
    setStyleSheet("QWidget#WebGraphicsView { background: transparent; }");
#else
    setStyleSheet("QWidget#WebGraphicsView { background: green; }");
#endif

}

void WebGraphicsView::loadFile(QString fileName){
    QString filePath = QFileInfo(fileName).absoluteFilePath();
    obj_webView->setUrl(QUrl::fromLocalFile(filePath));
}

void WebGraphicsView::loadUrl(QUrl url){
    obj_webView->setUrl(url);
}

QGraphicsWebView *WebGraphicsView::webView(){
    return obj_webView;
}

QGraphicsScene *WebGraphicsView::scene(){
    return obj_scene;
}
