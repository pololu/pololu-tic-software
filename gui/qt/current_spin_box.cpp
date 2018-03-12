// NOTE: One thing missing from this is better handling of what code will
// be selected when multiple codes map to the same current (selecting the
// smallest would be good).
//
// NOTE: Another big thing missing from this is an option to use units of amps
// instead of milliamps, and to allow the user to enter a current limit in
// either units even if we have a suffix like " mA" or " A" set.

#include "current_spin_box.h"

#include <QSpinBox>

current_spin_box::current_spin_box(QWidget * parent)
  : QSpinBox(parent)
{
  // We make this connection so that as the user types, the code member gets
  // updated.  So higher-level code using this object can listen to the
  // valueChanged event and then call either 'get_code' or 'getValue' depending
  // on what type of number they want.
  //
  // We rely on Qt's documented behavior that if multiple slots are connected to
  // one signal, they are called in the order they were connected.
  //
  // Warning: Don't use a QSignalBlocker with this class unless you really know
  // what you're doing, since you'll block this signal.
  connect(this, QOverload<int>::of(&valueChanged),
    this, &set_code_from_value);

  connect(this, &editingFinished, this, &editing_finished);
}

void current_spin_box::set_mapping(const QMap<int, int> & mapping)
{
  this->mapping = mapping;
  fix_code_if_not_allowed();
  set_value_from_code();
}

// Ensures that 'code' is one of the allowed codes, or -1 if there are no
// allowed codes.  Note that this is not as smart as it could be because it
// doesn't take into account the value corresponding to a code that is now
// disallowed, but in our applications we anticipate the caller settings the
// value explcitly to something if that ever happens.
void current_spin_box::fix_code_if_not_allowed()
{
  if (mapping.contains(code))
  {
    return;
  }

  // Find the minimum current, or -1 if the mapping is empty.
  int best_code = -1;
  int best_current = std::numeric_limits<int>::max();
  for (int candidate_code : mapping.keys())
  {
    int current = mapping.value(candidate_code);
    if (current < best_current)
    {
      best_code = candidate_code;
      best_current = current;
    }
  }
  code = best_code;
}

// Finds the code for the highest current less than the current currently
// displayed in the box, and set 'code' to that.  Sets 'code' to -1 if the
// mapping is empty.
void current_spin_box::set_code_from_value()
{
  int current_from_value = value();

  int best_code = -1;
  int best_current = -1;
  for (int candidate_code : mapping.keys())
  {
    int current = mapping.value(candidate_code);
    if (best_code == -1 || (current <= current_from_value && current > best_current))
    {
      best_code = candidate_code;
      best_current = current;
    }
  }
  code = best_code;
}

void current_spin_box::set_value_from_code()
{
  setValue(mapping.value(code, 0));
}

// When the user is done editing, we want to figure out the right code to
// use and update the spinbox to show the corresponding current.
void current_spin_box::editing_finished()
{
  set_code_from_value();
  set_value_from_code();
}

void current_spin_box::stepBy(int step_value)
{
  int current = value();

  while (step_value > 0)
  {
    int best_code = -1;
    int best_current = std::numeric_limits<int>::max();
    for (int candidate_code : mapping.keys())
    {
      int candidate_current = mapping.value(candidate_code);
      if (candidate_current > current && candidate_current < best_current)
      {
        best_code = candidate_code;
        best_current = candidate_current;
      }
    }
    if (best_code == -1)
    {
      break;  // Reached the max current.
    }
    code = best_code;
    step_value--;
  }

  while (step_value < 0)
  {
    int best_code = -1;
    int best_current = -1;
    for (int candidate_code : mapping.keys())
    {
      int candidate_current = mapping.value(candidate_code);
      if (candidate_current < current && candidate_current > best_current)
      {
        best_code = candidate_code;
        best_current = candidate_current;
      }
    }
    if (best_code == -1)
    {
      break;  // Reached the min current.
    }
    code = best_code;
    step_value++;
  }

  set_value_from_code();

  selectAll();
}
