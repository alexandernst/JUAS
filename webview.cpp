#include "webview.h"

void WebView::resizeEvent(QResizeEvent *event){
    this->resize(event->size());
}

WebView::WebView(QWidget *parent) : QWebEngineView(parent){
    //obj_webView->setAcceptTouchEvents(true);
    //obj_webView->setAcceptHoverEvents(true);

    this->setObjectName("WebGraphicsView");
    setStyleSheet("QWidget#WebGraphicsView { background: transparent; }");
}

void WebView::loadFile(QString fileName){
    QString filePath = QFileInfo(fileName).absoluteFilePath();
    this->setUrl(QUrl::fromLocalFile(filePath));
}

void WebView::loadUrl(QUrl url){
    this->setUrl(url);
}

QWebEngineView *WebView::webView(){
    return this;
}
