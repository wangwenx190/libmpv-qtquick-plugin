#include "mpvobject.h"

#include "mpvqthelper.hpp"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
#include <QGuiApplication>
#include <QX11Info>
#endif

namespace {

const char m_libmpvPathEnvVarName[] = "WWX190_LIBMPV_PATH";

mpv_handle *m_pMpv = nullptr;
mpv_render_context *m_pMpvGL = nullptr;

QString getLibmpvPath() {
    const QString defaultLibPath = QString::fromUtf8("mpv");
    return qEnvironmentVariable(m_libmpvPathEnvVarName, defaultLibPath);
}

void wakeup(void *ctx) {
    // This callback is invoked from any mpv thread (but possibly also
    // recursively from a thread that is calling the mpv API). Just notify
    // the Qt GUI thread to wake up (so that it can process events with
    // mpv_wait_event()), and return as quickly as possible.
    QMetaObject::invokeMethod(static_cast<MpvObject *>(ctx), "hasMpvEvents",
                              Qt::QueuedConnection);
}

void on_mpv_redraw(void *ctx) { MpvObject::on_update(ctx); }

void *get_proc_address_mpv(void *ctx, const char *name) {
    Q_UNUSED(ctx)
    const QOpenGLContext *const glctx = QOpenGLContext::currentContext();
    return glctx
        ? reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)))
        : nullptr;
}

} // namespace

class MpvRenderer : public QQuickFramebufferObject::Renderer {
    Q_DISABLE_COPY_MOVE(MpvRenderer)

public:
    MpvRenderer(MpvObject *mpvObject) : m_mpvObject(mpvObject) {
        Q_ASSERT(m_mpvObject);
    }
    ~MpvRenderer() override = default;

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject *
    createFramebufferObject(const QSize &size) override {
        // init mpv_gl:
        if (!m_pMpvGL) {
            mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr,
                                                  nullptr};
            mpv_render_param params[]{
                {MPV_RENDER_PARAM_API_TYPE,
                 const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                {MPV_RENDER_PARAM_INVALID, nullptr},
                {MPV_RENDER_PARAM_INVALID, nullptr}};
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
            if (QGuiApplication::platformName().contains("xcb",
                                                         Qt::CaseInsensitive)) {
                params[2].type = MPV_RENDER_PARAM_X11_DISPLAY;
                params[2].data = QX11Info::display();
            }
#endif

            const int mpvGLInitResult =
                mpv::qt::render_context_create(&m_pMpvGL, m_pMpv, params);
            Q_ASSERT_X(mpvGLInitResult >= 0, __FUNCTION__,
                       qUtf8Printable(mpv::qt::error_string(mpvGLInitResult)));
            mpv::qt::render_context_set_update_callback(m_pMpvGL, on_mpv_redraw,
                                                        m_mpvObject);

            QMetaObject::invokeMethod(m_mpvObject, "initFinished");
        }

        return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
    }

    void render() override {
        m_mpvObject->window()->resetOpenGLState();

        QOpenGLFramebufferObject *fbo = framebufferObject();
        mpv_opengl_fbo mpfbo;
        mpfbo.fbo = static_cast<int>(fbo->handle());
        mpfbo.w = fbo->width();
        mpfbo.h = fbo->height();
        mpfbo.internal_format = 0;
        int flip_y = 0;

        mpv_render_param params[] = {
            // Specify the default framebuffer (0) as target. This will
            // render onto the entire screen. If you want to show the video
            // in a smaller rectangle or apply fancy transformations, you'll
            // need to render into a separate FBO and draw it manually.
            {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
            // Flip rendering (needed due to flipped GL coordinate system).
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}};
        // See render_gl.h on what OpenGL environment mpv expects, and
        // other API details.
        mpv::qt::render_context_render(m_pMpvGL, params);

        m_mpvObject->window()->resetOpenGLState();
    }

private:
    MpvObject *m_mpvObject = nullptr;
};

MpvObject::MpvObject(QQuickItem *parent) : QQuickFramebufferObject(parent) {
    mpv::qt::libmpv_init(getLibmpvPath());

    m_pMpv = mpv::qt::create();
    Q_ASSERT(m_pMpv);

    mpvSetProperty(QString::fromUtf8("input-default-bindings"), false);
    mpvSetProperty(QString::fromUtf8("input-vo-keyboard"), false);
    mpvSetProperty(QString::fromUtf8("input-cursor"), false);
    mpvSetProperty(QString::fromUtf8("cursor-autohide"), false);

    auto iterator = properties.cbegin();
    while (iterator != properties.cend()) {
        mpvObserveProperty(iterator.key());
        ++iterator;
    }

    // From this point on, the wakeup function will be called. The callback
    // can come from any thread, so we use the QueuedConnection mechanism to
    // relay the wakeup in a thread-safe way.
    connect(this, &MpvObject::hasMpvEvents, this, &MpvObject::handleMpvEvents,
            Qt::QueuedConnection);
    mpv::qt::set_wakeup_callback(m_pMpv, wakeup, this);

    const int mpvInitResult = mpv::qt::initialize(m_pMpv);
    Q_ASSERT_X(mpvInitResult >= 0, __FUNCTION__,
               qUtf8Printable(mpv::qt::error_string(mpvInitResult)));

    connect(this, &MpvObject::onUpdate, this, &MpvObject::doUpdate,
            Qt::QueuedConnection);
}

MpvObject::~MpvObject() {
    // only initialized if something got drawn
    if (m_pMpvGL) {
        mpv::qt::render_context_free(m_pMpvGL);
    }
    mpv::qt::terminate_destroy(m_pMpv);
}

void MpvObject::on_update(void *ctx) {
    Q_EMIT static_cast<MpvObject *>(ctx)->onUpdate();
}

// connected to onUpdate() signal makes sure it runs on the GUI thread
void MpvObject::doUpdate() { update(); }

void MpvObject::processMpvLogMessage(void *event) {
    const auto e = static_cast<mpv_event_log_message *>(event);
    const char messagePrefix[] = "[libmpv] [LOG]";
    // The log message from libmpv contains new line. Remove it.
    const QString messageText = QString::fromUtf8(e->text).trimmed();
    const QString message = QString::fromUtf8("%1 %2").arg(
        QString::fromUtf8(messagePrefix), messageText);
    switch (e->log_level) {
    case MPV_LOG_LEVEL_V:
    case MPV_LOG_LEVEL_DEBUG:
    case MPV_LOG_LEVEL_TRACE:
        qDebug().noquote() << message;
        break;
    case MPV_LOG_LEVEL_WARN:
        qWarning().noquote() << message;
        break;
    case MPV_LOG_LEVEL_ERROR:
        qCritical().noquote() << message;
        break;
    case MPV_LOG_LEVEL_FATAL:
        // qFatal() doesn't support the "<<" operator.
        qFatal("%s %s", messagePrefix, e->text);
        break;
    case MPV_LOG_LEVEL_INFO:
        qInfo().noquote() << message;
        break;
    default:
        qDebug().noquote() << message;
        break;
    }
}

void MpvObject::processMpvPropertyChange(void *event) {
    const auto e = static_cast<mpv_event_property *>(event);
    const QString name = QString::fromUtf8(e->name);
    if (!propertyBlackList.contains(name)) {
        qDebug().noquote() << "[libmpv] [PROPERTY]" << name << "-->"
                           << mpvGetProperty(name, true);
    }
    if (properties.contains(name)) {
        const QString signalName = properties.value(name);
        if (!signalName.isEmpty()) {
            QMetaObject::invokeMethod(this, qUtf8Printable(signalName));
        }
    }
}

bool MpvObject::isLoaded() const {
    return ((mediaStatus() == MediaStatus::Loaded) ||
            (mediaStatus() == MediaStatus::Buffering) ||
            (mediaStatus() == MediaStatus::Buffered));
}

bool MpvObject::isPlaying() const {
    return playbackState() == PlaybackState::Playing;
}

bool MpvObject::isPaused() const {
    return playbackState() == PlaybackState::Paused;
}

bool MpvObject::isStopped() const {
    return playbackState() == PlaybackState::Stopped;
}

void MpvObject::setMediaStatus(MpvObject::MediaStatus mediaStatus) {
    if (this->mediaStatus() == mediaStatus) {
        return;
    }
    currentMediaStatus = mediaStatus;
    Q_EMIT mediaStatusChanged();
}

void MpvObject::videoReconfig() { Q_EMIT videoSizeChanged(); }

void MpvObject::audioReconfig() {}

void MpvObject::playbackStateChangeEvent() {
    if (isPlaying()) {
        Q_EMIT playing();
    }
    if (isPaused()) {
        Q_EMIT paused();
    }
    if (isStopped()) {
        Q_EMIT stopped();
    }
    Q_EMIT playbackStateChanged();
}

bool MpvObject::mpvSendCommand(const QVariant &arguments) {
    if (arguments.isNull() || !arguments.isValid()) {
        return false;
    }
    const char messagePrefix[] = "[libmpv] [COMMAND]";
    qDebug().noquote() << messagePrefix << arguments;
    int errorCode = 0;
    if (mpvCallType() == MpvCallType::Asynchronous) {
        errorCode = mpv::qt::command_async(m_pMpv, arguments, 0);
    } else {
        errorCode = mpv::qt::get_error(mpv::qt::command(m_pMpv, arguments));
    }
    if (errorCode < 0) {
        qWarning().noquote()
            << messagePrefix << "Failed to send command" << arguments << ':'
            << mpv::qt::error_string(errorCode);
    }
    return (errorCode >= 0);
}

bool MpvObject::mpvSetProperty(const QString &name, const QVariant &value) {
    if (name.isEmpty() || value.isNull() || !value.isValid()) {
        return false;
    }
    const char messagePrefix[] = "[libmpv] [PROPERTY]";
    qDebug().noquote() << messagePrefix << name << "-->" << value;
    int errorCode = 0;
    if (mpvCallType() == MpvCallType::Asynchronous) {
        errorCode = mpv::qt::set_property_async(m_pMpv, name, value, 0);
    } else {
        errorCode = mpv::qt::set_property(m_pMpv, name, value);
    }
    if (errorCode < 0) {
        qWarning().noquote()
            << messagePrefix << "Failed to change property" << name << "to"
            << value << ':' << mpv::qt::error_string(errorCode);
    }
    return (errorCode >= 0);
}

QVariant MpvObject::mpvGetProperty(const QString &name, bool silent,
                                   bool *ok) const {
    if (ok) {
        *ok = false;
    }
    if (name.isEmpty()) {
        return QVariant();
    }
    const char messagePrefix[] = "[libmpv] [PROPERTY]";
    const QVariant result = mpv::qt::get_property(m_pMpv, name);
    const int errorCode = mpv::qt::get_error(result);
    if (result.isNull() || !result.isValid() || (errorCode < 0)) {
        if (!silent) {
            qWarning().noquote()
                << messagePrefix << "Failed to query property" << name << ':'
                << mpv::qt::error_string(errorCode);
        }
    } else {
        if (ok) {
            *ok = true;
        }
        /*if ((name != "time-pos") && (name != "duration")) {
            qDebug().noquote() << "Querying a property from mpv:"
                               << name << "result:" << result;
        }*/
    }
    return result;
}

bool MpvObject::mpvObserveProperty(const QString &name) {
    if (name.isEmpty()) {
        return false;
    }
    const char messagePrefix[] = "[libmpv] [PROPERTY]";
    const int errorCode = mpv::qt::observe_property(m_pMpv, name, 0);
    if (errorCode < 0) {
        qWarning().noquote() << messagePrefix << "Failed to observe property"
                             << name << ':' << mpv::qt::error_string(errorCode);
    }
    return (errorCode >= 0);
}

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const {
    window()->setPersistentOpenGLContext(true);
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(const_cast<MpvObject *>(this));
}

QUrl MpvObject::source() const { return isStopped() ? QUrl() : currentSource; }

QString MpvObject::fileName() const {
    return isStopped()
        ? QString()
        : mpvGetProperty(QString::fromUtf8("filename")).toString();
}

QSize MpvObject::videoSize() const {
    if (isStopped()) {
        return QSize();
    }
    QSize size(
        qMax(mpvGetProperty(QString::fromUtf8("video-out-params/dw")).toInt(),
             0),
        qMax(mpvGetProperty(QString::fromUtf8("video-out-params/dh")).toInt(),
             0));
    const int rotate = videoRotate();
    if ((rotate == 90) || (rotate == 270)) {
        size.transpose();
    }
    return size;
}

MpvObject::PlaybackState MpvObject::playbackState() const {
    const bool stopped =
        mpvGetProperty(QString::fromUtf8("idle-active")).toBool();
    const bool paused = mpvGetProperty(QString::fromUtf8("pause")).toBool();
    return stopped ? PlaybackState::Stopped
                   : (paused ? PlaybackState::Paused : PlaybackState::Playing);
}

MpvObject::MediaStatus MpvObject::mediaStatus() const {
    return currentMediaStatus;
}

MpvObject::LogLevel MpvObject::logLevel() const {
    const QString level =
        mpvGetProperty(QString::fromUtf8("msg-level")).toString();
    if (level.isEmpty() || (level == QString::fromUtf8("no")) ||
        (level == QString::fromUtf8("off"))) {
        return LogLevel::Off;
    }
    const QString actualLevel = level.right(
        level.length() - level.lastIndexOf(QChar::fromLatin1('=')) - 1);
    if (actualLevel.isEmpty() || (actualLevel == QString::fromUtf8("no")) ||
        (actualLevel == QString::fromUtf8("off"))) {
        return LogLevel::Off;
    }
    if ((actualLevel == QString::fromUtf8("v")) ||
        (actualLevel == QString::fromUtf8("debug")) ||
        (actualLevel == QString::fromUtf8("trace"))) {
        return LogLevel::Debug;
    }
    if (actualLevel == QString::fromUtf8("warn")) {
        return LogLevel::Warning;
    }
    if (actualLevel == QString::fromUtf8("error")) {
        return LogLevel::Critical;
    }
    if (actualLevel == QString::fromUtf8("fatal")) {
        return LogLevel::Fatal;
    }
    if (actualLevel == QString::fromUtf8("info")) {
        return LogLevel::Info;
    }
    return LogLevel::Debug;
}

qint64 MpvObject::duration() const {
    return isStopped()
        ? 0
        : qMax(mpvGetProperty(QString::fromUtf8("duration")).toLongLong(),
               qint64(0));
}

qint64 MpvObject::position() const {
    return isStopped()
        ? 0
        : qBound(qint64(0),
                 mpvGetProperty(QString::fromUtf8("time-pos")).toLongLong(),
                 duration());
}

int MpvObject::volume() const {
    return qBound(0, mpvGetProperty(QString::fromUtf8("volume")).toInt(), 100);
}

bool MpvObject::mute() const {
    return mpvGetProperty(QString::fromUtf8("mute")).toBool();
}

bool MpvObject::seekable() const {
    return isStopped() ? false
                       : mpvGetProperty(QString::fromUtf8("seekable")).toBool();
}

QString MpvObject::mediaTitle() const {
    return isStopped()
        ? QString()
        : mpvGetProperty(QString::fromUtf8("media-title")).toString();
}

QString MpvObject::hwdec() const {
    // Querying "hwdec" itself will return empty string.
    return mpvGetProperty(QString::fromUtf8("hwdec-current")).toString();
}

QString MpvObject::mpvVersion() const {
    return mpvGetProperty(QString::fromUtf8("mpv-version")).toString();
}

QString MpvObject::mpvConfiguration() const {
    return mpvGetProperty(QString::fromUtf8("mpv-configuration")).toString();
}

QString MpvObject::ffmpegVersion() const {
    return mpvGetProperty(QString::fromUtf8("ffmpeg-version")).toString();
}

int MpvObject::vid() const {
    return isStopped() ? 0 : mpvGetProperty(QString::fromUtf8("vid")).toInt();
}

int MpvObject::aid() const {
    return isStopped() ? 0 : mpvGetProperty(QString::fromUtf8("aid")).toInt();
}

int MpvObject::sid() const {
    return isStopped() ? 0 : mpvGetProperty(QString::fromUtf8("sid")).toInt();
}

int MpvObject::videoRotate() const {
    return isStopped()
        ? 0
        : qMin(
              (qMax(mpvGetProperty(QString::fromUtf8("video-out-params/rotate"))
                        .toInt(),
                    0) +
               360) %
                  360,
              359);
}

qreal MpvObject::videoAspect() const {
    return isStopped()
        ? (16.0 / 9.0)
        : qMax(mpvGetProperty(QString::fromUtf8("video-out-params/aspect"))
                   .toReal(),
               0.0);
}

qreal MpvObject::speed() const {
    return qMax(mpvGetProperty(QString::fromUtf8("speed")).toReal(), 0.0);
}

bool MpvObject::deinterlace() const {
    return mpvGetProperty(QString::fromUtf8("deinterlace")).toBool();
}

bool MpvObject::audioExclusive() const {
    return mpvGetProperty(QString::fromUtf8("audio-exclusive")).toBool();
}

QString MpvObject::audioFileAuto() const {
    return mpvGetProperty(QString::fromUtf8("audio-file-auto")).toString();
}

QString MpvObject::subAuto() const {
    return mpvGetProperty(QString::fromUtf8("sub-auto")).toString();
}

QString MpvObject::subCodepage() const {
    QString codePage =
        mpvGetProperty(QString::fromUtf8("sub-codepage")).toString();
    if (codePage.startsWith(QChar::fromLatin1('+'))) {
        codePage.remove(0, 1);
    }
    return codePage;
}

QString MpvObject::vo() const {
    return mpvGetProperty(QString::fromUtf8("vo")).toString();
}

QString MpvObject::ao() const {
    return mpvGetProperty(QString::fromUtf8("ao")).toString();
}

QString MpvObject::screenshotFormat() const {
    return mpvGetProperty(QString::fromUtf8("screenshot-format")).toString();
}

bool MpvObject::screenshotTagColorspace() const {
    return mpvGetProperty(QString::fromUtf8("screenshot-tag-colorspace"))
        .toBool();
}

int MpvObject::screenshotPngCompression() const {
    return qBound(
        0,
        mpvGetProperty(QString::fromUtf8("screenshot-png-compression")).toInt(),
        9);
}

int MpvObject::screenshotJpegQuality() const {
    return qBound(
        0, mpvGetProperty(QString::fromUtf8("screenshot-jpeg-quality")).toInt(),
        100);
}

QString MpvObject::screenshotTemplate() const {
    return mpvGetProperty(QString::fromUtf8("screenshot-template")).toString();
}

QString MpvObject::screenshotDirectory() const {
    return mpvGetProperty(QString::fromUtf8("screenshot-directory")).toString();
}

QString MpvObject::profile() const {
    return mpvGetProperty(QString::fromUtf8("profile")).toString();
}

bool MpvObject::hrSeek() const {
    return mpvGetProperty(QString::fromUtf8("hr-seek")).toBool();
}

bool MpvObject::ytdl() const {
    return mpvGetProperty(QString::fromUtf8("ytdl")).toBool();
}

bool MpvObject::loadScripts() const {
    return mpvGetProperty(QString::fromUtf8("load-scripts")).toBool();
}

QString MpvObject::path() const {
    return isStopped()
        ? QString()
        : QDir::toNativeSeparators(
              mpvGetProperty(QString::fromUtf8("path")).toString());
}

QString MpvObject::fileFormat() const {
    return isStopped()
        ? QString()
        : mpvGetProperty(QString::fromUtf8("file-format")).toString();
}

qint64 MpvObject::fileSize() const {
    return isStopped()
        ? 0
        : qMax(mpvGetProperty(QString::fromUtf8("file-size")).toLongLong(),
               qint64(0));
}

qreal MpvObject::videoBitrate() const {
    return isStopped()
        ? 0.0
        : qMax(mpvGetProperty(QString::fromUtf8("video-bitrate")).toReal(),
               0.0);
}

qreal MpvObject::audioBitrate() const {
    return isStopped()
        ? 0.0
        : qMax(mpvGetProperty(QString::fromUtf8("audio-bitrate")).toReal(),
               0.0);
}

MpvObject::AudioDevices MpvObject::audioDeviceList() const {
    AudioDevices audioDevices;
    QVariantList deviceList =
        mpvGetProperty(QString::fromUtf8("audio-device-list")).toList();
    for (auto &&device : qAsConst(deviceList)) {
        const auto deviceInfo = device.toMap();
        SingleTrackInfo singleTrackInfo;
        singleTrackInfo[QString::fromUtf8("name")] =
            deviceInfo[QString::fromUtf8("name")];
        singleTrackInfo[QString::fromUtf8("description")] =
            deviceInfo[QString::fromUtf8("description")];
        audioDevices.append(singleTrackInfo);
    }
    return audioDevices;
}

QString MpvObject::videoFormat() const {
    return isStopped()
        ? QString()
        : mpvGetProperty(QString::fromUtf8("video-format")).toString();
}

MpvObject::MpvCallType MpvObject::mpvCallType() const {
    return currentMpvCallType;
}

MpvObject::MediaTracks MpvObject::mediaTracks() const {
    MediaTracks mediaTracks;
    QVariantList trackList =
        mpvGetProperty(QString::fromUtf8("track-list")).toList();
    for (auto &&track : qAsConst(trackList)) {
        const auto trackInfo = track.toMap();
        if ((trackInfo[QString::fromUtf8("type")] !=
             QString::fromUtf8("video")) &&
            (trackInfo[QString::fromUtf8("type")] !=
             QString::fromUtf8("audio")) &&
            (trackInfo[QString::fromUtf8("type")] !=
             QString::fromUtf8("sub"))) {
            continue;
        }
        SingleTrackInfo singleTrackInfo;
        singleTrackInfo[QString::fromUtf8("id")] =
            trackInfo[QString::fromUtf8("id")];
        singleTrackInfo[QString::fromUtf8("type")] =
            trackInfo[QString::fromUtf8("type")];
        singleTrackInfo[QString::fromUtf8("src-id")] =
            trackInfo[QString::fromUtf8("src-id")];
        if (trackInfo[QString::fromUtf8("title")].toString().isEmpty()) {
            if (trackInfo[QString::fromUtf8("lang")].toString() !=
                QString::fromUtf8("und")) {
                singleTrackInfo[QString::fromUtf8("title")] =
                    trackInfo[QString::fromUtf8("lang")];
            } else if (!trackInfo[QString::fromUtf8("external")].toBool()) {
                singleTrackInfo[QString::fromUtf8("title")] =
                    QString::fromUtf8("[internal]");
            } else {
                singleTrackInfo[QString::fromUtf8("title")] =
                    QString::fromUtf8("[untitled]");
            }
        } else {
            singleTrackInfo[QString::fromUtf8("title")] =
                trackInfo[QString::fromUtf8("title")];
        }
        singleTrackInfo[QString::fromUtf8("lang")] =
            trackInfo[QString::fromUtf8("lang")];
        singleTrackInfo[QString::fromUtf8("default")] =
            trackInfo[QString::fromUtf8("default")];
        singleTrackInfo[QString::fromUtf8("forced")] =
            trackInfo[QString::fromUtf8("forced")];
        singleTrackInfo[QString::fromUtf8("codec")] =
            trackInfo[QString::fromUtf8("codec")];
        singleTrackInfo[QString::fromUtf8("external")] =
            trackInfo[QString::fromUtf8("external")];
        singleTrackInfo[QString::fromUtf8("external-filename")] =
            trackInfo[QString::fromUtf8("external-filename")];
        singleTrackInfo[QString::fromUtf8("selected")] =
            trackInfo[QString::fromUtf8("selected")];
        singleTrackInfo[QString::fromUtf8("decoder-desc")] =
            trackInfo[QString::fromUtf8("decoder-desc")];
        if (trackInfo[QString::fromUtf8("type")] ==
            QString::fromUtf8("video")) {
            singleTrackInfo[QString::fromUtf8("albumart")] =
                trackInfo[QString::fromUtf8("albumart")];
            singleTrackInfo[QString::fromUtf8("demux-w")] =
                trackInfo[QString::fromUtf8("demux-w")];
            singleTrackInfo[QString::fromUtf8("demux-h")] =
                trackInfo[QString::fromUtf8("demux-h")];
            singleTrackInfo[QString::fromUtf8("demux-fps")] =
                trackInfo[QString::fromUtf8("demux-fps")];
            mediaTracks.videoChannels.append(singleTrackInfo);
        } else if (trackInfo[QString::fromUtf8("type")] ==
                   QString::fromUtf8("audio")) {
            singleTrackInfo[QString::fromUtf8("demux-channel-count")] =
                trackInfo[QString::fromUtf8("demux-channel-count")];
            singleTrackInfo[QString::fromUtf8("demux-channels")] =
                trackInfo[QString::fromUtf8("demux-channels")];
            singleTrackInfo[QString::fromUtf8("demux-samplerate")] =
                trackInfo[QString::fromUtf8("demux-samplerate")];
            mediaTracks.audioTracks.append(singleTrackInfo);
        } else if (trackInfo[QString::fromUtf8("type")] ==
                   QString::fromUtf8("sub")) {
            mediaTracks.subtitleStreams.append(singleTrackInfo);
        }
    }
    return mediaTracks;
}

MpvObject::Chapters MpvObject::chapters() const {
    Chapters chapters;
    QVariantList chapterList =
        mpvGetProperty(QString::fromUtf8("chapter-list")).toList();
    for (auto &&chapter : qAsConst(chapterList)) {
        const auto chapterInfo = chapter.toMap();
        SingleTrackInfo singleTrackInfo;
        singleTrackInfo[QString::fromUtf8("title")] =
            chapterInfo[QString::fromUtf8("title")];
        singleTrackInfo[QString::fromUtf8("time")] =
            chapterInfo[QString::fromUtf8("time")];
        chapters.append(singleTrackInfo);
    }
    return chapters;
}

MpvObject::Metadata MpvObject::metadata() const {
    Metadata metadata;
    QVariantMap metadataMap =
        mpvGetProperty(QString::fromUtf8("metadata")).toMap();
    auto iterator = metadataMap.cbegin();
    while (iterator != metadataMap.cend()) {
        metadata[iterator.key()] = iterator.value();
        ++iterator;
    }
    return metadata;
}

qreal MpvObject::avsync() const {
    return isStopped()
        ? 0.0
        : qMax(mpvGetProperty(QString::fromUtf8("avsync")).toReal(), 0.0);
}

int MpvObject::percentPos() const {
    return isStopped()
        ? 0
        : qBound(0, mpvGetProperty(QString::fromUtf8("percent-pos")).toInt(),
                 100);
}

qreal MpvObject::estimatedVfFps() const {
    return isStopped()
        ? 0.0
        : qMax(mpvGetProperty(QString::fromUtf8("estimated-vf-fps")).toReal(),
               0.0);
}

bool MpvObject::open(const QUrl &url) {
    if (!url.isValid()) {
        return false;
    }
    if (url != currentSource) {
        setSource(url);
    }
    if (!isPlaying()) {
        play();
    }
    return true;
}

bool MpvObject::play() {
    if (!isPaused() || !currentSource.isValid()) {
        return false;
    }
    const bool result = mpvSetProperty(QString::fromUtf8("pause"), false);
    if (result) {
        Q_EMIT playing();
    }
    return result;
}

bool MpvObject::play(const QUrl &url) {
    if (!url.isValid()) {
        return false;
    }
    bool result = false;
    if ((url == currentSource) && !isPlaying()) {
        result = play();
    } else {
        result = open(url);
    }
    return result;
}

bool MpvObject::pause() {
    if (!isPlaying()) {
        return false;
    }
    const bool result = mpvSetProperty(QString::fromUtf8("pause"), true);
    if (result) {
        Q_EMIT paused();
    }
    return result;
}

bool MpvObject::stop() {
    if (isStopped()) {
        return false;
    }
    const bool result = mpvSendCommand(QVariantList{QString::fromUtf8("stop")});
    if (result) {
        Q_EMIT stopped();
    }
    currentSource.clear();
    return result;
}

bool MpvObject::seek(qint64 value, bool absolute, bool percent) {
    if (isStopped()) {
        return false;
    }
    const qint64 min = (absolute || percent) ? 0 : -position();
    const qint64 max =
        percent ? 100 : (absolute ? duration() : duration() - position());
    return mpvSendCommand(
        QVariantList{QString::fromUtf8("seek"), qBound(min, value, max),
                     percent ? QString::fromUtf8("absolute-percent")
                             : (absolute ? QString::fromUtf8("absolute")
                                         : QString::fromUtf8("relative"))});
}

bool MpvObject::seekAbsolute(qint64 position) {
    if (isStopped() || (position == this->position())) {
        return false;
    }
    return seek(qBound(qint64(0), position, duration()), true);
}

bool MpvObject::seekRelative(qint64 offset) {
    if (isStopped() || (offset == 0)) {
        return false;
    }
    return seek(qBound(-position(), offset, duration() - position()));
}

bool MpvObject::seekPercent(int percent) {
    if (isStopped() || (percent == this->percentPos())) {
        return false;
    }
    return seek(qBound(0, percent, 100), true, true);
}

bool MpvObject::screenshot() {
    if (isStopped()) {
        return false;
    }
    // Replace "subtitles" with "video" if you don't want to include subtitles
    // when screenshotting.
    return mpvSendCommand(QVariantList{QString::fromUtf8("screenshot"),
                                       QString::fromUtf8("subtitles")});
}

bool MpvObject::screenshotToFile(const QString &filePath) {
    if (isStopped() || filePath.isEmpty()) {
        return false;
    }
    // libmpv's default: including subtitles when making a screenshot.
    return mpvSendCommand(QVariantList{QString::fromUtf8("screenshot-to-file"),
                                       filePath,
                                       QString::fromUtf8("subtitles")});
}

bool MpvObject::loadConfigFile(const QString &path) {
    if (path.isEmpty() || !QFileInfo::exists(path)) {
        return false;
    }
    const char messagePrefix[] = "[libmpv] [MISC]";
    const int errorCode = mpv::qt::load_config_file(m_pMpv, path);
    if (errorCode < 0) {
        qWarning().noquote()
            << messagePrefix << "Failed to load the config file" << path << ':'
            << mpv::qt::error_string(errorCode);
    }
    return (errorCode >= 0);
}

void MpvObject::setSource(const QUrl &source) {
    if (!source.isValid() || (source == currentSource)) {
        return;
    }
    const bool result = mpvSendCommand(QVariantList{
        QString::fromUtf8("loadfile"),
        source.isLocalFile() ? QDir::toNativeSeparators(source.toLocalFile())
                             : source.url()});
    if (result) {
        currentSource = source;
        Q_EMIT sourceChanged();
    }
}

void MpvObject::setMute(bool mute) {
    if (mute == this->mute()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("mute"), mute);
}

void MpvObject::setPlaybackState(MpvObject::PlaybackState playbackState) {
    if (isStopped() || (this->playbackState() == playbackState)) {
        return;
    }
    bool result = false;
    switch (playbackState) {
    case PlaybackState::Stopped:
        result = stop();
        break;
    case PlaybackState::Paused:
        result = pause();
        break;
    case PlaybackState::Playing:
        result = play();
        break;
    }
    if (result) {
        Q_EMIT playbackStateChanged();
    }
}

void MpvObject::setLogLevel(MpvObject::LogLevel logLevel) {
    if (logLevel == this->logLevel()) {
        return;
    }
    QString level = QString::fromUtf8("debug");
    switch (logLevel) {
    case LogLevel::Off:
        level = QString::fromUtf8("no");
        break;
    case LogLevel::Debug:
        // libmpv's log level: v (verbose) < debug < trace (print all messages)
        // Use "v" to avoid noisy message floods.
        level = QString::fromUtf8("v");
        break;
    case LogLevel::Warning:
        level = QString::fromUtf8("warn");
        break;
    case LogLevel::Critical:
        level = QString::fromUtf8("error");
        break;
    case LogLevel::Fatal:
        level = QString::fromUtf8("fatal");
        break;
    case LogLevel::Info:
        level = QString::fromUtf8("info");
        break;
    }
    const bool result1 = mpvSetProperty(QString::fromUtf8("terminal"),
                                        level != QString::fromUtf8("no"));
    const bool result2 = mpvSetProperty(QString::fromUtf8("msg-level"),
                                        QString::fromUtf8("all=%1").arg(level));
    const int errorCode = mpv::qt::request_log_messages(m_pMpv, level);
    const char messagePrefix[] = "[libmpv] [MISC]";
    if (result1 && result2 && (errorCode >= 0)) {
        Q_EMIT logLevelChanged();
    } else {
        qWarning().noquote()
            << messagePrefix << "Failed to change log level to" << level << ':'
            << mpv::qt::error_string(errorCode);
    }
}

void MpvObject::setPosition(qint64 position) {
    if (isStopped() || (position == this->position())) {
        return;
    }
    seek(qBound(qint64(0), position, duration()));
}

void MpvObject::setVolume(int volume) {
    if (volume == this->volume()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("volume"), qBound(0, volume, 100));
}

void MpvObject::setHwdec(const QString &hwdec) {
    if (hwdec.isEmpty() || (hwdec == this->hwdec())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("hwdec"), hwdec);
}

void MpvObject::setVid(int vid) {
    if (isStopped() || (vid == this->vid())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("vid"), qMax(vid, 0));
}

void MpvObject::setAid(int aid) {
    if (isStopped() || (aid == this->aid())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("aid"), qMax(aid, 0));
}

void MpvObject::setSid(int sid) {
    if (isStopped() || (sid == this->sid())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("sid"), qMax(sid, 0));
}

void MpvObject::setVideoRotate(int videoRotate) {
    if (isStopped() || (videoRotate == this->videoRotate())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("video-rotate"),
                   qBound(0, videoRotate, 359));
}

void MpvObject::setVideoAspect(qreal videoAspect) {
    if (isStopped() || (videoAspect == this->videoAspect())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("video-aspect-override"),
                   qMax(videoAspect, 0.0));
}

void MpvObject::setSpeed(qreal speed) {
    if (isStopped() || (speed == this->speed())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("speed"), qMax(speed, 0.0));
}

void MpvObject::setDeinterlace(bool deinterlace) {
    if (deinterlace == this->deinterlace()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("deinterlace"), deinterlace);
}

void MpvObject::setAudioExclusive(bool audioExclusive) {
    if (audioExclusive == this->audioExclusive()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("audio-exclusive"), audioExclusive);
}

void MpvObject::setAudioFileAuto(const QString &audioFileAuto) {
    if (audioFileAuto.isEmpty() || (audioFileAuto == this->audioFileAuto())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("audio-file-auto"), audioFileAuto);
}

void MpvObject::setSubAuto(const QString &subAuto) {
    if (subAuto.isEmpty() || (subAuto == this->subAuto())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("sub-auto"), subAuto);
}

void MpvObject::setSubCodepage(const QString &subCodepage) {
    if (subCodepage.isEmpty() || (subCodepage == this->subCodepage())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("sub-codepage"),
                   subCodepage.startsWith(QChar::fromLatin1('+'))
                       ? subCodepage
                       : (subCodepage.startsWith(QString::fromUtf8("cp"))
                              ? QChar::fromLatin1('+') + subCodepage
                              : subCodepage));
}

void MpvObject::setVo(const QString &vo) {
    if (vo.isEmpty() || (vo == this->vo())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("vo"), vo);
}

void MpvObject::setAo(const QString &ao) {
    if (ao.isEmpty() || (ao == this->ao())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("ao"), ao);
}

void MpvObject::setScreenshotFormat(const QString &screenshotFormat) {
    if (screenshotFormat.isEmpty() ||
        (screenshotFormat == this->screenshotFormat())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-format"), screenshotFormat);
}

void MpvObject::setScreenshotPngCompression(int screenshotPngCompression) {
    if (screenshotPngCompression == this->screenshotPngCompression()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-png-compression"),
                   qBound(0, screenshotPngCompression, 9));
}

void MpvObject::setScreenshotTemplate(const QString &screenshotTemplate) {
    if (screenshotTemplate.isEmpty() ||
        (screenshotTemplate == this->screenshotTemplate())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-template"),
                   screenshotTemplate);
}

void MpvObject::setScreenshotDirectory(const QString &screenshotDirectory) {
    if (screenshotDirectory.isEmpty() ||
        (screenshotDirectory == this->screenshotDirectory())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-directory"),
                   screenshotDirectory);
}

void MpvObject::setProfile(const QString &profile) {
    if (profile.isEmpty() || (profile == this->profile())) {
        return;
    }
    mpvSendCommand(QVariantList{QString::fromUtf8("apply-profile"), profile});
}

void MpvObject::setHrSeek(bool hrSeek) {
    if (hrSeek == this->hrSeek()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("hr-seek"),
                   hrSeek ? QString::fromUtf8("yes") : QString::fromUtf8("no"));
}

void MpvObject::setYtdl(bool ytdl) {
    if (ytdl == this->ytdl()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("ytdl"), ytdl);
}

void MpvObject::setLoadScripts(bool loadScripts) {
    if (loadScripts == this->loadScripts()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("load-scripts"), loadScripts);
}

void MpvObject::setScreenshotTagColorspace(bool screenshotTagColorspace) {
    if (screenshotTagColorspace == this->screenshotTagColorspace()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-tag-colorspace"),
                   screenshotTagColorspace);
}

void MpvObject::setScreenshotJpegQuality(int screenshotJpegQuality) {
    if (screenshotJpegQuality == this->screenshotJpegQuality()) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("screenshot-jpeg-quality"),
                   qBound(0, screenshotJpegQuality, 100));
}

void MpvObject::setMpvCallType(MpvObject::MpvCallType mpvCallType) {
    if (this->mpvCallType() == mpvCallType) {
        return;
    }
    currentMpvCallType = mpvCallType;
    Q_EMIT mpvCallTypeChanged();
}

void MpvObject::setPercentPos(int percentPos) {
    if (isStopped() || (percentPos == this->percentPos())) {
        return;
    }
    mpvSetProperty(QString::fromUtf8("percent-pos"),
                   qBound(0, percentPos, 100));
}

void MpvObject::handleMpvEvents() {
    // Process all events, until the event queue is empty.
    while (m_pMpv) {
        const auto event = mpv::qt::wait_event(m_pMpv, 0.005);
        // Nothing happened. Happens on timeouts or sporadic wakeups.
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        bool shouldOutput = true;
        switch (event->event_id) {
        // Happens when the player quits. The player enters a state where it
        // tries to disconnect all clients. Most requests to the player will
        // fail, and the client should react to this and quit with
        // mpv_destroy() as soon as possible.
        case MPV_EVENT_SHUTDOWN:
            break;
        // See mpv_request_log_messages().
        case MPV_EVENT_LOG_MESSAGE:
            processMpvLogMessage(event->data);
            shouldOutput = false;
            break;
        // Reply to a mpv_get_property_async() request.
        // See also mpv_event and mpv_event_property.
        case MPV_EVENT_GET_PROPERTY_REPLY:
            shouldOutput = false;
            break;
        // Reply to a mpv_set_property_async() request.
        // (Unlike MPV_EVENT_GET_PROPERTY, mpv_event_property is not used.)
        case MPV_EVENT_SET_PROPERTY_REPLY:
            shouldOutput = false;
            break;
        // Reply to a mpv_command_async() or mpv_command_node_async() request.
        // See also mpv_event and mpv_event_command.
        case MPV_EVENT_COMMAND_REPLY:
            shouldOutput = false;
            break;
        // Notification before playback start of a file (before the file is
        // loaded).
        case MPV_EVENT_START_FILE:
            setMediaStatus(MediaStatus::Loading);
            break;
        // Notification after playback end (after the file was unloaded).
        // See also mpv_event and mpv_event_end_file.
        case MPV_EVENT_END_FILE:
            setMediaStatus(MediaStatus::End);
            playbackStateChangeEvent();
            break;
        // Notification when the file has been loaded (headers were read
        // etc.), and decoding starts.
        case MPV_EVENT_FILE_LOADED:
            setMediaStatus(MediaStatus::Loaded);
            Q_EMIT loaded();
            playbackStateChangeEvent();
            break;
        // Triggered by the script-message input command. The command uses the
        // first argument of the command as client name (see mpv_client_name())
        // to dispatch the message, and passes along all arguments starting from
        // the second argument as strings.
        // See also mpv_event and mpv_event_client_message.
        case MPV_EVENT_CLIENT_MESSAGE:
            break;
        // Happens after video changed in some way. This can happen on
        // resolution changes, pixel format changes, or video filter changes.
        // The event is sent after the video filters and the VO are
        // reconfigured. Applications embedding a mpv window should listen to
        // this event in order to resize the window if needed.
        // Note that this event can happen sporadically, and you should check
        // yourself whether the video parameters really changed before doing
        // something expensive.
        case MPV_EVENT_VIDEO_RECONFIG:
            videoReconfig();
            break;
        // Similar to MPV_EVENT_VIDEO_RECONFIG. This is relatively
        // uninteresting, because there is no such thing as audio output
        // embedding.
        case MPV_EVENT_AUDIO_RECONFIG:
            audioReconfig();
            break;
        // Happens when a seek was initiated. Playback stops. Usually it will
        // resume with MPV_EVENT_PLAYBACK_RESTART as soon as the seek is
        // finished.
        case MPV_EVENT_SEEK:
            break;
        // There was a discontinuity of some sort (like a seek), and playback
        // was reinitialized. Usually happens after seeking, or ordered chapter
        // segment switches. The main purpose is allowing the client to detect
        // when a seek request is finished.
        case MPV_EVENT_PLAYBACK_RESTART:
            break;
        // Event sent due to mpv_observe_property().
        // See also mpv_event and mpv_event_property.
        case MPV_EVENT_PROPERTY_CHANGE:
            processMpvPropertyChange(event->data);
            shouldOutput = false;
            break;
        // Happens if the internal per-mpv_handle ringbuffer overflows, and at
        // least 1 event had to be dropped. This can happen if the client
        // doesn't read the event queue quickly enough with mpv_wait_event(), or
        // if the client makes a very large number of asynchronous calls at
        // once.
        // Event delivery will continue normally once this event was returned
        // (this forces the client to empty the queue completely).
        case MPV_EVENT_QUEUE_OVERFLOW:
            break;
        // Triggered if a hook handler was registered with mpv_hook_add(), and
        // the hook is invoked. If you receive this, you must handle it, and
        // continue the hook with mpv_hook_continue().
        // See also mpv_event and mpv_event_hook.
        case MPV_EVENT_HOOK:
            break;
        default:
            break;
        }
        if (shouldOutput) {
            qDebug().noquote()
                << "[libmpv] [EVENT]" << mpv::qt::event_name(event->event_id)
                << "event received.";
        }
    }
}
