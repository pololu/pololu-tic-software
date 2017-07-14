#pragma once

#include <QWizard>

class QLabel;
class QProgressBar;

enum wizard_page { INTRO, LEARN, CONCLUSION };
enum wizard_learn_step { NEUTRAL, MAX, MIN };

class LearnPage : public QWizardPage
{
  Q_OBJECT

public:
  LearnPage(QWidget * parent = 0);

private:
  QLayout * setup_input_status_layout();

  QLabel * instruction_label;
  QLabel * input_status_label;
  QLabel * input_status_value;
  QLabel * input_status_pretty;
  QLabel * sampling_label;
  QProgressBar * sampling_progress;

  QString input_pin_name = tr("Invalid");

  int step = NEUTRAL;
  void set_text_from_step();

  friend class InputWizard;
};

class InputWizard : public QWizard
{
  Q_OBJECT

public:
  InputWizard(QWidget * parent = 0);

  void set_control_mode(uint8_t control_mode);

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
};
