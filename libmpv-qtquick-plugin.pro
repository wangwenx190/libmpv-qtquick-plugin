TARGET = $$qtLibraryTarget(mpvwrapperplugin)
QT += quick
unix: !android: !macx: QT += x11extras
CONFIG += c++17 strict_c++ warn_on rtti_off exceptions_off
VERSION = 1.0.0.0
win32: shared {
    QMAKE_TARGET_PRODUCT = "MpvDeclarativeWrapper"
    QMAKE_TARGET_DESCRIPTION = "libmpv wrapper for Qt Quick"
    QMAKE_TARGET_COPYRIGHT = "GNU Lesser General Public License version 3"
    QMAKE_TARGET_COMPANY = "wangwenx190"
    CONFIG += skip_target_version_ext
}
DEFINES += MPV_ENABLE_DEPRECATED=0
dynamic_libmpv: DEFINES += WWX190_DYNAMIC_LIBMPV
win32: !mingw {
    # You can download shinchiro's libmpv SDK (build from mpv's master branch) from:
    # https://sourceforge.net/projects/mpv-player-windows/files/libmpv/
    isEmpty(MPV_SDK_DIR) {
        error(You have to setup \"MPV_SDK_DIR\" in \".qmake.conf\" first!)
    } else {
        MPV_LIB_DIR = $$MPV_SDK_DIR/lib
        MPV_LIB_DIR_EX = $$MPV_LIB_DIR/x
        contains(QMAKE_TARGET.arch, x86_64) {
            MPV_LIB_DIR_EX = $$join(MPV_LIB_DIR_EX,,,64)
        } else {
            MPV_LIB_DIR_EX = $$join(MPV_LIB_DIR_EX,,,86)
        }
        INCLUDEPATH += $$MPV_SDK_DIR/include
        # How to generate .lib files from .def files for MSVC:
        # https://github.com/mpv-player/mpv/blob/master/DOCS/compile-windows.md#linking-libmpv-with-msvc-programs
        !dynamic_libmpv: LIBS += -L$$MPV_SDK_DIR -L$$MPV_LIB_DIR -L$$MPV_LIB_DIR_EX -lmpv
    }
} else: !dynamic_libmpv {
    CONFIG += link_pkgconfig
    PKGCONFIG += mpv
}
HEADERS += mpvobject.h mpvqthelper.hpp
SOURCES += mpvobject.cpp plugin.cpp
uri = wangwenx190.QuickMpv
include(qmlplugin.pri)
