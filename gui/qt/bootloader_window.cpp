#include "bootloader_window.h"

bootloader_window::bootloader_window(QWidget * parent)
  : QMainWindow(parent)
{
  setup_window();
}

void bootloader_window::setup_window()
{
  retranslate();
  QMetaObject::connectSlotsByName(this);
}

void bootloader_window::retranslate()
{
  setWindowTitle(tr("Upgrade Firmware"));
}
