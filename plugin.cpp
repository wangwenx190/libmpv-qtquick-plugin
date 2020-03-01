#include <QQmlEngineExtensionPlugin>

extern void qml_register_types_wangwenx190_QuickMpv();

class MpvDeclarativeWrapper : public QQmlEngineExtensionPlugin {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MpvDeclarativeWrapper)
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
    explicit MpvDeclarativeWrapper(QObject *parent = nullptr)
        : QQmlEngineExtensionPlugin(parent) {
        volatile auto registration = &qml_register_types_wangwenx190_QuickMpv;
        Q_UNUSED(registration)
    }
    ~MpvDeclarativeWrapper() override = default;
};

#include "plugin.moc"
