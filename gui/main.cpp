#include <QApplication>
#include <QStyleFactory>
#include "main_controller.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
  // AA_EnableHighDpiScaling was added in Qt 5.6.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  // On non-Windows systems, use Qt's fusion style instead of a
  // native style.
#ifndef _WIN32
  QApplication::setStyle(QStyleFactory::create("fusion"));
#endif

  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);
  window.show();
  return app.exec();
}
