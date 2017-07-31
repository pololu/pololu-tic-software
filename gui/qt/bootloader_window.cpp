#include "bootloader_window.h"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

// On Mac OS X, field labels are usually right-aligned.
#ifdef __APPLE__
#define FIELD_LABEL_ALIGNMENT Qt::AlignRight
#else
#define FIELD_LABEL_ALIGNMENT Qt::AlignLeft
#endif

bootloader_window::bootloader_window(QWidget * parent)
  : QMainWindow(parent)
{
  setup_window();
}

void bootloader_window::setup_window()
{
  setWindowTitle(tr("Upgrade Firmware"));
  setStyleSheet("QPushButton { padding: 0.3em 1em; }");

  QWidget * central_widget = new QWidget();
  QGridLayout * layout = new QGridLayout();

  QLabel * file_label = new QLabel();
  file_label->setText(tr("Firmware file:"));
  layout->addWidget(file_label, 0, 0, FIELD_LABEL_ALIGNMENT);

  layout->setColumnStretch(2, 1);
  layout->setRowStretch(3, 1);

  central_widget->setLayout(layout);
  setCentralWidget(central_widget);

  QMetaObject::connectSlotsByName(this);
}
