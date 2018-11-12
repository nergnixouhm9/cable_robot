#ifndef HOMING_PROPRIOCEPTIVE_H
#define HOMING_PROPRIOCEPTIVE_H

#include <QObject>
#include <QString>

#include "StateMachine.h"
#include "robot/cablerobot.h"
#include "ctrl/controller_singledrive_naive.h"

class HomingProprioceptiveStartData: public EventData
{
public:
  HomingProprioceptiveStartData();
  HomingProprioceptiveStartData(const vect<qint16>& _init_torques, const quint8 _num_meas);

  vect<qint16> init_torques;
  quint8 num_meas;
};

class HomingProprioceptive : public QObject, public StateMachine
{
  Q_OBJECT

public:
  explicit HomingProprioceptive(QObject* parent, CableRobot* robot);

  enum States : BYTE
  {
    ST_IDLE,
    ST_ENABLED,
    ST_START_UP,
    ST_COILING,
    ST_UNCOILING,
    ST_SWITCH_CABLE,
    ST_OPTIMIZING,
    ST_GO_HOME,
    ST_FAULT,
    ST_MAX_STATES
  };

  bool IsCollectingData();

public:
  void Start(HomingProprioceptiveStartData* data);
  void Stop();
  void Next();
  void Optimize();
  void FaultTrigger();
  void FaultReset();

signals:
  void printToQConsole(const QString&) const;
  void acquisitionComplete() const;

private:
  static constexpr quint8 kNumMeasMin = 1U;

  CableRobot* robot_ = NULL;
  ControllerSingleDriveNaive controller_;
  quint8 num_meas_ = kNumMeasMin;

private:
  // clang-format off
  static constexpr char* kStatesStr[] = {
    const_cast<char*>("IDLE"),
    const_cast<char*>("ENABLED"),
    const_cast<char*>("START_UP"),
    const_cast<char*>("COILING"),
    const_cast<char*>("UNCOILING"),
    const_cast<char*>("SWITCH_CABLE"),
    const_cast<char*>("OPTIMIZING"),
    const_cast<char*>("GO_HOME"),
    const_cast<char*>("FAULT")};
  // clang-format on

  States prev_state_;

  // Define the state machine state functions with event data type
  STATE_DECLARE(HomingProprioceptive, Idle, NoEventData)
  GUARD_DECLARE(HomingProprioceptive, GuardEnabled, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Enabled, NoEventData)
  STATE_DECLARE(HomingProprioceptive, StartUp, HomingProprioceptiveStartData)
  GUARD_DECLARE(HomingProprioceptive, GuardCoiling, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Coiling, NoEventData)
  EXIT_DECLARE(HomingProprioceptive, CollectMeas)
  GUARD_DECLARE(HomingProprioceptive, GuardUncoiling, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Uncoiling, NoEventData)
  GUARD_DECLARE(HomingProprioceptive, GuardSwitch, NoEventData)
  STATE_DECLARE(HomingProprioceptive, SwitchCable, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Optimizing, NoEventData)
  STATE_DECLARE(HomingProprioceptive, GoHome, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Fault, NoEventData)

  // State map to define state object order. Each state map entry defines a state object.
  BEGIN_STATE_MAP_EX
  // clang-format off
    STATE_MAP_ENTRY_EX(&Idle)
    STATE_MAP_ENTRY_ALL_EX(&Enabled, &GuardEnabled, 0, 0)
    STATE_MAP_ENTRY_EX(&StartUp)
    STATE_MAP_ENTRY_ALL_EX(&Coiling, &GuardCoiling, 0, &CollectMeas)
    STATE_MAP_ENTRY_ALL_EX(&Uncoiling, &GuardUncoiling, 0, &CollectMeas)
    STATE_MAP_ENTRY_ALL_EX(&SwitchCable, &GuardSwitch, 0, 0)
    STATE_MAP_ENTRY_EX(&Optimizing)
    STATE_MAP_ENTRY_EX(&GoHome)
    STATE_MAP_ENTRY_EX(&Fault)
  // clang-format on
  END_STATE_MAP_EX

  void PrintStateTransition(const States current_state, const States new_state) const;
};

#endif // HOMING_PROPRIOCEPTIVE_H
