#include "bootloader_window.h"

#include <QAbstractListModel>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

class BootloaderComboBox : public QComboBox
{
  int x = 0;

  void showPopup()
  {
    clear();
    addItem("hey", 1);
    addItem("foo", 2);

    /**
    addItem("whee: " + QString::number(x), x);
    x++;
    addItem("bee: " + QString::number(x), x);
    x++;
    addItems({"hi", "there"});
    **/
  }
};

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
    filename_input = new QLineEdit();
    QLabel tmp;
    tmp.setText("C:/Users/SomePersonsLongerName/Downloads/abc01a-v1.00.fmi");
    filename_input->setMinimumWidth(tmp.sizeHint().width());
    layout->addWidget(filename_input, 0, 1);
  }

  browse_button = new QPushButton();
  browse_button->setText(tr("&Browse..."));
  browse_button->setObjectName("browse_button");
  layout->addWidget(browse_button, 0, 2);

  QLabel * device_label = new QLabel();
  device_label->setText(tr("Device:"));
  layout->addWidget(device_label, 1, 0, FIELD_LABEL_ALIGNMENT);

  {
    device_chooser = new BootloaderComboBox();
    QComboBox tmp;
    tmp.addItem("XXXXXX: #1234567890123456");
    device_chooser->setMinimumWidth(tmp.sizeHint().width());
    device_chooser->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(device_chooser, 1, 1);
  }

  progress_label = new QLabel();
  layout->addWidget(progress_label, 2, 1);

  progress_bar = new QProgressBar();
  progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  progress_bar->setVisible(false);
  layout->addWidget(progress_bar, 3, 1);

  program_button = new QPushButton();
  program_button->setText(tr("&Program"));
  program_button->setObjectName("program_button");
  layout->addWidget(program_button, 3, 2);

  layout->setColumnStretch(1, 1);
  layout->setRowStretch(4, 1);

  central_widget->setLayout(layout);
  setCentralWidget(central_widget);

  QMetaObject::connectSlotsByName(this);
}

/**
void bootloader_window::on_device_chooser_open()
{
  device_chooser->addItem(QString());
}
**/
