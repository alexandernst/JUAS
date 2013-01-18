#include "webgraphicsview.h"

void WebGraphicsView::resizeEvent(QResizeEvent *event){
    obj_webView->resize(event->size());
}

WebGraphicsView::WebGraphicsView(QWidget *parent) : QGraphicsView(parent){
    obj_webView = new QGraphicsWebView;
    obj_webView->setAcceptTouchEvents(true);
    obj_webView->setAcceptHoverEvents(true);

    scene = new QGraphicsScene;
    setScene(scene);
    scene->addItem(obj_webView);
    scene->setActiveWindow(obj_webView);

    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
