#include "InputWizard.h"

#include "tic.hpp"
#include "main_window.h" // fixme remove

#include <QChar>
#include <QIcon>
#include <QHboxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QVboxLayout>

#ifdef __APPLE__
#define NEXT_BUTTON_TEXT tr("Continue")
#define FINISH_BUTTON_TEXT tr("Done")
#else
#define NEXT_BUTTON_TEXT tr("Next")
#define FINISH_BUTTON_TEXT tr("Finish")
#endif

LearnPage::LearnPage(QWidget * parent)
  : QWizardPage(parent)
{
  QVBoxLayout * layout = new QVBoxLayout();

  instruction_label = new QLabel();
  instruction_label->setWordWrap(true);
  instruction_label->setAlignment(Qt::AlignTop);
  instruction_label->setMinimumHeight(fontMetrics().lineSpacing() * 2);
  layout->addWidget(instruction_label);
  layout->addSpacing(fontMetrics().height());

  layout->addLayout(setup_input_status_layout());
  layout->addSpacing(fontMetrics().height());

  QLabel * next_label = new QLabel(
    tr("When you click ") + NEXT_BUTTON_TEXT +
    tr(", this wizard will sample the input values for one second.  "
    "Please do not change the input while it is being sampled."));
  next_label->setWordWrap(true);
  layout->addWidget(next_label);
  layout->addSpacing(fontMetrics().height());

  sampling_label = new QLabel(tr("Sampling..."));
  layout->addWidget(sampling_label);

  sampling_progress = new QProgressBar();
  layout->addWidget(sampling_progress);

  setLayout(layout);
}

void LearnPage::set_text_from_step()
{
  switch (step)
  {
  case NEUTRAL:
    setTitle(tr("Step 1 of 3: Neutral"));
    instruction_label->setText(
      tr("Verify that you have connected your input to the ") + input_pin_name +
      tr(" pin.  Next, move the input to its neutral position."));
    break;

  case MAX:
    setTitle(tr("Step 2 of 3: Maximum"));
    instruction_label->setText(
      tr("Move the input to its maximum (full forward) position."));
    break;

  case MIN:
    setTitle(tr("Step 3 of 3: Minimum"));
    instruction_label->setText(
      tr("Move the input to its minimum (full reverse) position."));
    break;
  }
}

QLayout * LearnPage::setup_input_status_layout()
{
  QHBoxLayout * layout = new QHBoxLayout();

  input_status_label = new QLabel();
  layout->addWidget(input_status_label);

  input_status_value = new QLabel();
  layout->addWidget(input_status_value);

  input_status_pretty = new QLabel();
  layout->addWidget(input_status_pretty);

  return layout;
}

InputWizard::InputWizard(QWidget * parent)
  : QWizard(parent)
{
  setPage(INTRO, setup_intro_page());
  setPage(LEARN, setup_learn_page());
  setPage(CONCLUSION, setup_conclusion_page());

  setWindowTitle(tr("Input Setup Wizard"));
  setWindowIcon(QIcon(":app_icon"));
  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  // todo: maybe set Qt::MSWindowsFixedSizeDialogHint? it gets rid of unwanted resize handles at least on left, right, and bottom edges

  setFixedSize(sizeHint());

  connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(on_currentIdChanged(int)));
}

void InputWizard::set_control_mode(uint8_t control_mode)
{
  this->control_mode = control_mode;
  set_text_from_control_mode();
}

void InputWizard::on_currentIdChanged(int id)
{
  if (suppress_events) { return; }

  if ((id == INTRO) && (learn_page->step > NEUTRAL))
  {
    // User clicked Back from learn page, but was on a step > 1. Return to the
    // learn page and decrement the step.
    suppress_events = true;
    next();
    suppress_events = false;
    learn_page->step--;
    learn_page->set_text_from_step();
  }
  else if ((id == CONCLUSION) && (learn_page->step < MIN))
  {
    // User clicked Next from learn page, but was on a step < 3. Return to the
    // learn page and increment the step.
    suppress_events = true;
    back();
    suppress_events = false;
    learn_page->step++;
    learn_page->set_text_from_step();
  }
}

QString InputWizard::control_mode_name()
{
  switch (control_mode)
  {
  case TIC_CONTROL_MODE_RC_POSITION:
  case TIC_CONTROL_MODE_RC_SPEED:
    return tr("RC");

  case TIC_CONTROL_MODE_ANALOG_POSITION:
  case TIC_CONTROL_MODE_ANALOG_SPEED:
    return tr("analog");

  default:
    return tr("(Invalid)");
  }
}

QString InputWizard::input_pin_name()
{
  switch (control_mode)
  {
  case TIC_CONTROL_MODE_RC_POSITION:
  case TIC_CONTROL_MODE_RC_SPEED:
    return "RC";

  case TIC_CONTROL_MODE_ANALOG_POSITION:
  case TIC_CONTROL_MODE_ANALOG_SPEED:
    return u8"SDA\u200A/\u200AAN";

  default:
    return tr("(Invalid)");
  }
}

static QString capitalize(QString const & str)
{
  QString new_str = str;
  new_str[0] = new_str[0].toTitleCase();
  return new_str;
}

void InputWizard::set_text_from_control_mode()
{
  intro_label->setText(
    tr("This wizard will help you quickly set the scaling parameters for the Tic's ") +
    control_mode_name() + tr(" input."));

  learn_page->input_status_label->setText(capitalize(control_mode_name()) + tr(" input:"));

  learn_page->input_pin_name = input_pin_name();
  learn_page->set_text_from_step();
}

QWizardPage * InputWizard::setup_intro_page()
{
  QWizardPage * page = new QWizardPage();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(tr("Input setup wizard"));

  intro_label = new QLabel();
  intro_label->setWordWrap(true);
  layout->addWidget(intro_label);
  layout->addStretch(1);

  QLabel * deenergized_label = new QLabel(
    tr("NOTE: Your motor has been automatically de-energized so that it does not "
    "cause problems while you are using this wizard.  To energize it manually "
    "later, you can click the \"Resume\" button (after fixing any errors)."));
  deenergized_label->setWordWrap(true);
  layout->addWidget(deenergized_label);

  page->setLayout(layout);
  return page;
}

QWizardPage * InputWizard::setup_learn_page()
{
  learn_page = new LearnPage();
  return learn_page;
}

QWizardPage * InputWizard::setup_conclusion_page()
{
  QWizardPage * page = new QWizardPage();
  QVBoxLayout * layout = new QVBoxLayout();

  page->setTitle(tr("Input setup finished"));

  QLabel * completed_label = new QLabel(
    tr("You have successfully completed this wizard.  You can see your new "
    "settings on the \"Input and motor settings\" tab after you click ") +
    FINISH_BUTTON_TEXT + tr(".  "
    "To use the new settings, you must first apply them to the device."));
  completed_label->setWordWrap(true);
  layout->addWidget(completed_label);

  page->setLayout(layout);
  return page;
}