# libmpv-qtquick-plugin

libmpv wrapper for Qt Quick. Can be used as a normal visual element in .qml files easily.

## Features

- Can be easily embeded into any Qt Quick GUI applications.
- Support both shared and static ways of compiling, linking and loading.
- Cross-platform support: Windows, Linux, macOS, Android, iOS, UWP, etc.

## TODO

- Support the *CMake + Ninja* build system.

  Note: Qt is dropping QMake support and is migrating to use CMake in the coming major release, Qt 6. However, CMake is still not quite usable when creating qml plugins in Qt 5 days, so let's migrate to CMake in Qt 6.

## Usage

Once you have installed this plugin successfully, you can use it like any other normal visual elements of Qt Quick in .qml files:

```qml
import QtQuick.Dialogs 1.3
import wangwenx190.QuickMpv 1.0

Shortcut {
    sequence: StandardKey.Open
    onActivated: fileDialog.open()
}

FileDialog {
    id: fileDialog

    title: qsTr("Please select a media file.")
    folder: shortcuts.movies
    nameFilters: [qsTr("Video files (%1)").arg(mpvPlayer.videoSuffixes.join(' ')), qsTr("Audio files (%1)").arg(mpvPlayer.audioSuffixes.join(' ')), qsTr("All files (*)")]

    onAccepted: mpvPlayer.source = fileDialog.fileUrl
}

MpvPlayer {
    id: mpvPlayer
    anchors.fill: parent

    source: "file:///D:/Videos/test.mkv" // playback will start immediately once the source url is changed
    hrSeek: false
    loadScripts: true
    ytdl: true
    screenshotFormat: "png" // "jpg" or "png"
    logLevel: MpvObject.Debug
    volume: 85 // 0-100

    onPositionChanged: // do something
    onDurationChanged: // do something
    onVideoSizeChanged: // do something
    onPlaybackStateChanged: // do something
    onMediaStatusChanged: // do something
}
```

Notes

- Be aware of the `initFinished` signal. Please refer to the first topic of the [FAQ](#FAQ) section.
- `MpvPlayer` (defined in [*MpvPlayer.qml*](/imports/wangwenx190/QuickMpv/MpvPlayer.qml)) is just a simple wrapper of the QML type `MpvObject` (defined in [*mpvobject.h*](/mpvobject.h) and [*mpvobject.cpp*](/mpvobject.cpp)). You can also use `MpvObject` directly if you want. It's usage is exactly the same with `MpvPlayer`.
- `mpvPlayer.duration`, `mpvPlayer.position` and `mpvPlayer.seek(offset)` use **SECONDS** instead of milliseconds.
- `mpvPlayer.seek(offset)` uses relative offset, not absolute position. You can use a negative number to jump backward. If you want to jump to an absolute position, please consider using `mpvPlayer.seekAbsolute(position)` instead. There also exists a method called `mpvPlayer.seekPercent(percent)`, which can jump to a known percent of the playback progress, the parameter *percent* should be an integer between 0 and 100. `mpvPlayer.seekRelative(offset)` is just an alias of `mpvPlayer.seek(offset)`.
- You can use `mpvPlayer.open(url)` to load and play *url* directly, it is equivalent to `mpvPlayer.source = url` (no need to call `mpvPlayer.play()` manually, because the playback will start immediately once the source url is changed).
- You can also use `mpvPlayer.play()` to resume a paused playback, `mpvPlayer.pause()` to pause a playing playback, `mpvPlayer.stop()` to stop a loaded playback and `mpvPlayer.seek(offset)` to jump to a different position.
- To get the current playback state, use `mpvPlayer.isPlaying()`, `mpvPlayer.isPaused()` and `mpvPlayer.isStopped()`.
- Qt will load the qml plugins automatically if you have installed them into their correct locations, you don't need to load them manually (and to be honest I don't know how to load them manually either).
- If you want to integrate it into your application rather than load it dynamically, the traditional `qmlRegisterType()` function is also supported.

For more information, please refer to [*MpvPlayer.qml*](/imports/wangwenx190/QuickMpv/MpvPlayer.qml).

## Compilation

Before doing anything else, please make sure you have a compiler that supports at least C++14 and a recent version of Qt.

1. Checkout source code:

   ```bash
   git clone https://github.com/wangwenx190/libmpv-qtquick-plugin.git
   ```

   Note: Please remember to install *Git* yourself. Windows users can download it from: <https://git-scm.com/downloads>

2. Setup libmpv SDK:

   For Linux developers, you just need to install `libmpv-dev` (or something like that, depending on your Linux distro). No more things to do. It's that easy.

   However, if you are using Windows, things are a little different. You can download *shinchiro*'s package from <https://sourceforge.net/projects/mpv-player-windows/files/libmpv/> , the **mpv.lib** needed by MSVC should be generated manually, you can refer to <https://github.com/mpv-player/mpv/blob/master/DOCS/compile-windows.md#linking-libmpv-with-msvc-programs> for more information. Here's an excerpt:

   > You can build C++ programs in Visual Studio and link them with libmpv. To do this, you need a Visual Studio which supports ``stdint.h`` (recent ones do), and you need to create a import library for the mpv DLL:
   >
   > ```bat
   > lib /def:mpv.def /name:mpv-1.dll /out:mpv.lib /MACHINE:X64
   > ```
   >
   > The string in the ``/name:`` parameter must match the filename of the DLL (this is simply the filename the MSVC linker will use). The ``mpv.def`` can be retrieved from the mpv build directory, or can be produced by MingGW's gendef.exe helper from the mpv DLL.
   >
   > Static linking is not possible.

   Once everything is ready, then write the following things to a text file named **.qmake.conf** and save it to this repository's directory:

   ```conf
   # You should replace the "D:/code/mpv-sdk" with your own path.
   # Better to use "/" instead of "\", even on Windows platform.
   isEmpty(MPV_SDK_DIR): MPV_SDK_DIR = D:/code/mpv-sdk
   ```

   Note: If you are using shinchiro's package, the mpv header files will be located in *$$MPV_SDK_DIR/include*, while the correct location should be *$$MPV_SDK_DIR/include/mpv*, please remember to move these files to the new folder yourself, otherwise the compiler will complaint about cannot find mpv's header file.

3. Create a directory for building:

   Linux:

   ```bash
   mkdir build
   cd build
   ```

   Windows (cmd):

   ```bat
   md build
   cd build
   ```

   Windows (PowerShell):

   ```powershell
   New-Item -Path "build" -ItemType "Directory"
   Set-Location -Path "build"
   ```

4. Build and Install:

   Linux:

   ```bash
   qmake
   make
   make install
   ```

   Windows:

   ```bat
   qmake
   jom/nmake/mingw32-make
   jom/nmake/mingw32-make install
   ```

   Note: If you are not using MinGW, then *JOM* is your best choice on Windows. Qt's official website to download *JOM*: <http://download.qt.io/official_releases/jom/>

## FAQ

- Why another window appears instead of rendering in my own application?

   Because the initialization of libmpv's renderer is a bit slow, if you try to play any media contents before it is initialized, libmpv will create a separate window to do the rendering. You can use the `initFinished` signal to avoid this. The `initFinished` signal is emitted once the renderer finished initializing. Don't use the `completed` signal of the component (`Component.onCompleted`) because the initialization of the component is very fast. You should wait for the `initFinished` signal instead.
- Why is the CPU usage very high (>=10%) ?

   You need to enable **hardware decoding** manually:

   ```qml
   import wangwenx190.QuickMpv 1.0

   MpvPlayer {
       // ...
       hwdec: "auto" // type: string
       // ...
   }
   ```

- Why is the playback process not smooth enough or even stuttering?

   If you can insure the video file itself isn't damaged, here are two possible reasons and their corresponding solutions:
   1. You are using **software decoding** instead of hardware decoding.

      libmpv will not enable **hardware decoding** by default. You will have to enable it manually if you need it. Please refer to the previous topic to learn about how to enable it.
   2. You need a more powerful GPU, maybe even a better CPU. libmpv is never designed to run on too crappy computers.

- How to set the log level of libmpv?

    ```qml
   import wangwenx190.QuickMpv 1.0

   MpvPlayer {
       // ...
       logLevel: MpvObject.Debug // type: enum
       // ...
   }
   ```

   Note: For more log levels, please refer to [*MpvPlayer.qml*](/imports/wangwenx190/QuickMpv/MpvPlayer.qml).
- Why my application complaints about failed to create EGL context ... etc at startup and then crashed?

   ANGLE only supports OpenGL version <= 3.1. Please check whether you are using OpenGL newer than 3.1 through ANGLE or not.

   Desktop OpenGL doesn't have this limit. You can use any version you like. The default version that Qt uses is 2.0, which I think is kind of out-dated.

   Here is how to change the OpenGL version in Qt:

   ```cpp
   QSurfaceFormat surfaceFormat;
   // Here we use OpenGL version 4.6 for instance.
   // Don't use any versions newer than 3.1 if you are using ANGLE.
   surfaceFormat.setMajorVersion(4);
   surfaceFormat.setMinorVersion(6);
   // You can also use "QSurfaceFormat::CoreProfile" to disable the using of deprecated OpenGL APIs, however, some deprecated APIs will still be usable.
   surfaceFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
   QSurfaceFormat::setDefaultFormat(surfaceFormat);
   ```

## License

[GNU Lesser General Public License version 3](/LICENSE.md)
