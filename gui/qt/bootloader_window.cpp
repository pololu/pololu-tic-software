#include "bootloader_window.h"
#include "file_util.h"

#include <bootloader.h>

#include <QAbstractListModel>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

// #include <libusbp-1/libusbp.hpp>  // tmphax

static QString directory_hint = QDir::homePath();

void update_bootloader_combo_box(QComboBox & box)
{
  // Record the OS ID of the item currently selected.
  QString id;
  if (box.currentData().isValid())
  {
    id = box.currentData().toString();
  }

  // tmphax: show all USB devices
  // auto device_list = libusbp::list_connected_devices();
  auto device_list = bootloader::list_connected_devices();
  box.clear();
  box.addItem("", QString());
  for (const auto & device : device_list)
  {
    box.addItem(
      QString::fromStdString(device.get_short_name() +
        " #" + device.get_serial_number()),
      QString::fromStdString(device.get_os_id()));
  }

  int index = box.findData(id);  // could be -1 if it is not found
  box.setCurrentIndex(index);
}

class BootloaderComboBox : public QComboBox
{
  void showPopup()
  {
    update_bootloader_combo_box(*this);
    QComboBox::showPopup();
  }
};

// On Mac OS X, field labels are usually right-aligned.
#ifdef __APPLE__
#define FIELD_LABEL_ALIGNMENT Qt::AlignRight
#else
#define FIELD_LABEL_ALIGNMENT Qt::AlignLeft
#endif

bootloader_window::bootloader_window(QWidget * parent)
{
  setup_window();

  // Set the parent this way to improve the centering of the Window.
  // (It's a bit far up instead of being a bit far to the right.)
  setParent(parent, Qt::Window);
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
    tmp.addItem("XXXXXX bootloader: #1234567890123456");
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

void bootloader_window::on_browse_button_clicked()
{
  QString filename = QFileDialog::getOpenFileName(this,
    tr("Select a Firmware File"),
    directory_hint,
    tr("Firmware image files (*.fmi)"));

  if (!filename.isNull())
  {
    directory_hint = QFileInfo(filename).canonicalPath();
    filename_input->setText(filename);
  }
}

void bootloader_window::on_program_button_clicked()
{
  // Make sure a firmware file was selected.
  std::string filename = filename_input->text().toStdString();
  if (filename.empty())
  {
    show_error_message("Please enter a filename.");
    return;
  }

  // Read in the firmware file.
  std::string file_contents;
  try
  {
    file_contents = read_string_from_file(filename);
  }
  catch (const std::exception & e)
  {
    show_error_message(e.what());
    return;
  }

  // Parse the firmware file.
  firmware_archive::data data;
  try
  {
    data.read_from_string(file_contents);
  }
  catch (const std::exception & e)
  {
    show_error_message(e.what());
    return;
  }

  // Make sure a bootloader was selected.
  std::string bootloader_id;
  if (device_chooser->currentData().isValid())
  {
    bootloader_id = device_chooser->currentData().toString().toStdString();
  }
  if (bootloader_id.empty())
  {
    show_error_message("Please select a device.");
    return;
  }

  /**
  auto device_list = bootloader::list_connected_devices();

  const firmware_archive::image & image;
  try
  {
    image = data.find_image(bootloader.get_vendor_id(), bootloader.get_product_id());
  }
  **/
}

void bootloader_window::show_error_message(const std::string & message)
{
  QMessageBox mbox(QMessageBox::Critical, windowTitle(),
    QString::fromStdString(message), QMessageBox::NoButton, this);
  mbox.exec();
}

