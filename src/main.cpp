#include <QApplication>
#include <QtDebug>

#include "gui/login_window.h"
#include "lib/easyloggingpp/src/easylogging++.h"
#include "utils/easylog_wrapper.h"

INITIALIZE_EASYLOGGINGPP

// static el::Logger *logger = el::Loggers::getLogger("default");
// static LogBuffer buffer(logger);

int main(int argc, char* argv[])
{
  START_EASYLOGGINGPP(argc, argv);
  // Configure all loggers
  el::Loggers::configureFromGlobal(SRCDIR "/config/logs.conf");

  QApplication a(argc, argv);
  qRegisterMetaType<grabec::GSWDriveInPdos>("grabec::GSWDriveInPdos");
  qRegisterMetaType<Bitfield8>("Bitfield8");
  qRegisterMetaType<id_t>("id_t");
  CLOG(INFO, "event") << "App START";

  LoginWindow w;
  w.show();
  CLOG(INFO, "event") << "Prompt login window";

  return a.exec();
}
