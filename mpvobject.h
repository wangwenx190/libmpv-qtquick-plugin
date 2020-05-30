/*
 * MIT License
 *
 * Copyright (C) 2020 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

// Don't use any deprecated APIs from MPV.
#ifdef MPV_ENABLE_DEPRECATED
#undef MPV_ENABLE_DEPRECATED
#endif

#define MPV_ENABLE_DEPRECATED 0

#include <QHash>
#include <QQuickFramebufferObject>
#include <QUrl>

class MpvRenderer;

class MpvObject : public QQuickFramebufferObject {
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY_MOVE(MpvObject)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QSize videoSize READ videoSize NOTIFY videoSizeChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(
        qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    Q_PROPERTY(MpvObject::PlaybackState playbackState READ playbackState WRITE
                   setPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(MpvObject::MediaStatus mediaStatus READ mediaStatus NOTIFY
                   mediaStatusChanged)
    Q_PROPERTY(MpvObject::LogLevel logLevel READ logLevel WRITE setLogLevel
                   NOTIFY logLevelChanged)
    Q_PROPERTY(QString hwdec READ hwdec WRITE setHwdec NOTIFY hwdecChanged)
    Q_PROPERTY(QString mpvVersion READ mpvVersion CONSTANT)
    Q_PROPERTY(QString mpvConfiguration READ mpvConfiguration CONSTANT)
    Q_PROPERTY(QString ffmpegVersion READ ffmpegVersion CONSTANT)
    Q_PROPERTY(int vid READ vid WRITE setVid NOTIFY vidChanged)
    Q_PROPERTY(int aid READ aid WRITE setAid NOTIFY aidChanged)
    Q_PROPERTY(int sid READ sid WRITE setSid NOTIFY sidChanged)
    Q_PROPERTY(int videoRotate READ videoRotate WRITE setVideoRotate NOTIFY
                   videoRotateChanged)
    Q_PROPERTY(qreal videoAspect READ videoAspect WRITE setVideoAspect NOTIFY
                   videoAspectChanged)
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(bool deinterlace READ deinterlace WRITE setDeinterlace NOTIFY
                   deinterlaceChanged)
    Q_PROPERTY(bool audioExclusive READ audioExclusive WRITE setAudioExclusive
                   NOTIFY audioExclusiveChanged)
    Q_PROPERTY(QString audioFileAuto READ audioFileAuto WRITE setAudioFileAuto
                   NOTIFY audioFileAutoChanged)
    Q_PROPERTY(
        QString subAuto READ subAuto WRITE setSubAuto NOTIFY subAutoChanged)
    Q_PROPERTY(QString subCodepage READ subCodepage WRITE setSubCodepage NOTIFY
                   subCodepageChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY(QString vo READ vo WRITE setVo NOTIFY voChanged)
    Q_PROPERTY(QString ao READ ao WRITE setAo NOTIFY aoChanged)
    Q_PROPERTY(QString screenshotFormat READ screenshotFormat WRITE
                   setScreenshotFormat NOTIFY screenshotFormatChanged)
    Q_PROPERTY(
        int screenshotPngCompression READ screenshotPngCompression WRITE
            setScreenshotPngCompression NOTIFY screenshotPngCompressionChanged)
    Q_PROPERTY(QString screenshotTemplate READ screenshotTemplate WRITE
                   setScreenshotTemplate NOTIFY screenshotTemplateChanged)
    Q_PROPERTY(QString screenshotDirectory READ screenshotDirectory WRITE
                   setScreenshotDirectory NOTIFY screenshotDirectoryChanged)
    Q_PROPERTY(
        QString profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(bool hrSeek READ hrSeek WRITE setHrSeek NOTIFY hrSeekChanged)
    Q_PROPERTY(bool ytdl READ ytdl WRITE setYtdl NOTIFY ytdlChanged)
    Q_PROPERTY(bool loadScripts READ loadScripts WRITE setLoadScripts NOTIFY
                   loadScriptsChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QString fileFormat READ fileFormat NOTIFY fileFormatChanged)
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(qreal videoBitrate READ videoBitrate NOTIFY videoBitrateChanged)
    Q_PROPERTY(qreal audioBitrate READ audioBitrate NOTIFY audioBitrateChanged)
    Q_PROPERTY(MpvObject::AudioDevices audioDeviceList READ audioDeviceList
                   NOTIFY audioDeviceListChanged)
    Q_PROPERTY(
        bool screenshotTagColorspace READ screenshotTagColorspace WRITE
            setScreenshotTagColorspace NOTIFY screenshotTagColorspaceChanged)
    Q_PROPERTY(int screenshotJpegQuality READ screenshotJpegQuality WRITE
                   setScreenshotJpegQuality NOTIFY screenshotJpegQualityChanged)
    Q_PROPERTY(QString videoFormat READ videoFormat NOTIFY videoFormatChanged)
    Q_PROPERTY(MpvObject::MpvCallType mpvCallType READ mpvCallType WRITE
                   setMpvCallType NOTIFY mpvCallTypeChanged)
    Q_PROPERTY(MpvObject::MediaTracks mediaTracks READ mediaTracks NOTIFY
                   mediaTracksChanged)
    Q_PROPERTY(QStringList videoSuffixes READ videoSuffixes CONSTANT)
    Q_PROPERTY(QStringList audioSuffixes READ audioSuffixes CONSTANT)
    Q_PROPERTY(QStringList subtitleSuffixes READ subtitleSuffixes CONSTANT)
    Q_PROPERTY(
        MpvObject::Chapters chapters READ chapters NOTIFY chaptersChanged)
    Q_PROPERTY(
        MpvObject::Metadata metadata READ metadata NOTIFY metadataChanged)
    Q_PROPERTY(qreal avsync READ avsync NOTIFY avsyncChanged)
    Q_PROPERTY(int percentPos READ percentPos WRITE setPercentPos NOTIFY
                   percentPosChanged)
    Q_PROPERTY(
        qreal estimatedVfFps READ estimatedVfFps NOTIFY estimatedVfFpsChanged)

    friend class MpvRenderer;

    using SingleTrackInfo = QHash<QString, QVariant>;

public:
    enum class PlaybackState { Stopped, Playing, Paused };
    Q_ENUM(PlaybackState)

    enum class MediaStatus {
        Unknown,
        NoMedia,
        Loading,
        Loaded,
        Stalled,
        Buffering,
        Buffered,
        End,
        Invalid
    };
    Q_ENUM(MediaStatus)

    enum class LogLevel { Off, Debug, Warning, Critical, Fatal, Info };
    Q_ENUM(LogLevel)

    enum class MpvCallType { Synchronous, Asynchronous };
    Q_ENUM(MpvCallType)

    struct MediaTracks {
        QVector<SingleTrackInfo> videoChannels;
        QVector<SingleTrackInfo> audioTracks;
        QVector<SingleTrackInfo> subtitleStreams;
    };

    using Chapters = QVector<SingleTrackInfo>;

    using AudioDevices = QVector<SingleTrackInfo>;

    using Metadata = SingleTrackInfo;

    explicit MpvObject(QQuickItem *parent = nullptr);
    ~MpvObject() override;

    static void on_update(void *ctx);
    Renderer *createRenderer() const override;

    // Current media's source in QUrl.
    QUrl source() const;
    // Currently played file, with path stripped. If this is an URL, try to undo
    // percent encoding as well. (The result is not necessarily correct, but
    // looks better for display purposes. Use the path property to get an
    // unmodified filename)
    QString fileName() const;
    // Video's picture size
    // video-out-params/dw = video-params/dw = dwidth
    // video-out-params/dh = video-params/dh = dheight
    QSize videoSize() const;
    // Playback state
    MpvObject::PlaybackState playbackState() const;
    // Media status
    MpvObject::MediaStatus mediaStatus() const;
    // Control verbosity directly for each module. The all module changes the
    // verbosity of all the modules. The verbosity changes from this option are
    // applied in order from left to right, and each item can override a
    // previous one.
    // --msg-level=<module1=level1,module2=level2,...>
    // Available levels: no, fatal, error, warn, info, status (default), v
    // (verbose messages), debug, trace (print all messages produced by mpv)
    MpvObject::LogLevel logLevel() const;
    // Duration of the current file in **SECONDS**, not milliseconds.
    qint64 duration() const;
    // Position in current file in **SECONDS**, not milliseconds.
    qint64 position() const;
    // Set the startup volume: --volume=<0-100>
    int volume() const;
    // Set startup audio mute status (default: no): --mute=<yes|no>
    bool mute() const;
    // Return whether it's generally possible to seek in the current file.
    bool seekable() const;
    // If the currently played file has a title tag, use that.
    // Otherwise, if the media type is DVD, return the volume ID of DVD.
    // Otherwise, return the filename property.
    QString mediaTitle() const;
    // Hardware video decoding API: --hwdec=<api>
    // <api> can be: no, auto, vdpau, vaapi, videotoolbox, dxva2, d3d11va,
    // mediacodec, mmal, cuda, nvdec, crystalhd, rkmpp
    QString hwdec() const;
    // The mpv version/copyright string
    QString mpvVersion() const;
    // The configuration arguments which were passed to the mpv build system
    QString mpvConfiguration() const;
    // The contents of the av_version_info() API call
    QString ffmpegVersion() const;
    // Video channel: --vid=<ID|auto|no>
    int vid() const;
    // Audio track: --aid=<ID|auto|no>
    int aid() const;
    // Subtitle stream: --sid=<ID|auto|no>
    int sid() const;
    // Rotate the video clockwise, in degrees: --video-rotate=<0-359|no>
    // Use "video-out-params/rotate" to query this variable
    // video-out-params/rotate = video-params/rotate = video-rotate
    int videoRotate() const;
    // Video aspect ratio: --video-aspect-override=<ratio|no>
    // Eg: --video-aspect-override=4:3 or --video-aspect-override=1.3333
    // Eg: --video-aspect-override=16:9 or --video-aspect-override=1.7777
    // Use "video-out-params/aspect" to query this variable.
    // video-out-params/aspect = video-params/aspect = video-aspect-override
    qreal videoAspect() const;
    // Playback speed: --speed=<0.01-100>
    qreal speed() const;
    // Enable or disable interlacing (default: no): --deinterlace=<yes|no>
    bool deinterlace() const;
    // Enable exclusive output mode. In this mode, the system is usually locked
    // out, and only mpv will be able to output audio.
    // --audio-exclusive=<yes|no>
    bool audioExclusive() const;
    // Load additional audio files matching the video filename. The parameter
    // specifies how external audio files are matched.
    // --audio-file-auto=<no|exact|fuzzy|all>
    // no: Don't automatically load external audio files (default)
    // exact: Load the media filename with audio file extension
    // fuzzy: Load all audio files containing media filename
    // all: Load all audio files in the current and --audio-file-paths
    // directories
    QString audioFileAuto() const;
    // Load additional subtitle files matching the video filename. The parameter
    // specifies how external subtitle files are matched. exact is enabled by
    // default.
    // --sub-auto=<no|exact|fuzzy|all>
    // no, exact, fuzzy, all: same as --audio-file-auto.
    QString subAuto() const;
    // Subtitle codepage (default: auto): --sub-codepage=<codepage>
    // Eg: --sub-codepage=latin2
    // Eg: --sub-codepage=+cp1250
    QString subCodepage() const;
    // Specify a priority list of video output drivers to be used.
    // --vo=<driver1,driver2,...[,]>
    // drivers: xv, x11, vdpau, direct3d, gpu, sdl, vaapi, null, caca, tct,
    // image, libmpv, drm, mediacodec_embed
    QString vo() const;
    // Specify a priority list of audio output drivers to be used.
    // --ao=<driver1,driver2,...[,]>
    // drivers: alsa, oss, jack, coreaudio, coreaudio_exclusive, openal, pulse,
    // sdl, null, pcm, rsound, sndio, wasapi
    QString ao() const;
    // Set the image file type used for saving screenshots.
    // --screenshot-format=<png|jpg>
    QString screenshotFormat() const;
    // Tag screenshots with the appropriate colorspace.
    // --screenshot-tag-colorspace=<yes|no>
    bool screenshotTagColorspace() const;
    // --screenshot-png-compression=<0-9>
    // Set the PNG compression level. Higher means better compression. This will
    // affect the file size of the written screenshot file and the time it takes
    // to write a screenshot. Too high compression might occupy enough CPU time
    // to interrupt playback. The default is 7.
    int screenshotPngCompression() const;
    // --screenshot-jpeg-quality=<0-100>
    // Set the JPEG quality level. Higher means better quality. The default
    // is 90.
    int screenshotJpegQuality() const;
    // --screenshot-template=<template>
    // %F: Filename of the currently played video but strip the file extension,
    // including the dot.
    // %x: Directory path of the currently played video (empty if not on local
    // file system, eg: http://).
    // %p: Current playback time, in the same format as used in the OSD. The
    // result is a string of the form "HH:MM:SS".
    QString screenshotTemplate() const;
    // --screenshot-directory=<path>
    // Store screenshots in this directory. If the template filename is already
    // absolute, the directory is ignored. If the directory does not exist, it
    // is created on the first screenshot. If it is not a directory, an error is
    // generated when trying to write a screenshot.
    QString screenshotDirectory() const;
    // Preset configurations
    // You can apply profiles on start with the --profile=<name> option, or at
    // runtime with the apply-profile <name> command.
    QString profile() const;
    // --hr-seek=<no|absolute|yes>
    // Select when to use precise seeks that are not limited to keyframes
    // no: Never use precise seeks;
    // yes: Use precise seeks whenever possible;
    // absolute: Use precise seeks if the seek is to an absolute position in the
    // file, such as a chapter seek, but not for relative seeks like the default
    // behavior of arrow keys (default).
    bool hrSeek() const;
    // --ytdl=<yes|no>
    // Enable the youtube-dl hook-script. It will look at the input URL, and
    // will play the video located on the website
    bool ytdl() const;
    // --load-scripts=<yes|no>
    // Auto-load scripts from the scripts configuration subdirectory (usually
    // ~/.config/mpv/scripts/)
    bool loadScripts() const;
    // Full path of the currently played file. Usually this is exactly the same
    // string you pass on the mpv command line or the loadfile command, even if
    // it's a relative path. If you expect an absolute path, you will have to
    // determine it yourself
    QString path() const;
    // Symbolic name of the file format. In some cases, this is a
    // comma-separated list of format names, e.g. mp4 is mov,mp4,m4a,3gp,3g2,mj2
    // (the list may grow in the future for any format).
    QString fileFormat() const;
    // Length in bytes of the source file/stream.
    qint64 fileSize() const;
    // Video bitrate
    qreal videoBitrate() const;
    // Audio bitrate
    qreal audioBitrate() const;
    // Return the list of discovered audio devices.
    MpvObject::AudioDevices audioDeviceList() const;
    // Video format as string.
    QString videoFormat() const;
    // The call type of mpv client APIs.
    MpvObject::MpvCallType mpvCallType() const;
    // Video, audio and subtitle tracks.
    MpvObject::MediaTracks mediaTracks() const;
    // File types supported by mpv:
    // https://github.com/mpv-player/mpv/blob/master/player/external_files.c
    QStringList videoSuffixes() const {
        return QStringList{
            QString::fromUtf8("*.3g2"),   QString::fromUtf8("*.3ga"),
            QString::fromUtf8("*.3gp"),   QString::fromUtf8("*.3gp2"),
            QString::fromUtf8("*.3gpp"),  QString::fromUtf8("*.amv"),
            QString::fromUtf8("*.asf"),   QString::fromUtf8("*.asx"),
            QString::fromUtf8("*.avf"),   QString::fromUtf8("*.avi"),
            QString::fromUtf8("*.bdm"),   QString::fromUtf8("*.bdmv"),
            QString::fromUtf8("*.bik"),   QString::fromUtf8("*.clpi"),
            QString::fromUtf8("*.cpi"),   QString::fromUtf8("*.dat"),
            QString::fromUtf8("*.divx"),  QString::fromUtf8("*.drc"),
            QString::fromUtf8("*.dv"),    QString::fromUtf8("*.dvr-ms"),
            QString::fromUtf8("*.f4v"),   QString::fromUtf8("*.flv"),
            QString::fromUtf8("*.gvi"),   QString::fromUtf8("*.gxf"),
            QString::fromUtf8("*.hdmov"), QString::fromUtf8("*.hlv"),
            QString::fromUtf8("*.iso"),   QString::fromUtf8("*.letv"),
            QString::fromUtf8("*.lrv"),   QString::fromUtf8("*.m1v"),
            QString::fromUtf8("*.m2p"),   QString::fromUtf8("*.m2t"),
            QString::fromUtf8("*.m2ts"),  QString::fromUtf8("*.m2v"),
            QString::fromUtf8("*.m3u"),   QString::fromUtf8("*.m3u8"),
            QString::fromUtf8("*.m4v"),   QString::fromUtf8("*.mkv"),
            QString::fromUtf8("*.moov"),  QString::fromUtf8("*.mov"),
            QString::fromUtf8("*.mp2"),   QString::fromUtf8("*.mp2v"),
            QString::fromUtf8("*.mp4"),   QString::fromUtf8("*.mp4v"),
            QString::fromUtf8("*.mpe"),   QString::fromUtf8("*.mpeg"),
            QString::fromUtf8("*.mpeg1"), QString::fromUtf8("*.mpeg2"),
            QString::fromUtf8("*.mpeg4"), QString::fromUtf8("*.mpg"),
            QString::fromUtf8("*.mpl"),   QString::fromUtf8("*.mpls"),
            QString::fromUtf8("*.mpv"),   QString::fromUtf8("*.mpv2"),
            QString::fromUtf8("*.mqv"),   QString::fromUtf8("*.mts"),
            QString::fromUtf8("*.mtv"),   QString::fromUtf8("*.mxf"),
            QString::fromUtf8("*.mxg"),   QString::fromUtf8("*.nsv"),
            QString::fromUtf8("*.nuv"),   QString::fromUtf8("*.ogm"),
            QString::fromUtf8("*.ogv"),   QString::fromUtf8("*.ogx"),
            QString::fromUtf8("*.ps"),    QString::fromUtf8("*.qt"),
            QString::fromUtf8("*.qtvr"),  QString::fromUtf8("*.ram"),
            QString::fromUtf8("*.rec"),   QString::fromUtf8("*.rm"),
            QString::fromUtf8("*.rmj"),   QString::fromUtf8("*.rmm"),
            QString::fromUtf8("*.rms"),   QString::fromUtf8("*.rmvb"),
            QString::fromUtf8("*.rmx"),   QString::fromUtf8("*.rp"),
            QString::fromUtf8("*.rpl"),   QString::fromUtf8("*.rv"),
            QString::fromUtf8("*.rvx"),   QString::fromUtf8("*.thp"),
            QString::fromUtf8("*.tod"),   QString::fromUtf8("*.tp"),
            QString::fromUtf8("*.trp"),   QString::fromUtf8("*.ts"),
            QString::fromUtf8("*.tts"),   QString::fromUtf8("*.txd"),
            QString::fromUtf8("*.vcd"),   QString::fromUtf8("*.vdr"),
            QString::fromUtf8("*.vob"),   QString::fromUtf8("*.vp8"),
            QString::fromUtf8("*.vro"),   QString::fromUtf8("*.webm"),
            QString::fromUtf8("*.wm"),    QString::fromUtf8("*.wmv"),
            QString::fromUtf8("*.wtv"),   QString::fromUtf8("*.xesc"),
            QString::fromUtf8("*.xspf")};
    }
    QStringList audioSuffixes() const {
        return QStringList{
            QString::fromUtf8("*.mp3"),  QString::fromUtf8("*.aac"),
            QString::fromUtf8("*.mka"),  QString::fromUtf8("*.dts"),
            QString::fromUtf8("*.flac"), QString::fromUtf8("*.ogg"),
            QString::fromUtf8("*.m4a"),  QString::fromUtf8("*.ac3"),
            QString::fromUtf8("*.opus"), QString::fromUtf8("*.wav"),
            QString::fromUtf8("*.wv")};
    }
    QStringList subtitleSuffixes() const {
        return QStringList{
            QString::fromUtf8("*.utf"),   QString::fromUtf8("*.utf8"),
            QString::fromUtf8("*.utf-8"), QString::fromUtf8("*.idx"),
            QString::fromUtf8("*.sub"),   QString::fromUtf8("*.srt"),
            QString::fromUtf8("*.rt"),    QString::fromUtf8("*.ssa"),
            QString::fromUtf8("*.ass"),   QString::fromUtf8("*.mks"),
            QString::fromUtf8("*.vtt"),   QString::fromUtf8("*.sup"),
            QString::fromUtf8("*.scc"),   QString::fromUtf8("*.smi")};
    }
    // Chapter list
    MpvObject::Chapters chapters() const;
    // Metadata map
    MpvObject::Metadata metadata() const;
    // Last A/V synchronization difference. Unavailable if audio or video is
    // disabled.
    qreal avsync() const;
    // Position in current file (0-100). The advantage over using this instead
    // of calculating it out of other properties is that it properly falls back
    // to estimating the playback position from the byte position, if the file
    // duration is not known.
    int percentPos() const;
    // Estimated/measured FPS of the video filter chain output. (If no filters
    // are used, this corresponds to decoder output.) This uses the average of
    // the 10 past frame durations to calculate the FPS. It will be inaccurate
    // if frame-dropping is involved (such as when framedrop is explicitly
    // enabled, or after precise seeking). Files with imprecise timestamps (such
    // as Matroska) might lead to unstable results.
    qreal estimatedVfFps() const;

    void setSource(const QUrl &source);
    void setMute(bool mute);
    void setPlaybackState(MpvObject::PlaybackState playbackState);
    void setLogLevel(MpvObject::LogLevel logLevel);
    void setPosition(qint64 position);
    void setVolume(int volume);
    void setHwdec(const QString &hwdec);
    void setVid(int vid);
    void setAid(int aid);
    void setSid(int sid);
    void setVideoRotate(int videoRotate);
    void setVideoAspect(qreal videoAspect);
    void setSpeed(qreal speed);
    void setDeinterlace(bool deinterlace);
    void setAudioExclusive(bool audioExclusive);
    void setAudioFileAuto(const QString &audioFileAuto);
    void setSubAuto(const QString &subAuto);
    void setSubCodepage(const QString &subCodepage);
    void setVo(const QString &vo);
    void setAo(const QString &ao);
    void setScreenshotFormat(const QString &screenshotFormat);
    void setScreenshotPngCompression(int screenshotPngCompression);
    void setScreenshotTemplate(const QString &screenshotTemplate);
    void setScreenshotDirectory(const QString &screenshotDirectory);
    void setProfile(const QString &profile);
    void setHrSeek(bool hrSeek);
    void setYtdl(bool ytdl);
    void setLoadScripts(bool loadScripts);
    void setScreenshotTagColorspace(bool screenshotTagColorspace);
    void setScreenshotJpegQuality(int screenshotJpegQuality);
    void setMpvCallType(MpvObject::MpvCallType mpvCallType);
    void setPercentPos(int percentPos);

public Q_SLOTS:
    bool open(const QUrl &url);
    bool play();
    bool play(const QUrl &url);
    bool pause();
    bool stop();
    bool seek(qint64 value, bool absolute = false, bool percent = false);
    // Jump to an absolute position, in seconds. libmpv supports negative
    // position, which means jump from the end of the file, but I will not
    // implement it in a short period of time because I think it's useless.
    bool seekAbsolute(qint64 position);
    // Jump to a relative position, in seconds. If the offset is negative, then
    // the player will jump back.
    bool seekRelative(qint64 offset);
    // Jump to an absolute percent position (0-100). Although libmpv supports
    // relative percent, I will not implement it in a short period of time
    // because I don't think it is useful enough.
    bool seekPercent(int percent);
    bool screenshot();
    // According to mpv's manual, the file path must contain an extension
    // name, otherwise the behavior is arbitrary.
    bool screenshotToFile(const QString &filePath);
    // Loads and parses the config file, and sets every entry in the config
    // file's default section as if mpv_set_option_string() is called.
    bool loadConfigFile(const QString &path);

protected Q_SLOTS:
    void handleMpvEvents();

private Q_SLOTS:
    void doUpdate();

private:
    bool mpvSendCommand(const QVariant &arguments);
    bool mpvSetProperty(const QString &name, const QVariant &value);
    QVariant mpvGetProperty(const QString &name, bool silent = false,
                            bool *ok = nullptr) const;
    bool mpvObserveProperty(const QString &name);

    void processMpvLogMessage(void *event);
    void processMpvPropertyChange(void *event);

    bool isLoaded() const;
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    void setMediaStatus(MpvObject::MediaStatus mediaStatus);

    // Should be called when MPV_EVENT_VIDEO_RECONFIG happens.
    // Never do anything expensive here.
    void videoReconfig();
    // Should be called when MPV_EVENT_AUDIO_RECONFIG happens.
    // Never do anything expensive here.
    void audioReconfig();

    void playbackStateChangeEvent();

private:
    QUrl currentSource = QUrl();
    MpvObject::MediaStatus currentMediaStatus = MpvObject::MediaStatus::NoMedia;
    MpvObject::MpvCallType currentMpvCallType =
        MpvObject::MpvCallType::Synchronous;

    const QHash<QString, QString> properties = {
        {QString::fromUtf8("dwidth"), QString::fromUtf8("videoSizeChanged")},
        {QString::fromUtf8("dheight"), QString::fromUtf8("videoSizeChanged")},
        {QString::fromUtf8("duration"), QString::fromUtf8("durationChanged")},
        {QString::fromUtf8("time-pos"), QString::fromUtf8("positionChanged")},
        {QString::fromUtf8("volume"), QString::fromUtf8("volumeChanged")},
        {QString::fromUtf8("mute"), QString::fromUtf8("muteChanged")},
        {QString::fromUtf8("seekable"), QString::fromUtf8("seekableChanged")},
        {QString::fromUtf8("hwdec"), QString::fromUtf8("hwdecChanged")},
        {QString::fromUtf8("vid"), QString::fromUtf8("vidChanged")},
        {QString::fromUtf8("aid"), QString::fromUtf8("aidChanged")},
        {QString::fromUtf8("sid"), QString::fromUtf8("sidChanged")},
        {QString::fromUtf8("video-rotate"),
         QString::fromUtf8("videoRotateChanged")},
        {QString::fromUtf8("video-out-params/aspect"),
         QString::fromUtf8("videoAspectChanged")},
        {QString::fromUtf8("speed"), QString::fromUtf8("speedChanged")},
        {QString::fromUtf8("deinterlace"),
         QString::fromUtf8("deinterlaceChanged")},
        {QString::fromUtf8("audio-exclusive"),
         QString::fromUtf8("audioExclusiveChanged")},
        {QString::fromUtf8("audio-file-auto"),
         QString::fromUtf8("audioFileAutoChanged")},
        {QString::fromUtf8("sub-auto"), QString::fromUtf8("subAutoChanged")},
        {QString::fromUtf8("sub-codepage"),
         QString::fromUtf8("subCodepageChanged")},
        {QString::fromUtf8("filename"), QString::fromUtf8("fileNameChanged")},
        {QString::fromUtf8("media-title"),
         QString::fromUtf8("mediaTitleChanged")},
        {QString::fromUtf8("vo"), QString::fromUtf8("voChanged")},
        {QString::fromUtf8("ao"), QString::fromUtf8("aoChanged")},
        {QString::fromUtf8("screenshot-format"),
         QString::fromUtf8("screenshotFormatChanged")},
        {QString::fromUtf8("screenshot-png-compression"),
         QString::fromUtf8("screenshotPngCompressionChanged")},
        {QString::fromUtf8("screenshot-template"),
         QString::fromUtf8("screenshotTemplateChanged")},
        {QString::fromUtf8("screenshot-directory"),
         QString::fromUtf8("screenshotDirectoryChanged")},
        {QString::fromUtf8("profile"), QString::fromUtf8("profileChanged")},
        {QString::fromUtf8("hr-seek"), QString::fromUtf8("hrSeekChanged")},
        {QString::fromUtf8("ytdl"), QString::fromUtf8("ytdlChanged")},
        {QString::fromUtf8("load-scripts"),
         QString::fromUtf8("loadScriptsChanged")},
        {QString::fromUtf8("path"), QString::fromUtf8("pathChanged")},
        {QString::fromUtf8("file-format"),
         QString::fromUtf8("fileFormatChanged")},
        {QString::fromUtf8("file-size"), QString::fromUtf8("fileSizeChanged")},
        {QString::fromUtf8("video-bitrate"),
         QString::fromUtf8("videoBitrateChanged")},
        {QString::fromUtf8("audio-bitrate"),
         QString::fromUtf8("audioBitrateChanged")},
        {QString::fromUtf8("audio-device-list"),
         QString::fromUtf8("audioDeviceListChanged")},
        {QString::fromUtf8("screenshot-tag-colorspace"),
         QString::fromUtf8("screenshotTagColorspaceChanged")},
        {QString::fromUtf8("screenshot-jpeg-quality"),
         QString::fromUtf8("screenshotJpegQualityChanged")},
        {QString::fromUtf8("video-format"),
         QString::fromUtf8("videoFormatChanged")},
        {QString::fromUtf8("pause"), QString::fromUtf8("playbackStateChanged")},
        {QString::fromUtf8("idle-active"),
         QString::fromUtf8("playbackStateChanged")},
        {QString::fromUtf8("track-list"),
         QString::fromUtf8("mediaTracksChanged")},
        {QString::fromUtf8("chapter-list"),
         QString::fromUtf8("chaptersChanged")},
        {QString::fromUtf8("metadata"), QString::fromUtf8("metadataChanged")},
        {QString::fromUtf8("avsync"), QString::fromUtf8("avsyncChanged")},
        {QString::fromUtf8("percent-pos"),
         QString::fromUtf8("percentPosChanged")},
        {QString::fromUtf8("estimated-vf-fps"),
         QString::fromUtf8("estimatedVfFpsChanged")}};

    // These properties are changing all the time during the playback process.
    // So we have to add them to the black list, otherwise we'll get huge
    // message floods.
    const QStringList propertyBlackList = {
        QString::fromUtf8("time-pos"),
        QString::fromUtf8("playback-time"),
        QString::fromUtf8("percent-pos"),
        QString::fromUtf8("video-bitrate"),
        QString::fromUtf8("audio-bitrate"),
        QString::fromUtf8("estimated-vf-fps"),
        QString::fromUtf8("avsync")};

Q_SIGNALS:
    void onUpdate();
    void hasMpvEvents();
    void initFinished();

    void loaded();
    void playing();
    void paused();
    void stopped();

    void sourceChanged();
    void videoSizeChanged();
    void playbackStateChanged();
    void mediaStatusChanged();
    void logLevelChanged();
    void durationChanged();
    void positionChanged();
    void volumeChanged();
    void muteChanged();
    void seekableChanged();
    void hwdecChanged();
    void vidChanged();
    void aidChanged();
    void sidChanged();
    void videoRotateChanged();
    void videoAspectChanged();
    void speedChanged();
    void deinterlaceChanged();
    void audioExclusiveChanged();
    void audioFileAutoChanged();
    void subAutoChanged();
    void subCodepageChanged();
    void fileNameChanged();
    void mediaTitleChanged();
    void voChanged();
    void aoChanged();
    void screenshotFormatChanged();
    void screenshotPngCompressionChanged();
    void screenshotTemplateChanged();
    void screenshotDirectoryChanged();
    void profileChanged();
    void hrSeekChanged();
    void ytdlChanged();
    void loadScriptsChanged();
    void pathChanged();
    void fileFormatChanged();
    void fileSizeChanged();
    void videoBitrateChanged();
    void audioBitrateChanged();
    void audioDeviceListChanged();
    void screenshotTagColorspaceChanged();
    void screenshotJpegQualityChanged();
    void videoFormatChanged();
    void mpvCallTypeChanged();
    void mediaTracksChanged();
    void chaptersChanged();
    void metadataChanged();
    void avsyncChanged();
    void percentPosChanged();
    void estimatedVfFpsChanged();
};

Q_DECLARE_METATYPE(MpvObject::MediaTracks)
