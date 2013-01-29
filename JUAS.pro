html_folder.source = html
html_folder.target = .
DEPLOYMENTFOLDERS = html_folder

INCLUDEPATH += $$PWD
QMAKE_CXXFLAGS += -std=c++0x

QT += webkitwidgets widgets network

SOURCES += \
    main.cpp \
    webwidget.cpp \
    webgraphicsview.cpp

HEADERS += \
    webwidget.h \
    webgraphicsview.h

defineTest(addHTML) {
    for(deploymentfolder, DEPLOYMENTFOLDERS) {
        item = item$${deploymentfolder}
        itemsources = $${item}.files
        $$itemsources = $$eval($${deploymentfolder}.source)
        itempath = $${item}.path
        $$itempath= $$eval($${deploymentfolder}.target)
        export($$itemsources)
        export($$itempath)
        DEPLOYMENT += $$item
    }

    export (DEPLOYMENT)
}

addHTML()
