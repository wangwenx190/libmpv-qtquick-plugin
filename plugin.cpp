#include <QQmlEngineExtensionPlugin>

class MpvDeclarativeWrapper : public QQmlEngineExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)
};

#include "plugin.moc"
