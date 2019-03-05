#ifndef CABLE_ROBOT_HOMING_PROPRIOCEPTIVE_H
#define CABLE_ROBOT_HOMING_PROPRIOCEPTIVE_H

#include <QObject>
#include <QString>

#include "json.hpp"

#include "StateMachine.h"
#include "easylogging++.h"
#include "inc/filters.h"

#include "ctrl/controller_singledrive.h"
#include "homing/matlab_thread.h"
#include "robot/cablerobot.h"
#include "utils/types.h"


// Aliases -----------------------------------------
using json = nlohmann::json; // JSON library support

class HomingProprioceptiveStartData: public EventData
{
 public:
  HomingProprioceptiveStartData();
  HomingProprioceptiveStartData(const vect<qint16>& _init_torques,
                                const vect<qint16>& _max_torques, const quint8 _num_meas);

  vect<qint16> init_torques;
  vect<qint16> max_torques;
  quint8 num_meas;
};

std::ostream& operator<<(std::ostream& stream, const HomingProprioceptiveStartData& data);


class HomingProprioceptiveHomeData: public EventData
{
 public:
  vect<double> init_lengths;
  vect<double> init_angles;
};

std::ostream& operator<<(std::ostream& stream, const HomingProprioceptiveHomeData& data);


class HomingProprioceptive: public QObject, public StateMachine
{
  Q_OBJECT

 public:
  HomingProprioceptive(QObject* parent, CableRobot* robot);
  ~HomingProprioceptive();

  enum States : BYTE
  {
    ST_IDLE,
    ST_ENABLED,
    ST_START_UP,
    ST_SWITCH_CABLE,
    ST_COILING,
    ST_UNCOILING,
    ST_OPTIMIZING,
    ST_HOME,
    ST_FAULT,
    ST_MAX_STATES
  };

  bool IsCollectingData();

  vect<id_t> GetActuatorsID() const { return active_actuators_id_; }
  ActuatorStatus GetActuatorStatus(const id_t id);

  bool ParseExtFile(const QString& filepath, HomingProprioceptiveHomeData*);

 public:
  //--------- External events -------------------------------------------------------//

  void Start(HomingProprioceptiveStartData* data);
  void Stop();
  void Disable();
  void Optimize();
  void GoHome(HomingProprioceptiveHomeData* data);
  void FaultTrigger();
  void FaultReset();

 signals:
  void printToQConsole(const QString&) const;
  void acquisitionComplete() const;
  void homingComplete() const;
  void stateChanged(const quint8&) const;
  void progressValue(const int&) const;
  void stopWaitingCmd() const;

 private slots:
  void handleActuatorStatusUpdate(const ActuatorStatus& actuator_status);
  void handleMatlabResultsReady();
  void updateOptimizationProgress();

 private:
  CableRobot* robot_ptr_ = NULL;
  ControllerSingleDrive controller_;

  static constexpr size_t kNumMeasMin_     = 1;
  static constexpr qint16 kTorqueSsErrTol_ = 5;
  size_t num_meas_                         = kNumMeasMin_;
  size_t num_tot_meas_;
  size_t working_actuator_idx_;
  size_t meas_step_;
  vect<qint16> init_torques_;
  vect<qint16> max_torques_;
  vect<qint16> torques_;
  vect<qint32> reg_pos_;

  bool stop_cmd_recv_;
  bool disable_cmd_recv_;

  QMutex qmutex_;

  vect<id_t> active_actuators_id_;
  vect<ActuatorStatus> actuators_status_;

  static constexpr int kOptProgressIntervalMsec_ = 150;
  QTimer optimization_progess_timer_;
  int optimization_progress_counter_;

  // Tuning params for detecting platform steadyness
  static constexpr double kBufferingTimeSec_  = 3.0;     // [sec]
  static constexpr double kCutoffFreq_        = 20.0;    // [Hz]
  static constexpr double kMaxAngleDeviation_ = 0.00005; // [rad]

  RetVal WaitUntilPlatformSteady();

  void DumpMeasAndMoveNext();

 private:
  //--------- State machine ---------------------------------------------------------//

  // clang-format off
  static constexpr char* kStatesStr[] = {
    const_cast<char*>("IDLE"),
    const_cast<char*>("ENABLED"),
    const_cast<char*>("START_UP"),
    const_cast<char*>("SWITCH_CABLE"),
    const_cast<char*>("COILING"),
    const_cast<char*>("UNCOILING"),
    const_cast<char*>("OPTIMIZING"),
    const_cast<char*>("GO_HOME"),
    const_cast<char*>("FAULT")};
  // clang-format on

  States prev_state_;

  // Define the state machine state functions with event data type
  GUARD_DECLARE(HomingProprioceptive, GuardIdle, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Idle, NoEventData)
  GUARD_DECLARE(HomingProprioceptive, GuardEnabled, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Enabled, NoEventData)
  STATE_DECLARE(HomingProprioceptive, StartUp, HomingProprioceptiveStartData)
  GUARD_DECLARE(HomingProprioceptive, GuardSwitch, NoEventData)
  STATE_DECLARE(HomingProprioceptive, SwitchCable, NoEventData)
  ENTRY_DECLARE(HomingProprioceptive, EntryCoiling, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Coiling, NoEventData)
  ENTRY_DECLARE(HomingProprioceptive, EntryUncoiling, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Uncoiling, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Optimizing, NoEventData)
  STATE_DECLARE(HomingProprioceptive, Home, HomingProprioceptiveHomeData)
  STATE_DECLARE(HomingProprioceptive, Fault, NoEventData)

  // State map to define state object order
  BEGIN_STATE_MAP_EX
  // clang-format off
    STATE_MAP_ENTRY_ALL_EX(&Idle, &GuardIdle, 0, 0)
    STATE_MAP_ENTRY_ALL_EX(&Enabled, &GuardEnabled, 0, 0)
    STATE_MAP_ENTRY_EX(&StartUp)
    STATE_MAP_ENTRY_ALL_EX(&SwitchCable, &GuardSwitch, 0, 0)
    STATE_MAP_ENTRY_ALL_EX(&Coiling, 0, &EntryCoiling, 0)
    STATE_MAP_ENTRY_ALL_EX(&Uncoiling, 0, &EntryUncoiling, 0)
    STATE_MAP_ENTRY_EX(&Optimizing)
    STATE_MAP_ENTRY_EX(&Home)
    STATE_MAP_ENTRY_EX(&Fault)
  // clang-format on
  END_STATE_MAP_EX

  void PrintStateTransition(const States current_state, const States new_state) const;
};

#endif // CABLE_ROBOT_HOMING_PROPRIOCEPTIVE_H
