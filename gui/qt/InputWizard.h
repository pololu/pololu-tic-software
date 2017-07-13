#pragma once

#include <QWizard>

class InputWizard : public QWizard
{
  Q_OBJECT
  
public:
  InputWizard(QWidget * parent = 0);

private:
  QWizardPage * setup_intro_page();
};
