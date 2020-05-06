import QtQuick 2.15
import wangwenx190.QuickMpv 1.0

/*!
    \qmltype MpvPlayer
    \inherits Item
    \brief A convenience type for playing a specified media content.

    \c MpvPlayer is a libmpv wrapper for Qt Quick. It can be easily
    embeded into any Qt Quick GUI applications.

    \qml
    MpvPlayer {
        id: mpvPlayer

        width : 800
        height : 600

        source: "video.avi"

        MouseArea {
            anchors.fill: parent
            onClicked: mpvPlayer.play()
        }

        focus: true
        Keys.onSpacePressed: mpvPlayer.isPlaying() ? mpvPlayer.pause() : mpvPlayer.play()
        Keys.onLeftPressed: mpvPlayer.seek(-5)
        Keys.onRightPressed: mpvPlayer.seek(5)
    }
    \endqml

    Notes

    \list
    \li Do \b NOT try to interact with the C++ class directly. Just do everything
        you want through this QML type.
    \li \c MpvPlayer is just a simple wrapper of the QML type \c MpvObject. You can
        also use \c MpvObject directly if you want. It's usage is exactly the same
        with \c MpvPlayer.
    \li To get the best performance, try \c ANGLE. Using desktop OpenGL is also fine
        but it's performance will not be as good as ANGLE. Never use \c {Mesa llvmpipe}
        unless you are running your applications in virtual machines.
    \li The default OpenGL version that Qt uses is \c 2.0, which I think is kind of
        out-dated, you can use a newer version instead. However, don't use any versions
        newer than \c 3.1 if you are using ANGLE, because ANGLE doesn't support OpenGL
        versions newer than 3.1.
    \li The official libmpv package only provides a shared library, however, you can
        compile it yourself to get the static library. Static linking is possible,
        see how \c bomi does. Goto shinchiro's repository to see how to compile it.
    \li Qt will load the QML plugins automatically if you have installed them into their
        correct locations, you don't need to load them manually (and to be honest I don't
        know how to load them manually either).
    \endlist
*/

Item {
    id: mpvPlayer

    /*!
        \qmlproperty url MpvPlayer::source

        This property holds the source URL of the media.

        Playback will start \b immediately once the \l source property changed.
    */
    property alias source: mpvObject.source

    /*!
        \qmlproperty size MpvPlayer::videoSize

        This property holds the video's picture size.
    */
    property alias videoSize: mpvObject.videoSize

    /*!
        \qmlproperty qlonglong MpvPlayer::duration

        Duration of the current file in \b seconds. If the duration is unknown,
        the property is unavailable. Note that the file duration is not always
        exactly known, so this is an estimate.
    */
    property alias duration: mpvObject.duration

    /*!
        \qmlproperty qlonglong MpvPlayer::position

        Position in current file in \b seconds.

        To change this position, use the \l seek() method.

        \sa seek()
    */
    property alias position: mpvObject.position

    /*!
        \qmlproperty int MpvPlayer::volume

        This property holds the audio volume.

        The volume is scaled linearly from \c 0 (silence) to \c 100 (full volume). Values outside
        this range will be clamped.

        The default volume is \c 100.
    */
    property alias volume: mpvObject.volume

    /*!
        \qmlproperty bool MpvPlayer::mute

        This property holds whether the audio output is muted.

        The default value is \c false.
    */
    property alias mute: mpvObject.mute

    /*!
        \qmlproperty bool MpvPlayer::seekable

        This property holds whether the playback position of the video can be
        changed.

        If true, calling the \l seek() method will cause playback to seek to the new position.
    */
    property alias seekable: mpvObject.seekable

    /*!
        \qmlproperty enumeration MpvPlayer::playbackState

        This property indicates the playback state of the media.

        \table
        \header
            \li Value
            \li Description
        \row
            \li MpvObject.Playing
            \li the media is playing
        \row
            \li MpvObject.Paused
            \li the media is paused
        \row
            \li MpvObject.Stopped
            \li the media is stopped
        \endtable

        The default playback state is \c MpvObject.Stopped.
    */
    property alias playbackState: mpvObject.playbackState

    /*!
        \qmlproperty enumeration MpvPlayer::mediaStatus

        This property holds the status of media loading. It can be one of:

        \table
        \header
            \li Value
            \li Description
        \row
            \li MpvObject.Unknown
            \li the status of the media cannot be determined
        \row
            \li MpvObject.NoMedia
            \li no media has been set
        \row
            \li MpvObject.Loading
            \li the media is currently being loaded
        \row
            \li MpvObject.Loaded
            \li the media has been loaded
        \row
            \li MpvObject.Stalled
            \li playback has been interrupted while the media is buffering data
        \row
            \li MpvObject.Buffering
            \li the media is buffering data
        \row
            \li MpvObject.Buffered
            \li the media has buffered data
        \row
            \li MpvObject.End
            \li the media has played to the end
        \row
            \li MpvObject.Invalid
            \li the media cannot be played
        \endtable

        The default media status is \c MpvObject.NoMedia.
    */
    property alias mediaStatus: mpvObject.mediaStatus

    /*!
        \qmlproperty enumeration MpvPlayer::logLevel

        This property indicates the log level of libmpv.

        \table
        \header
            \li Value
            \li Description
        \row
            \li MpvObject.Off
            \li turn off logging output for libmpv
        \row
            \li MpvObject.Debug
            \li debug level (noisy)
        \row
            \li MpvObject.Warning
            \li warning level (less noisy)
        \row
            \li MpvObject.Critical
            \li critical level (less noisy)
        \row
            \li MpvObject.Fatal
            \li fatal level (less noisy)
        \row
            \li MpvObject.Info
            \li info level (less noisy)
        \endtable

        The default log level is \c MpvObject.Debug.
    */
    property alias logLevel: mpvObject.logLevel

    /*!
        \qmlproperty string MpvPlayer::hwdec

        This property holds the hardware decoding algorithm of the media.

        \table
        \header
            \li Algorithm name
            \li Brief description
        \row
            \li no
            \li always use software decoding (default)
        \row
            \li auto
            \li enable best hw decoder (see below)
        \row
            \li yes
            \li exactly the same as \c auto
        \row
            \li auto-copy
            \li enable best hw decoder with copy-back (see below)
        \row
            \li vdpau
            \li requires \c --vo=gpu or \c --vo=vdpau (Linux only)
        \row
            \li vdpau-copy
            \li copies video back into system RAM (Linux with some GPUs only)
        \row
            \li vaapi
            \li requires \c --vo=gpu or \c --vo=vaapi (Linux only)
        \row
            \li vaapi-copy
            \li copies video back into system RAM (Linux with some GPUs only)
        \row
            \li videotoolbox
            \li requires \c --vo=gpu (OS X 10.8 and up), or \c --vo=opengl-cb (iOS 9.0 and up)
        \row
            \li videotoolbox-copy
            \li copies video back into system RAM (OS X 10.8 or iOS 9.0 and up)
        \row
            \li dxva2
            \li requires \c --vo=gpu with \c --gpu-context=d3d11, \c --gpu-context=angle or \c --gpu-context=dxinterop (Windows only)
        \row
            \li dxva2-copy
            \li copies video back to system RAM (Windows only)
        \row
            \li d3d11va
            \li requires \c --vo=gpu with \c --gpu-context=d3d11 or \c --gpu-context=angle (Windows 8+ only)
        \row
            \li d3d11va-copy
            \li copies video back to system RAM (Windows 8+ only)
        \row
            \li mediacodec
            \li requires \c --vo=mediacodec_embed (Android only)
        \row
            \li mediacodec-copy
            \li copies video back to system RAM (Android only)
        \row
            \li mmal
            \li requires \c --vo=gpu (Raspberry Pi only - default if available)
        \row
            \li mmal-copy
            \li copies video back to system RAM (Raspberry Pi only)
        \row
            \li cuda
            \li requires \c --vo=gpu (Any platform CUDA is available)
        \row
            \li cuda-copy
            \li copies video back to system RAM (Any platform CUDA is available)
        \row
            \li nvdec
            \li requires \c --vo=gpu (Any platform CUDA is available)
        \row
            \li nvdec-copy
            \li copies video back to system RAM (Any platform CUDA is available)
        \row
            \li crystalhd
            \li copies video back to system RAM (Any platform supported by hardware)
        \row
            \li rkmpp
            \li requires \c --vo=gpu (some RockChip devices only)
        \endtable

        \c auto tries to automatically enable hardware decoding using the first available method.
        This still depends what VO you are using. For example, if you are not using \c --vo=gpu
        or \c --vo=vdpau, vdpau decoding will never be enabled. Also note that if the first found
        method doesn't actually work, it will always fall back to software decoding, instead of
        trying the next method (might matter on some Linux systems).

        \c auto-copy selects only modes that copy the video data back to system memory after decoding.
        This selects modes like \c vaapi-copy (and so on). If none of these work, hardware decoding
        is disabled. This mode is always guaranteed to incur no additional loss compared to software
        decoding, and will allow CPU processing with video filters.

        The \c vaapi mode, if used with \c --vo=gpu, requires Mesa 11 and most likely works with
        Intel GPUs only. It also requires the opengl EGL backend.

        The \c cuda and \c cuda-copy modes provides deinterlacing in the decoder which is useful as there
        is no other deinterlacing mechanism in the gpu output path. To use this deinterlacing you
        must pass the option: \c vd-lavc-o=deint=[weave|bob|adaptive]. Pass \c weave (or leave the option
        unset) to not attempt any deinterlacing. \c cuda should always be preferred unless the \c gpu vo
        is not being used or filters are required.

        \c nvdec is a newer implementation of CUVID/CUDA decoding, which uses the FFmpeg decoders for
        file parsing. Experimental, is known not to correctly check whether decoding is supported by
        the hardware at all. Deinterlacing is not supported. Since this uses FFmpeg's codec parsers,
        it is expected that this generally causes fewer issues than \c cuda.

        Most video filters will not work with hardware decoding as they are primarily implemented on
        the CPU. Some exceptions are \c vdpaupp, \c vdpaurb and \c vavpp.

        The \c ...-copy modes (e.g. \c dxva2-copy) allow you to use hardware decoding with any VO,
        backend or filter. Because these copy the decoded video back to system RAM, they're likely less
        efficient than the direct modes (like e.g. \c dxva2), and probably not more efficient than
        software decoding except for some codecs (e.g. HEVC).

        Hardware decoding is \b disabled by default. You will have to enable it
        manually if you want to use it.
    */
    property alias hwdec: mpvObject.hwdec

    /*!
        \qmlproperty string MpvPlayer::mpvVersion

        Return the mpv version/copyright string. Depending on how the binary was built, it might
        contain either a release version, or just a git hash.
    */
    property alias mpvVersion: mpvObject.mpvVersion

    /*!
        \qmlproperty string MpvPlayer::mpvConfiguration

        Return the configuration arguments which were passed to the build system
        (typically the way \c {./waf configure ...} was invoked).
    */
    property alias mpvConfiguration: mpvObject.mpvConfiguration

    /*!
        \qmlproperty string MpvPlayer::ffmpegVersion

        Return the contents of the \c av_version_info() API call. This is a string
        which identifies the build in some way, either through a release version number,
        or a git hash. This applies to Libav as well (the property is still named the
        same.) This property is unavailable if mpv is linked against older FFmpeg and
        Libav versions.
    */
    property alias ffmpegVersion: mpvObject.ffmpegVersion

    /*!
        \qmlproperty int MpvPlayer::vid

        Select video channel. \c auto selects the default, \c no disables video.

        \c --video is an alias for \c --vid.
    */
    property alias vid: mpvObject.vid

    /*!
        \qmlproperty int MpvPlayer::aid

        Select audio track. \c auto selects the default, \c no disables audio.

        \c --audio is an alias for \c --aid.
    */
    property alias aid: mpvObject.aid

    /*!
        \qmlproperty int MpvPlayer::sid

        Display the subtitle stream specified by \c <ID>. \c auto selects the default, \c no disables subtitles

        \c --sub is an alias for \c --sid.
    */
    property alias sid: mpvObject.sid

    /*!
        \qmlproperty int MpvPlayer::videoRotate

        Rotate the video clockwise, in degrees. Currently supports 90° steps only. If \c no is
        given, the video is never rotated, even if the file has rotation metadata. (The
        rotation value is added to the rotation metadata, which means the value \c 0 would
        rotate the video according to the rotation metadata.)
    */
    property alias videoRotate: mpvObject.videoRotate

    /*!
        \qmlproperty double MpvPlayer::videoAspect

        Video aspect ratio

        \table
        \header
            \li Ratio
            \li Value
        \row
            \li 4:3
            \li 1.3333
        \row
            \li 16:9
            \li 1.7777
        \endtable
    */
    property alias videoAspect: mpvObject.videoAspect

    /*!
        \qmlproperty double MpvPlayer::speed

        Playback speed. Default is \c 1.0.
    */
    property alias speed: mpvObject.speed

    /*!
        \qmlproperty bool MpvPlayer::deinterlace

        Enable or disable interlacing (default: no). Interlaced video shows ugly
        comb-like artifacts, which are visible on fast movement. Enabling this
        typically inserts the yadif video filter in order to deinterlace the video,
        or lets the video output apply deinterlacing if supported.
    */
    property alias deinterlace: mpvObject.deinterlace

    /*!
        \qmlproperty bool MpvPlayer::audioExclusive

        Enable exclusive output mode. In this mode, the system is usually locked out,
        and only mpv will be able to output audio.

        This only works for some audio outputs, such as \c wasapi and \c coreaudio. Other
        audio outputs silently ignore this options. They either have no concept of
        exclusive mode, or the mpv side of the implementation is missing.
    */
    property alias audioExclusive: mpvObject.audioExclusive

    /*!
        \qmlproperty string MpvPlayer::audioFileAuto

        Load additional audio files matching the video filename. The parameter
        specifies how external audio files are matched.

        \table
        \header
            \li Value
            \li Description
        \row
            \li no
            \li Don't automatically load external audio files (default).
        \row
            \li exact
            \li Load the media filename with audio file extension.
        \row
            \li fuzzy
            \li Load all audio files containing media filename.
        \row
            \li all
            \li Load all audio files in the current and \c --audio-file-paths directories.
        \endtable
    */
    property alias audioFileAuto: mpvObject.audioFileAuto

    /*!
        \qmlproperty string MpvPlayer::subAuto

        Load additional subtitle files matching the video filename. The parameter
        specifies how external subtitle files are matched. \c exact is enabled by default.

        \table
        \header
            \li Value
            \li Description
        \row
            \li no
            \li Don't automatically load external subtitle files.
        \row
            \li exact
            \li Load the media filename with subtitle file extension (default).
        \row
            \li fuzzy
            \li Load all subs containing media filename.
        \row
            \li all
            \li Load all subs in the current and \c --sub-file-paths directories.
        \endtable
    */
    property alias subAuto: mpvObject.subAuto

    /*!
        \qmlproperty string MpvPlayer::subCodepage

        You can use this option to specify the subtitle codepage. uchardet will be
        used to guess the charset. (If mpv was not compiled with uchardet, then
        \c utf-8 is the effective default.)

        The default value for this option is \c auto, which enables autodetection.

        The following steps are taken to determine the final codepage, in order:

        \list
        \li if the specific codepage has a \c +, use that codepage
        \li if the data looks like UTF-8, assume it is UTF-8
        \li if \c --sub-codepage is set to a specific codepage, use that
        \li run uchardet, and if successful, use that
        \li otherwise, use \c UTF-8-BROKEN
        \endlist

        The pseudo codepage \c UTF-8-BROKEN is used internally. If it's set, subtitles
        are interpreted as UTF-8 with "Latin 1" as fallback for bytes which are not
        valid UTF-8 sequences. iconv is never involved in this mode.
    */
    property alias subCodepage: mpvObject.subCodepage

    /*!
        \qmlproperty string MpvPlayer::fileName

        Currently played file, with path stripped. If this is an URL, try to undo
        percent encoding as well. (The result is not necessarily correct, but looks
        better for display purposes. Use the \c path property to get an unmodified
        filename.)
    */
    property alias fileName: mpvObject.fileName

    /*!
        \qmlproperty string MpvPlayer::mediaTitle

        If the currently played file has a \c title tag, use that.
        Otherwise, if the media type is DVD, return the volume ID of DVD.
        Otherwise, return the \c filename property.
    */
    property alias mediaTitle: mpvObject.mediaTitle

    /*!
        \qmlproperty string MpvPlayer::vo

        Specify a priority list of video output drivers to be used.
        If the list has a trailing ",", mpv will fall back on drivers not
        contained in the list.

        Available video output drivers are:

        \table
        \header
            \li Driver name
            \li Brief description
        \row
            \li xv (X11 only)
            \li Uses the XVideo extension to enable hardware-accelerated display.
                This is the most compatible VO on X, but may be low-quality, and
                has issues with OSD and subtitle display.
        \row
            \li x11 (X11 only)
            \li Shared memory video output driver without hardware acceleration
                that works whenever X11 is present.
        \row
            \li vdpau (X11 only)
            \li Uses the VDPAU interface to display and optionally also decode
                video. Hardware decoding is used with \c --hwdec=vdpau.
        \row
            \li direct3d (Windows only)
            \li Video output driver that uses the Direct3D interface.
        \row
            \li gpu
            \li General purpose, customizable, GPU-accelerated video output driver.
                It supports extended scaling methods, dithering, color management,
                custom shaders, HDR, and more.
        \row
            \li sdl
            \li SDL 2.0+ Render video output driver, depending on system with or
                without hardware acceleration. Should work on all platforms supported
                by SDL 2.0. For tuning, refer to your copy of the file \c SDL_hints.h.
        \row
            \li vaapi
            \li Intel VA API video output driver with support for hardware decoding.
                Note that there is absolutely no reason to use this, other than
                compatibility. This is low quality, and has issues with OSD.
        \row
            \li null
            \li Produces no video output. Useful for benchmarking.
        \row
            \li caca
            \li Color ASCII art video output driver that works on a text console.
        \row
            \li tct
            \li Color Unicode art video output driver that works on a text console.
                Depends on support of true color by modern terminals to display the
                images at full color range. On Windows it requires an ansi terminal
                such as mintty.
        \row
            \li image
            \li Output each frame into an image file in the current directory. Each
                file takes the frame number padded with leading zeros as name.
        \row
            \li libmpv
            \li For use with libmpv direct embedding. As a special case, on OS X it
                is used like a normal VO within mpv (cocoa-cb). Otherwise useless in
                any other contexts. (See \c <mpv/render.h>.)
                This also supports many of the options the \c gpu VO has, depending
                on the backend.
        \row
            \li drm (Direct Rendering Manager)
            \li Video output driver using Kernel Mode Setting / Direct Rendering Manager.
                Should be used when one doesn't want to install full-blown graphical
                environment (e.g. no X). Does not support hardware acceleration (if you
                need this, check the \c drm backend for \c gpu VO).
        \row
            \li mediacodec_embed (Android)
            \li Renders \c IMGFMT_MEDIACODEC frames directly to an \c android.view.Surface.
                Requires \c --hwdec=mediacodec for hardware decoding, along with
                \c --vo=mediacodec_embed and \c --wid=(intptr_t)(*android.view.Surface).
                Since this video output driver uses native decoding and rendering routines,
                many of mpv's features (subtitle rendering, OSD/OSC, video filters, etc)
                are not available with this driver.
        \endtable
    */
    property alias vo: mpvObject.vo

    /*!
        \qmlproperty string MpvPlayer::ao

        Specify a priority list of audio output drivers to be used.
        If the list has a trailing ",", mpv will fall back on drivers not
        contained in the list.

        Available audio output drivers are:

        \table
        \header
            \li Driver name
            \li Brief description
        \row
            \li alsa (Linux only)
            \li ALSA audio output driver
        \row
            \li oss
            \li OSS audio output driver
        \row
            \li jack
            \li JACK (Jack Audio Connection Kit) audio output driver.
        \row
            \li coreaudio (Mac OS X only)
            \li Native Mac OS X audio output driver using AudioUnits and
                the CoreAudio sound server.
                Automatically redirects to \c coreaudio_exclusive when playing
                compressed formats.
        \row
            \li coreaudio_exclusive (Mac OS X only)
            \li Native Mac OS X audio output driver using direct device access
                and exclusive mode (bypasses the sound server).
        \row
            \li openal
            \li OpenAL audio output driver
        \row
            \li pulse
            \li PulseAudio audio output driver
        \row
            \li sdl
            \li SDL 1.2+ audio output driver. Should work on any platform supported
                by SDL 1.2, but may require the \c SDL_AUDIODRIVER environment variable
                to be set appropriately for your system.
        \row
            \li null
            \li Produces no audio output but maintains video playback speed.
        \row
            \li pcm
            \li Raw PCM/WAVE file writer audio output
        \row
            \li rsound
            \li Audio output to an RSound daemon.
        \row
            \li sndio
            \li Audio output to the OpenBSD sndio sound system
        \row
            \li wasapi
            \li Audio output to the Windows Audio Session API.
        \endtable
    */
    property alias ao: mpvObject.ao

    /*!
        \qmlproperty string MpvPlayer::screenshotFormat

        Set the image file type used for saving screenshots.

        Available choices:

        \table
        \header
            \li Name
            \li Description
        \row
            \li png
            \li PNG
        \row
            \li jpg
            \li JPEG (default)
        \row
            \li jpeg
            \li JPEG (alias for jpg)
        \endtable
    */
    property alias screenshotFormat: mpvObject.screenshotFormat

    /*!
        \qmlproperty int MpvPlayer::screenshotPngCompression

        Set the PNG compression level. Higher means better compression. This
        will affect the file size of the written screenshot file and the time
        it takes to write a screenshot. Too high compression might occupy enough
        CPU time to interrupt playback. The default is 7.
    */
    property alias screenshotPngCompression: mpvObject.screenshotPngCompression

    /*!
        \qmlproperty string MpvPlayer::screenshotTemplate

        Specify the filename template used to save screenshots. The template
        specifies the filename without file extension, and can contain format
        specifiers, which will be substituted when taking a screenshot. By
        default, the template is \c mpv-shot%n, which results in filenames like
        \c mpv-shot0012.png for example.

        The template can start with a relative or absolute path, in order to
        specify a directory location where screenshots should be saved.

        If the final screenshot filename points to an already existing file, the
        file will not be overwritten. The screenshot will either not be saved, or
        if the template contains \c %n, saved using different, newly generated filename.
    */
    property alias screenshotTemplate: mpvObject.screenshotTemplate

    /*!
        \qmlproperty string MpvPlayer::screenshotDirectory

        Store screenshots in this directory. This path is joined with the filename
        generated by \c --screenshot-template. If the template filename is already
        absolute, the directory is ignored.

        If the directory does not exist, it is created on the first screenshot.
        If it is not a directory, an error is generated when trying to write a screenshot.
    */
    property alias screenshotDirectory: mpvObject.screenshotDirectory

    /*!
        \qmlproperty string MpvPlayer::profile

        To ease working with different configurations, profiles can be defined in
        the configuration files.
    */
    property alias profile: mpvObject.profile

    /*!
        \qmlproperty bool MpvPlayer::hrSeek

        Select when to use precise seeks that are not limited to keyframes. Such
        seeks require decoding video from the previous keyframe up to the target
        position and so can take some time depending on decoding performance. For
        some video formats, precise seeks are disabled. This option selects the
        default choice to use for seeks; it is possible to explicitly override that
        default in the definition of key bindings and in input commands.

        \table
        \header
            \li Value
            \li Description
        \row
            \li no
            \li Never use precise seeks.
        \row
            \li absolute
            \li Use precise seeks if the seek is to an absolute position in the file,
                such as a chapter seek, but not for relative seeks like the default
                behavior of arrow keys (default).
        \row
            \li yes
            \li Use precise seeks whenever possible.
        \row
            \li always
            \li Same as \c yes (for compatibility).
        \endtable

        Although this is a \c string in libmpv, we simply use \c bool instead.
    */
    property alias hrSeek: mpvObject.hrSeek

    /*!
        \qmlproperty bool MpvPlayer::ytdl

        Enable the youtube-dl hook-script. It will look at the input URL, and will
        play the video located on the website. This works with many streaming sites,
        not just the one that the script is named after. This requires a recent
        version of youtube-dl to be installed on the system. (Enabled by default.)

        If the script can't do anything with an URL, it will do nothing.
    */
    property alias ytdl: mpvObject.ytdl

    /*!
        \qmlproperty bool MpvPlayer::loadScripts

        If set to \c no, don't auto-load scripts from the \c scripts configuration
        subdirectory (usually \c {~/.config/mpv/scripts/}). (Default: \c yes)
    */
    property alias loadScripts: mpvObject.loadScripts

    /*!
        \qmlproperty string MpvPlayer::path

        Full path of the currently played file. Usually this is exactly the same
        string you pass on the mpv command line or the \c loadfile command, even if
        it's a relative path. If you expect an absolute path, you will have to
        determine it yourself, for example by using the \c working-directory property.
    */
    property alias path: mpvObject.path

    /*!
        \qmlproperty string MpvPlayer::fileFormat

        Symbolic name of the file format. In some cases, this is a comma-separated
        list of format names, e.g. mp4 is \c mov,mp4,m4a,3gp,3g2,mj2 (the list may grow
        in the future for any format).
    */
    property alias fileFormat: mpvObject.fileFormat

    /*!
        \qmlproperty qlonglong MpvPlayer::fileSize

        Length in bytes of the source file/stream. (This is the same as \c ${stream-end}.
        For segmented/multi-part files, this will return the size of the main or
        manifest file, whatever it is.)
    */
    property alias fileSize: mpvObject.fileSize

    /*!
        \qmlproperty double MpvPlayer::videoBitrate

        Bitrate values calculated on the packet level. This works by dividing the
        bit size of all packets between two keyframes by their presentation timestamp
        distance. (This uses the timestamps are stored in the file, so e.g. playback
        speed does not influence the returned values.) In particular, the video bitrate
        will update only per keyframe, and show the "past" bitrate. To make the property
        more UI friendly, updates to these properties are throttled in a certain way.

        The unit is bits per second. OSD formatting turns these values in kilobits (or
        megabits, if appropriate), which can be prevented by using the raw property
        value, e.g. with \c ${=video-bitrate}.

        Note that the accuracy of these properties is influenced by a few factors.
        If the underlying demuxer rewrites the packets on demuxing (done for some file
        formats), the bitrate might be slightly off. If timestamps are bad or jittery
        (like in Matroska), even constant bitrate streams might show fluctuating bitrate.

        \sa audioBitrate
    */
    property alias videoBitrate: mpvObject.videoBitrate

    /*!
        \qmlproperty double MpvPlayer::audioBitrate

        \sa videoBitrate
    */
    property alias audioBitrate: mpvObject.audioBitrate

    /*!
        \qmlproperty MpvDeclarativeObject::AudioDevices MpvPlayer::audioDeviceList

        Return the list of discovered audio devices.
    */
    property alias audioDeviceList: mpvObject.audioDeviceList

    /*!
        \qmlproperty bool MpvPlayer::screenshotTagColorspace

        Tag screenshots with the appropriate colorspace.

        Note that not all formats are supported.

        Default: \c false.
    */
    property alias screenshotTagColorspace: mpvObject.screenshotTagColorspace

    /*!
        \qmlproperty int MpvPlayer::screenshotJpegQuality

        Set the JPEG quality level. Higher means better quality.

        It should be an integer between \c 0 and \c 100.

        The default is \c 90.
    */
    property alias screenshotJpegQuality: mpvObject.screenshotJpegQuality

    /*!
        \qmlproperty string MpvPlayer::videoFormat

        Video format as string.
    */
    property alias videoFormat: mpvObject.videoFormat

    /*!
        \qmlproperty enumeration MpvPlayer::mpvCallType

        Set the mpv call type, should be one of \c MpvDeclarativeObject::Synchronous
        and \c MpvDeclarativeObject::Asynchronous.

        The default is \c MpvDeclarativeObject::Synchronous.
    */
    property alias mpvCallType: mpvObject.mpvCallType

    /*!
        \qmlproperty MpvDeclarativeObject::MediaTracks MpvPlayer::mediaTracks

        List of video/audio/subtitle tracks.
    */
    property alias mediaTracks: mpvObject.mediaTracks

    /*!
        \qmlproperty QStringList MpvPlayer::videoSuffixes

        Return the video file extension names supported by libmpv.
    */
    property alias videoSuffixes: mpvObject.videoSuffixes

    /*!
        \qmlproperty QStringList MpvPlayer::audioSuffixes

        Return the audio file extension names supported by libmpv.
    */
    property alias audioSuffixes: mpvObject.audioSuffixes

    /*!
        \qmlproperty QStringList MpvPlayer::subtitleSuffixes

        Return the subtitle file extension names supported by libmpv.
    */
    property alias subtitleSuffixes: mpvObject.subtitleSuffixes

    /*!
        \qmlproperty MpvDeclarativeObject::Chapters MpvPlayer::chapters

        List of chapters.
    */
    property alias chapters: mpvObject.chapters

    /*!
        \qmlproperty MpvDeclarativeObject::Metadata MpvPlayer::metadata

        Metadata.
    */
    property alias metadata: mpvObject.metadata

    /*!
        \qmlproperty double MpvPlayer::avsync

        Last A/V synchronization difference. Unavailable if audio or video is disabled.
    */
    property alias avsync: mpvObject.avsync

    /*!
        \qmlproperty int MpvPlayer::percentPos

        Position in current file (0-100). The advantage over using this instead of
        calculating it out of other properties is that it properly falls back to
        estimating the playback position from the byte position, if the file duration
        is not known.
    */
    property alias percentPos: mpvObject.percentPos

    /*!
        \qmlproperty double MpvPlayer::estimatedVfFps

        Estimated/measured FPS of the video filter chain output. (If no filters are
        used, this corresponds to decoder output.) This uses the average of the 10
        past frame durations to calculate the FPS. It will be inaccurate if frame-dropping
        is involved (such as when framedrop is explicitly enabled, or after precise seeking).
        Files with imprecise timestamps (such as Matroska) might lead to unstable results.
    */
    property alias estimatedVfFps: mpvObject.estimatedVfFps

    /*!
        \qmlsignal MpvPlayer::initFinished()

        This signal is emitted when the renderer finished initialization.

        The corresponding handler is \c onInitFinished.
    */
    signal initFinished

    /*!
        \qmlsignal MpvPlayer::loaded()

        This signal is emitted when the media is loaded.

        The corresponding handler is \c onLoaded.
    */
    signal loaded

    /*!
        \qmlsignal MpvPlayer::playing()

        This signal is emitted when the playback starts / resumes.

        The corresponding handler is \c onPlaying.
    */
    signal playing

    /*!
        \qmlsignal MpvPlayer::paused()

        This signal is emitted when the playback is paused.

        The corresponding handler is \c onPaused.
    */
    signal paused

    /*!
        \qmlsignal MpvPlayer::stopped()

        This signal is emitted when the playback is stopped.

        The corresponding handler is \c onStopped.
    */
    signal stopped

    /*!
        \qmlmethod MpvPlayer::open(url)

        Load the given \a url and start the playback immediately.
    */
    function open(url) {
        mpvObject.open(url);
    }

    /*!
        \qmlmethod MpvPlayer::play()

        Starts playback of the media.
    */
    function play() {
        mpvObject.play();
    }

    /*!
        \qmlmethod MpvPlayer::pause()

        Pauses playback of the media.
    */
    function pause() {
        mpvObject.pause();
    }

    /*!
        \qmlmethod MpvPlayer::stop()

        Stops playback of the media.
    */
    function stop() {
        mpvObject.stop();
    }

    /*!
        \qmlmethod MpvPlayer::seek(offset)

        If the \l seekable property is true, seeks the current
        playback position to \a offset.

        Seeking may be asynchronous, so the \l position property
        may not be updated immediately.

        The unit of it is \b SECONDS.

        \sa seekable, position
    */
    function seek(offset) {
        mpvObject.seek(offset);
    }

    /*!
        \qmlmethod MpvPlayer::seekAbsolute(position)

        Seek to an absolute \a position.

        The unit of it is \b SECONDS.
    */
    function seekAbsolute(position) {
        mpvObject.seekAbsolute(position);
    }

    /*!
        \qmlmethod MpvPlayer::seekRelative(offset)

        Seek to the \a offset relative to the current position.

        This method is an alias of \l seek()

        \sa seek()
    */
    function seekRelative(offset) {
        mpvObject.seekRelative(offset);
    }

    /*!
        \qmlmethod MpvPlayer::seekPercent(percent)

        Seek to the given percent. The parameter \a percent should be
        an integer between \c 0 and \c 100.
    */
    function seekPercent(percent) {
        mpvObject.seekPercent(percent);
    }

    /*!
        \qmlmethod MpvPlayer::screenshot()

        Take a screenshot. The image file will be saved to the executable's
        directory if the \l screenshotDirectory is not specified.

        \sa screenshotDirectory, screenshotFormat, screenshotTemplate
    */
    function screenshot() {
        mpvObject.screenshot();
    }

    /*!
        \qmlmethod MpvPlayer::screenshotToFile(path)

        Take a screenshot and save the image file to the given \a path.
    */
    function screenshotToFile(path) {
        mpvObject.screenshotToFile(path);
    }

    /*!
        \qmlmethod MpvPlayer::loadConfigFile(path)

        Load options from an external config file from \a path.
    */
    function loadConfigFile(path) {
        mpvObject.loadConfigFile(path);
    }

    /*!
        \qmlmethod MpvPlayer::isPlaying()

        Returns \c true if the playback is playing, otherwise returns \c false.
    */
    function isPlaying() {
        return mpvObject.playbackState === MpvObject.Playing;
    }

    /*!
        \qmlmethod MpvPlayer::isPaused()

        Returns \c true if the playback is paused, otherwise returns \c false.
    */
    function isPaused() {
        return mpvObject.playbackState === MpvObject.Paused;
    }

    /*!
        \qmlmethod MpvPlayer::isStopped()

        Returns \c true if the playback is stopped, otherwise returns \c false.
    */
    function isStopped() {
        return mpvObject.playbackState === MpvObject.Stopped;
    }

    MpvObject {
        id: mpvObject
        anchors.fill: mpvPlayer
        onInitFinished: mpvPlayer.initFinished()
        onLoaded: mpvPlayer.loaded()
        onPlaying: mpvPlayer.playing()
        onPaused: mpvPlayer.paused()
        onStopped: mpvPlayer.stopped()
    }
}
