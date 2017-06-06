#include "main_window.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include <cassert>

main_window::main_window(QWidget *parent)
  : QMainWindow(parent)
{
  setup_window();
}

void main_window::show_error_message(const std::string & message)
{
    QMessageBox mbox(QMessageBox::Critical, windowTitle(),
      QString(message.c_str()));
    mbox.exec();
}

void main_window::show_exception(const std::exception & e,
    const std::string & context)
{
    std::string message;
    if (context.size() > 0)
    {
      message += context;
      message += "  ";
    }
    message += e.what();
    show_error_message(message);
}

void main_window::set_connection_status(const std::string & status, bool error)
{
  if (error)
  {
    connection_status_value->setStyleSheet("QLabel { color: red; }");
  }
  else
  {
    connection_status_value->setStyleSheet("");
  }
  connection_status_value->setText(QString(status.c_str()));
}

void main_window::set_accel_max(uint32_t accel_max)
{
  suppress_events = true;
  accel_max_value->setValue(accel_max);
  suppress_events = false;
}

void main_window::showEvent(QShowEvent * event)
{
  Q_UNUSED(event);
  if (!start_event_reported)
  {
    start_event_reported = true;
    start_controller();
  }
}

void main_window::on_connect_action_triggered()
{
  connect_to_device();
}

void main_window::on_apply_settings_action_triggered()
{
  apply_settings();
}

void main_window::on_accel_max_value_valueChanged(int value)
{
    if (suppress_events) { return; }
    handle_accel_max_input(value);
}

// On Mac OS X, field labels are usually right-aligned.
#ifdef __APPLE__
#define FIELD_LABEL_ALIGNMENT Qt::AlignRight
#else
#define FIELD_LABEL_ALIGNMENT Qt::AlignLeft
#endif

void main_window::setup_window()
{
  setup_menu_bar();
  
  central_widget = new QWidget();
  QGridLayout * layout = central_widget_layout = new QGridLayout();
  
  int row = 0;
  // todo: will require rework if we add more columns
  layout->addWidget(setup_settings_widget(), row++, 0, 0);
  layout->addWidget(setup_footer(), row++, 0, 0);
  
  central_widget->setLayout(layout);
  setCentralWidget(central_widget);
  
  retranslate();
  
  // Make the window non-resizable.
  setFixedSize(sizeHint());
  
  QMetaObject::connectSlotsByName(this);
}

void main_window::setup_menu_bar()
{
  menu_bar = new QMenuBar();
      
  file_menu = menu_bar->addMenu("");

  exit_action = new QAction(this);
  exit_action->setObjectName("exit_action");
  connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));
  file_menu->addAction(exit_action);

  device_menu = menu_bar->addMenu("");

  connect_action = new QAction(this);
  connect_action->setObjectName("connect_action");
  connect_action->setShortcut(Qt::CTRL + Qt::Key_N);
  // todo: connect by name?
  //connect(connect_action, SIGNAL(triggered()),
  //  this, SLOT(on_connect_action_triggered()));
  device_menu->addAction(connect_action);

  disconnect_action = new QAction(this);
  disconnect_action->setObjectName("disconnect_action");
  disconnect_action->setShortcut(Qt::CTRL + Qt::Key_D);
  device_menu->addAction(disconnect_action);

  device_menu->addSeparator();

  reload_settings_action = new QAction(this);
  reload_settings_action->setObjectName("reload_settings_action");
  device_menu->addAction(reload_settings_action);

  restore_defaults_action = new QAction(this);
  restore_defaults_action->setObjectName("restore_defaults_action");
  device_menu->addAction(restore_defaults_action);

  apply_settings_action = new QAction(this);
  apply_settings_action->setObjectName("apply_settings_action");
  apply_settings_action->setShortcut(Qt::CTRL + Qt::Key_P);
  device_menu->addAction(apply_settings_action);

  help_menu = menu_bar->addMenu("");

  documentation_action = new QAction(this);
  documentation_action->setObjectName("documentation_action");
  documentation_action->setShortcut(QKeySequence::HelpContents);
  help_menu->addAction(documentation_action);

  about_action = new QAction(this);
  about_action->setObjectName("about_action");
  about_action->setShortcut(QKeySequence::WhatsThis);
  help_menu->addAction(about_action);

  setMenuBar(menu_bar);
}

QWidget * main_window::setup_settings_widget()
{
  // todo: put box in layout in widget or get rid of the extra layer
  return setup_settings_box();
}

// [all-settings]
QWidget * main_window::setup_settings_box()
{
  settings_box = new QGroupBox();
  QGridLayout * layout = settings_box_layout = new QGridLayout();
  //layout->setColumnStretch(1, 1);
  int row = 0;
  
  {
    accel_max_value = new QSpinBox();
    accel_max_value->setObjectName("accel_max_value");
    accel_max_value->setRange(TIC_MIN_ALLOWED_ACCEL, TIC_MAX_ALLOWED_ACCEL);
    accel_max_label = new QLabel();
    accel_max_label->setBuddy(accel_max_value);
    layout->addWidget(accel_max_label, row, 0, FIELD_LABEL_ALIGNMENT);
    layout->addWidget(accel_max_value, row, 1, Qt::AlignLeft);
    row++;
  }
  
  //layout->setRowStretch(row, 1);

  settings_box->setLayout(layout);
  return settings_box;
}

QWidget * main_window::setup_footer()
{
  footer_widget = new QWidget();
  QGridLayout * layout = footer_widget_layout = new QGridLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  int col = 0;
  layout->addWidget(setup_connection_status(), 0, col++, Qt::AlignLeft);
  layout->addWidget(setup_apply_button(), 0, col++, Qt::AlignRight);
  
  layout->setColumnStretch(0, 1);
  
  footer_widget->setLayout(layout);
  return footer_widget;
}

QWidget * main_window::setup_connection_status()
{
  connection_status_value = new QLabel();
  return connection_status_value;
}

QWidget * main_window::setup_apply_button()
{
  apply_settings_button = new QPushButton();

  connect(apply_settings_button, SIGNAL(clicked()),
    this, SLOT(on_apply_settings_action_triggered()));
  return apply_settings_button;
}

void main_window::retranslate()
{
  setWindowTitle(tr("Pololu Tic Configuration Utility"));

  file_menu->setTitle(tr("&File"));
  exit_action->setText(tr("E&xit"));
  device_menu->setTitle(tr("&Device"));
  connect_action->setText(tr("&Connect"));
  disconnect_action->setText(tr("&Disconnect"));
  reload_settings_action->setText(tr("Re&load Settings from Device"));
  restore_defaults_action->setText(tr("&Restore Default Settings"));
  apply_settings_action->setText(tr("&Apply Settings"));
  help_menu->setTitle(tr("&Help"));
  documentation_action->setText(tr("&Online Documentation..."));
  about_action->setText(tr("&About..."));

  // deviceInfoBox->setTitle(tr("Device info"));
  // deviceNameLabel->setText(tr("Name") + FIELD_LABEL_SUFFIX);
  // serialNumberLabel->setText(tr("Serial number") + FIELD_LABEL_SUFFIX);
  // firmwareVersionLabel->setText(tr("Firmware version") + FIELD_LABEL_SUFFIX);
  // progPortLabel->setText(tr("Programming port") + FIELD_LABEL_SUFFIX);
  // ttlPortLabel->setText(tr("TTL port") + FIELD_LABEL_SUFFIX);
  // lastDeviceResetLabel->setText(tr("Last device reset") + FIELD_LABEL_SUFFIX);

  // programmingResultsBox->setTitle(tr("Results from last programming"));
  // measuredVccMinLabel->setText(tr("Target VCC measured minimum") + FIELD_LABEL_SUFFIX);
  // measuredVccMaxLabel->setText(tr("Target VCC measured maximum") + FIELD_LABEL_SUFFIX);
  // measuredVddMinLabel->setText(tr("Programmer VDD measured minimum") + FIELD_LABEL_SUFFIX);
  // measuredVddMaxLabel->setText(tr("Programmer VDD measured maximum") + FIELD_LABEL_SUFFIX);

  // currentStatusBox->setTitle(tr("Current status"));
  // currentVccLabel->setText(tr("Target VCC") + FIELD_LABEL_SUFFIX);
  // currentVddLabel->setText(tr("Programmer VDD") + FIELD_LABEL_SUFFIX);
  // currentRegulatorLevelLabel->setText(tr("VDD regulator set point") + FIELD_LABEL_SUFFIX);

  // [all-settings]
  settings_box->setTitle(tr("Settings"));
  accel_max_label->setText(tr("Max. acceleration:"));
  // ispFrequencyLabel->setText(tr("ISP Frequency") + FIELD_LABEL_SUFFIX);
  // maxIspFrequencyLabel->setText(tr("Max ISP Frequency") + FIELD_LABEL_SUFFIX);
  // regulatorModeLabel->setText(tr("Regulator mode") + FIELD_LABEL_SUFFIX);
  // vccOutputEnabledLabel->setText(tr("VCC output") + FIELD_LABEL_SUFFIX);
  // vccOutputIndicatorLabel->setText(tr("VCC output indicator") + FIELD_LABEL_SUFFIX);
  // lineAFunctionLabel->setText(tr("Line A function") + FIELD_LABEL_SUFFIX);
  // lineBFunctionLabel->setText(tr("Line B function") + FIELD_LABEL_SUFFIX);
  // vccVddMaxRangeLabel->setText(tr("VCC/VDD maximum range") + FIELD_LABEL_SUFFIX);
  // vcc3v3MinLabel->setText(tr("VCC 3.3 V minimum") + FIELD_LABEL_SUFFIX);
  // vcc3v3MaxLabel->setText(tr("VCC 3.3 V maximum") + FIELD_LABEL_SUFFIX);
  // vcc5vMinLabel->setText(tr("VCC 5 V minimum") + FIELD_LABEL_SUFFIX);
  // vcc5vMaxLabel->setText(tr("VCC 5 V maximum") + FIELD_LABEL_SUFFIX);
  // stk500HardwareVersionLabel->setText(tr("STK500 hardware version") + FIELD_LABEL_SUFFIX);
  // stk500SoftwareVersionLabel->setText(tr("STK500 software version") + FIELD_LABEL_SUFFIX);

  // cancelChangesButton->setText("Cancel Changes"); // TODO: use same name as menu item
  // defaultsButton->setText("Defaults"); // TODO: use same name as menu item
  apply_settings_button->setText(apply_settings_action->text());
}



void main_window::start_controller()
{
  assert(!connected());

  bool successfully_updated_list = try_update_device_list();
  if (!successfully_updated_list)
  {
    handle_model_changed();
    return;
  }

  if (device_list.size() > 0)
  {
    really_connect();
  }
  else
  {
    handle_model_changed();
  }
}

void main_window::connect_to_device()
{
  if (connected())
  {
    // todo: figure out what to do with multiple devices
    return;
  }
  
  bool successfully_updated_list = try_update_device_list();
  if (!successfully_updated_list)
  {
    return;
  }

  if (device_list.size() > 0)
  {
    really_connect();
  }
  else
  {
    show_error_message(
      "No Tic was found.  "
      "Please verify that the Tic is connected to the computer via USB."
      );
  }
}

void main_window::really_connect()
{
  assert(device_list.size() > 0);

  try
  {
    // Close the old handle in case one is already open.
    device_handle.close();

    connection_error = false;
    //disconnectedByUser = false;

    // Open a handle to the specified programmer.
    device_handle = tic::handle(device_list.at(0));

  }
  catch (const std::exception & e)
  {
    //setConnectionError("Failed to connect to device.");
    show_exception(e, "There was an error connecting to the device.");
    handle_model_changed();
    return;
  }

  /*try
  {
      model->reloadFirmwareVersionString();
  }
  catch (const std::exception & e)
  {
      showException(e, "There was an error getting the firmware version.");
  }*/

  try
  {
    settings = device_handle.get_settings();
  }
  catch (const std::exception & e)
  {
    show_exception(e, "There was an error loading settings from the device.");
  }

  /*try
  {
      model->reloadVariables();
  }
  catch (const std::exception & e)
  {
      showException(e, "There was an error getting the status of the device.");
  }*/

  handle_model_changed();
}

bool main_window::try_update_device_list()
{
  try
  {
    device_list = tic::list_connected_devices();
    return true;
  }
  catch (const std::exception & e)
  {
    // model->setConnectionError("Failed to get the list of devices.");
    show_exception(e, "There was an error getting the list of devices.");
    return false;
  }
}

void main_window::handle_model_changed()
{
  handle_device_changed();
  //handleVariablesChanged();
  handle_settings_changed();
}

void main_window::handle_device_changed()
{
  if (connected())
  {
    set_connection_status("Connected.", false);
  }
  else if (connection_error)
  {
    set_connection_status(connection_error_message, true);
  }
  else if (disconnected_by_user)
  {
    set_connection_status("Not connected.", false);
  }
  else
  {
    // This is a subtle way of saying that we are not connected but we will
    // auto-connect when we see a device available.
    set_connection_status("Not connected yet...", false);
  }
}

void main_window::handle_settings_changed()
{
  // [all-settings]
  set_accel_max(tic_settings_accel_max_get(settings.pointer_get()));
}

void main_window::handle_accel_max_input(uint32_t accel_max)
{
  if (!connected()) { return; }
  tic_settings_accel_max_set(settings.pointer_get(), accel_max);
  settings_modified = true;
  handle_settings_changed();
}

void main_window::apply_settings()
{
  if (!connected()) { return; }

  try
  {
    assert(connected());
    device_handle.set_settings(settings);
    device_handle.reinitialize();
    settings_modified = false;  // this must be last in case exceptions are thrown
  }
  catch (const std::exception & e)
  {
    show_exception(e, "There was an error applying settings.");
  }

  handle_settings_changed();
}
