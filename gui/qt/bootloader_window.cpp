#include "bootloader_window.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
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

  {
    QLineEdit * filename_input = new QLineEdit();
    QLabel tmpLabel;
    tmpLabel.setText("C:/Users/SomePersonsLongerName/Downloads/abc01a-v1.00.fmi");
    filename_input->setMinimumWidth(tmpLabel.sizeHint().width());
    layout->addWidget(filename_input, 0, 1);
  }

  QPushButton * browse_button = new QPushButton();
  browse_button->setText(tr("&Browse..."));
  browse_button->setObjectName("browse_button");
  layout->addWidget(browse_button, 0, 20);

  QLabel * device_label = new QLabel();
  device_label->setText(tr("Device:"));
  layout->addWidget(device_label, 1, 0, FIELD_LABEL_ALIGNMENT);

  layout->setColumnStretch(2, 1);
  layout->setRowStretch(3, 1);

  central_widget->setLayout(layout);
  setCentralWidget(central_widget);

  QMetaObject::connectSlotsByName(this);
}
