isEmpty(uri): error(uri cannot be empty.)
!qtHaveModule(qml): error(Qt Declarative module is not installed.)
TEMPLATE = lib
QT += qml
CONFIG += plugin qmltypes relative_qt_rpath utf8_source
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
QML_IMPORT_NAME = $$uri
uri_path = $$replace(QML_IMPORT_NAME, \., /)
import_path = imports/$$uri_path
isEmpty(uri_version): uri_version = 1
QML_IMPORT_MAJOR_VERSION = $$uri_version
DESTDIR = $$import_path
QMLTYPES_FILENAME = $$DESTDIR/plugins.qmltypes
isEmpty(root_path): root_path = $$PWD
PLUGINFILES = $$root_path/$$import_path/qmldir $$files($$root_path/$$import_path/*.qml)
shared {
    pluginfiles_copy.files = $$PLUGINFILES
    pluginfiles_install.files = $$PLUGINFILES $$OUT_PWD/$$QMLTYPES_FILENAME
}
# Make sure our plugin is loadable as well for static Qt builds.
static {
    QMAKE_MOC_OPTIONS += -Muri=$$QML_IMPORT_NAME
    static_plugin_resources.files = $$PLUGINFILES
    static_plugin_resources.prefix = /qt-project.org/$$import_path
    RESOURCES += static_plugin_resources
    pluginfiles_copy.files = $$import_path/qmldir
    pluginfiles_install.files = $$OUT_PWD/$$QMLTYPES_FILENAME
}
pluginfiles_copy.path = $$DESTDIR
COPIES += pluginfiles_copy
install_path = $$[QT_INSTALL_QML]/$$uri_path
pluginfiles_install.path = $$install_path
# The .qmltypes file will be generated afterwards, don't check now.
pluginfiles_install.CONFIG += no_check_exist
target.path = $$install_path
INSTALLS += target pluginfiles_install
OTHER_FILES += $$PLUGINFILES
# CONFIG += install_ok  # Do not cargo-cult this!
