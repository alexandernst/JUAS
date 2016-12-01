INCLUDEPATH += $$PWD
QMAKE_CXXFLAGS += -std=c++0x

QT += webenginewidgets widgets network

SOURCES += \
    main.cpp \
    trayicon.cpp \
    webwidget.cpp \
    webview.cpp

HEADERS += \
    webwidget.h \
    webview.h

unix:mac {
    copydata.commands = $(COPY_DIR) $$PWD/html $$OUT_PWD/$${TARGET}.app/Contents/MacOS
} else {
    copydata.commands = $(COPY_DIR) $$PWD/html $$OUT_PWD/
}
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

DISTFILES += \
    html/js/libs/EventBus.js \
    html/js/libs/jquery-layout.js \
    html/js/libs/jquery-ui.js \
    html/js/libs/jquery.js \
    html/js/libs/underscore.js \
    html/js/my_app.js \
    html/css/smoothness/images/ui-bg_flat_0_aaaaaa_40x100.png \
    html/css/smoothness/images/ui-bg_flat_75_ffffff_40x100.png \
    html/css/smoothness/images/ui-bg_glass_55_fbf9ee_1x400.png \
    html/css/smoothness/images/ui-bg_glass_65_ffffff_1x400.png \
    html/css/smoothness/images/ui-bg_glass_75_dadada_1x400.png \
    html/css/smoothness/images/ui-bg_glass_75_e6e6e6_1x400.png \
    html/css/smoothness/images/ui-bg_glass_95_fef1ec_1x400.png \
    html/css/smoothness/images/ui-bg_highlight-soft_75_cccccc_1x100.png \
    html/css/smoothness/images/ui-icons_222222_256x240.png \
    html/css/smoothness/images/ui-icons_2e83ff_256x240.png \
    html/css/smoothness/images/ui-icons_454545_256x240.png \
    html/css/smoothness/images/ui-icons_888888_256x240.png \
    html/css/smoothness/images/ui-icons_cd0a0a_256x240.png \
    html/img/trayicon.png \
    html/img/trayicon_disabled.png \
    html/css/smoothness/jquery-ui.css \
    html/css/themes/default/main.css \
    html/index.html \
    html/js/libs/core.js
