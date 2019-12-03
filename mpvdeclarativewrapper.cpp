#include "mpvdeclarativewrapper.h"
#include "mpvdeclarativeobject.h"

void MpvDeclarativeWrapper::registerTypes(const char *uri) {
    Q_ASSERT(uri == "wangwenx190.QuickMpv");
    qmlRegisterType<MpvDeclarativeObject>(uri, 1, 0, "MpvObject");
}
