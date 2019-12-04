TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mpvwrapperplugin)
QT += quick
unix: !android: !macx: QT += x11extras

# Qt's QML plugins should be relocatable
CONFIG += relative_qt_rpath

# Disable deprecated mpv APIs.
DEFINES += MPV_ENABLE_DEPRECATED=0

win32: !mingw {
    # You can download shinchiro's libmpv SDK (build from mpv's master branch) from:
    # https://sourceforge.net/projects/mpv-player-windows/files/libmpv/
    isEmpty(MPV_SDK_DIR) {
        error("You have to setup \"MPV_SDK_DIR\" in \"user.conf\" first!")
    } else {
        MPV_BIN_DIR = $$MPV_SDK_DIR/bin/x
        MPV_LIB_DIR = $$MPV_SDK_DIR/lib/x
        contains(QMAKE_TARGET.arch, x86_64) {
            MPV_BIN_DIR = $$join(MPV_BIN_DIR,,,64)
            MPV_LIB_DIR = $$join(MPV_LIB_DIR,,,64)
        } else {
            MPV_BIN_DIR = $$join(MPV_BIN_DIR,,,86)
            MPV_LIB_DIR = $$join(MPV_LIB_DIR,,,86)
        }
        INCLUDEPATH += $$MPV_SDK_DIR/include
        # How to generate .lib files from .def files for MSVC:
        # https://github.com/mpv-player/mpv/blob/master/DOCS/compile-windows.md#linking-libmpv-with-msvc-programs
        LIBS += \
            -L$$MPV_SDK_DIR \
            -L$$MPV_LIB_DIR \
            -lmpv
        libmpv.path = $$[QT_INSTALL_BINS]
        libmpv.files = \
            $$MPV_SDK_DIR/mpv-1.dll \
            $$MPV_BIN_DIR/mpv.dll \
            $$MPV_BIN_DIR/d3dcompiler_43.dll \
            $$MPV_BIN_DIR/youtube-dl.exe
        INSTALLS += libmpv
    }
} else {
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}

VERSION = 1.0.0.0
win32: shared {
    QMAKE_TARGET_PRODUCT = "MpvDeclarativeWrapper"
    QMAKE_TARGET_DESCRIPTION = "libmpv wrapper for Qt Quick"
    QMAKE_TARGET_COPYRIGHT = "GNU Lesser General Public License version 3"
    QMAKE_TARGET_COMPANY = "wangwenx190"
    CONFIG += skip_target_version_ext
}

HEADERS += \
    $$files(*.h) \
    $$files(*.hpp)

SOURCES += $$files(*.cpp)

QML_FILES += $$files(*.qml)

DISTFILES += \
    qmldir \
    plugins.qmltypes \
    $$QML_FILES

uri = wangwenx190.QuickMpv

# Insert the plugins URI into its meta data to enable usage
# of static plugins in QtDeclarative:
QMAKE_MOC_OPTIONS += -Muri=$$uri

# The resources must be compiled into the library if we want
# to build it as a static plugin.
static: CONFIG += builtin_resources
else: CONFIG += install_qml_files

builtin_resources {
    static_plugin_resources.files = \
        qmldir \
        $$QML_FILES
    # For static qml plugins, the prefix of all resources must be
    # "/qt-project.org/imports/<Your URI (must replace '.' with '/')>",
    # otherwise they can't be loaded successfully by the qml engine.
    static_plugin_resources.prefix = /qt-project.org/imports/$$replace(uri, \., /)
    RESOURCES += static_plugin_resources
}

qmldir.files = qmldir
qmltypes.files = plugins.qmltypes
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \., /)
qmldir.path = $$installPath
qmltypes.path = $$installPath
target.path = $$installPath
INSTALLS += \
    target \
    qmldir \
    qmltypes
install_qml_files {
    qml.files = $$QML_FILES
    qml.path = $$installPath
    INSTALLS += qml
}

# plugins.qmltypes is used by Qt Creator for syntax highlighting and the QML code model.  It needs
# to be regenerated whenever the QML elements exported by the plugin change.  This cannot be done
# automatically at compile time because qmlplugindump does not support some QML features and it may
# not be possible when cross-compiling.
#
# To regenerate run 'make qmltypes' which will update the plugins.qmltypes file in the source
# directory.  Then review and commit the changes made to plugins.qmltypes.
#
# Turns out that the qmlplugindump tool will be unconditionally disabled when building a static
# version of Qt. Only the shared Qt can build it.
!cross_compile: shared {
    # qtPrepareTool() must be called outside a build pass, as it protects
    # against concurrent wrapper creation by omitting it during build passes.
    # However, creating the actual targets is reserved to the build passes.
    qtPrepareTool(QMLPLUGINDUMP, qmlplugindump)
    exists($$QMLPLUGINDUMP) {
        build_pass|!debug_and_release {
            load(resolve_target)
            qmltypes.target = qmltypes
            qmltypes.commands = $$QMLPLUGINDUMP -nonrelocatable $$uri 1.0 > $$_PRO_FILE_PWD_/plugins.qmltypes
            qmltypes.depends = $$QMAKE_RESOLVED_TARGET
        } else {
            # Causing an error when only build the release version.
            # qmltypes.CONFIG += recursive
        }
        QMAKE_EXTRA_TARGETS += qmltypes
    }
}

# Generate documentation
qtPrepareTool(QDOC, qdoc)
exists($$QDOC) {
    build_pass|!debug_and_release {
        docs.target = docs
        # Running QDoc in Single Execution Mode will cause errors. Don't know why.
        docs.commands = $$QDOC $$_PRO_FILE_PWD_/mpvdeclarativewrapper.qdocconf
    }
    QMAKE_EXTRA_TARGETS += docs
}
