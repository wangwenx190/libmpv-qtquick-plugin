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
    # The following line is critical if we are building plugins statically.
    # Don't remove or modify it if you don't know what you are doing.
    QMAKE_MOC_OPTIONS += -Muri=$$QML_IMPORT_NAME
    # The "qmldir" file and all .qml files need to be packed into the
    # static library to let the QML engine be able to load them.
    static_plugin_resources.files = $$PLUGINFILES
    # For static plugins, the root path of all resources must be prefixed by
    # "/qt-project.org/imports", otherwise the QML engine won't be able to load our resources.
    # We don't add "/imports" here because the real file's path already includes it.
    static_plugin_resources.prefix = /qt-project.org
    RESOURCES += static_plugin_resources
    pluginfiles_copy.files = $$import_path/qmldir
    # Although the "qmldir" file has been packed into the static library,
    # we still have to put one copy outside, because QMake won't find
    # our plugin if it's not there.
    pluginfiles_install.files = $$root_path/$$import_path/qmldir $$OUT_PWD/$$QMLTYPES_FILENAME
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
