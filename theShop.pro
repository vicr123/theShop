TEMPLATE = subdirs

SUBDIRS += \
    application

exists($$[QT_INSTALL_HEADERS]/thedesk/plugininterface.h) {
    SUBDIRS += thedesk-plugin
}
