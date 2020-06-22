QT       += core gui thelib network
SHARE_APP_NAME = theshop

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = theshop

# Include the-libs build tools
include(/usr/share/the-libs/pri/buildmaster.pri)

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += packagekitqt5
    INCLUDEPATH += /usr/include/packagekitqt5/PackageKit

    INCLUDEPATH += /usr/include/AppStreamQt/
    LIBS += -lAppStreamQt
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    metadatarepository.cpp \
    packagemodel.cpp \
    updates/updatesmodel.cpp \
    updates/updateswidget.cpp

HEADERS += \
    mainwindow.h \
    metadatarepository.h \
    packagemodel.h \
    updates/updatesmodel.h \
    updates/updateswidget.h

FORMS += \
    mainwindow.ui \
    updates/updateswidget.ui

unix {
    target.path = /usr/bin/

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.theshop.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/theshop.svg

    INSTALLS += target icon desktop
}


RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.theshop.desktop
