QT       += core gui widgets printsupport

TARGET = RiskGuard
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alertswidget.cpp \
    eventanalysisdialog.cpp \
    eventriskwidget.cpp \
    logindialog.cpp \
    macroanalysiswidget.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    reportswidget.cpp \
    searchresultsdialog.cpp \
    settingsdialog.cpp \
    stochasticriskwidget.cpp \
    usericon.cpp \
    userprofiledialog.cpp \
    worldmapwidget.cpp

HEADERS += \
    alertswidget.h \
    eventanalysisdialog.h \
    eventriskwidget.h \
    logindialog.h \
    macroanalysiswidget.h \
    mainwindow.h \
    qcustomplot.h \
    reportswidget.h \
    searchresultsdialog.h \
    settingsdialog.h \
    stochasticriskwidget.h \
    usericon.h \
    userprofiledialog.h \
    worldmapwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    theme_dark.qss \
    theme_light.qss
