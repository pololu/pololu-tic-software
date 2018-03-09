// NOTE: One thing missing from this is better handling of what code will
// be selected when multiple codes map to the same current (selecting the
// smallest would be good).
//
// NOTE: Another big thing missing from this is an option to use units of amps
// instead of milliamps, and to allow the user to enter a current limit in
// either units even if we have a suffix like " mA" or " A" set.

#pragma once

#include <QSpinBox>

class current_spin_box : public QSpinBox
{
  Q_OBJECT

public:
  current_spin_box(QWidget * parent = Q_NULLPTR);

  // Set the mapping of allowed codes to current values.
  void set_mapping(const QMap<int, int> &);

  int get_code() { return code; }

private slots:
  void editing_finished();

private:
  void fix_code_if_not_allowed();
  void set_code_from_value();
  void set_value_from_code();

  QMap<int, int> mapping;
  int code = -1;

protected:
  virtual void stepBy(int step_value);
};

