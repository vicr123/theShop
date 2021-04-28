QT += gui widgets tdesktopenvironment

TEMPLATE = lib
CONFIG += plugin
TARGET = theshop-thedesk-plugin

CONFIG += c++11

INCLUDEPATH += $$[QT_INSTALL_HEADERS]/libthedesk $$[QT_INSTALL_HEADERS]/thedesk/
LIBS += -lthedesk
CONFIG += link_pkgconfig
PKGCONFIG += packagekitqt5
INCLUDEPATH += /usr/include/packagekitqt5/PackageKit

DBUS_ADAPTORS += com.vicr123.theShop.DesktopSession.xml

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
    offlinecheck.cpp \
    offlineerrorresults.cpp \
    packagekitdaemon.cpp \
    plugin.cpp \
    shopdbus.cpp

HEADERS += \
    offlinecheck.h \
    offlineerrorresults.h \
    packagekitdaemon.h \
    plugin.h \
    shopdbus.h

DISTFILES += \
    Plugin.json

target.path = $$THELIBS_INSTALL_LIB/thedesk/plugins
INSTALLS += target

FORMS += \
    offlineerrorresults.ui
