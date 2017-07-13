#include "InputWizard.h"

#include <QIcon>
#include <QLabel>
#include <QVboxLayout>

InputWizard::InputWizard(QWidget * parent)
  : QWizard(parent)
{
  addPage(setup_intro_page());
  
  setWindowTitle(tr("Input Setup Wizard"));
  setWindowIcon(QIcon(":app_icon"));
  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

QWizardPage * InputWizard::setup_intro_page()
{
  QWizardPage * page = new QWizardPage;
  QVBoxLayout * layout = new QVBoxLayout;
  
  page->setTitle(tr("Input Setup Wizard"));
  
  QLabel * intro_label = new QLabel(tr(
    "This wizard will help you quickly set the scaling parameters for the "
    "Tic's analog or RC input."));
  intro_label->setWordWrap(true);
  
  QLabel * deenergized_label = new QLabel(tr(
    "NOTE: Your motor has been automatically de-energized so that it does not "
    "cause problems while you are using this wizard.  To energize it manually "
    "later, you can click the \"Resume\" button (after fixing any errors)."));
  deenergized_label->setWordWrap(true);
    
  layout->addWidget(intro_label);
  layout->addStretch(1);
  layout->addWidget(deenergized_label);
  
  page->setLayout(layout);
  return page;
}