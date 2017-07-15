#pragma once

#include <QWizard>

class LearnPage;
class QLabel;
class QProgressBar;

class main_window;
class main_controller;

enum wizard_page { INTRO, LEARN, CONCLUSION };
enum wizard_learn_step { NEUTRAL, MAX, MIN };

class InputWizard : public QWizard
{
  Q_OBJECT

public:
  InputWizard(main_window * window);

  void set_controller(main_controller * controller);

  void set_control_mode(uint8_t control_mode);

  void set_input(uint16_t input);

private slots:
  void on_currentIdChanged(int id);

private:
  bool suppress_events = false;

  QWizardPage * setup_intro_page();
  QWizardPage * setup_learn_page();
  QWizardPage * setup_conclusion_page();

  QLabel * intro_label;
  LearnPage * learn_page;

  uint8_t control_mode;
  QString control_mode_name();
  QString input_pin_name();
  void set_text_from_control_mode();

  main_window * window;
  main_controller * controller;
};

class LearnPage : public QWizardPage
{
  Q_OBJECT

public:
  LearnPage(QWidget * parent = 0);

protected:
  bool isComplete() const override;

private:
  void set_next_button_enabled(bool enabled);
  void set_progress_visible(bool visible);
  void set_text_from_step();

  bool handle_back();
  bool handle_next();

  bool sampling = false;
  bool enable_next_button = true;
  QString input_pin_name = tr("Invalid");
  int step = NEUTRAL;

  QLayout * setup_input_layout();

  QLabel * instruction_label;
  QLabel * input_label;
  QLabel * input_value;
  QLabel * input_pretty;
  QLabel * sampling_label;
  QProgressBar * sampling_progress;

  friend class InputWizard;
};