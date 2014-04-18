#include <QDBusAbstractAdaptor>
#include "adaptor.h"

Adaptor::Adaptor(QObject *parent) : QDBusAbstractAdaptor(parent) {
  setAutoRelaySignals(true);
}

Adaptor::~Adaptor(){

}
