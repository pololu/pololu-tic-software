#include <QApplication>
#include "main_controller.h"
#include "main_window.h"

int main(int argc, char ** argv)
{
  QApplication app(argc, argv);
  main_controller controller;
  main_window window;
  controller.set_window(&window);
  window.set_controller(&controller);
  window.show();
  return app.exec();
}
