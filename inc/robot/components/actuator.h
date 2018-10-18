#ifndef CABLE_ROBOT_ACTUATOR_H
#define CABLE_ROBOT_ACTUATOR_H

#include "state_machine/inc/StateMachine.h"
#include "libgrabrt/inc/clocks.h"

#include "winch.h"
#include "pulleys_system.h"

/**
 * @brief The Actuator class
 */
class Actuator : public StateMachine
{
public:
  /**
   * @brief Actuator
   * @param slave_position
   */
  Actuator(const uint8_t slave_position, ActuatorParams* const params);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //// External events
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /**
   * @brief Enable
   */
  void Enable();
  /**
   * @brief Disable
   */
  void Disable();
  /**
   * @brief FaultReset
   */
  void FaultReset();

  /**
   * @brief GetWinch
   * @return
   */
  const Winch* GetWinch() const { return &winch_; }
  /**
   * @brief GetPulley
   * @return
   */
  const PulleysSystem* GetPulley() const { return &pulley_; }

  /**
   * @brief SetCableLength
   * @param target_length
   */
  void SetCableLength(const double target_length);
  /**
   * @brief SetCableSpeed
   * @param target_speed
   */
  void SetCableSpeed(const int32_t target_speed);
  /**
   * @brief SetCableTorque
   * @param target_torque
   */
  void SetCableTorque(const int16_t target_torque);

  /**
   * @brief UpdateHomeConfig
   * @param cable_len
   * @param cable_len_true
   * @param pulley_angle
   */
  void UpdateHomeConfig(const double cable_len, const double cable_len_true,
                        const double pulley_angle);
  /**
   * @brief UpdateStartConfig
   */
  void UpdateStartConfig();
  /**
   * @brief UpdateConfig
   */
  void UpdateConfig();

  /**
   * @brief IsIdle
   * @return
   */
  bool IsIdle() { return GetCurrentState() == ST_IDLE; }
  /**
   * @brief IsEnabled
   * @return
   */
  bool IsEnabled() { return GetCurrentState() == ST_ENABLED; }
  /**
   * @brief IsInFault
   * @return
   */
  bool IsInFault() { return GetCurrentState() == ST_FAULT; }

private:
  static constexpr double kMaxTransitionTimeSec_ = 0.010;
  // clang-format off
  static constexpr char* kStatesStr_[] = {
    const_cast<char*>("IDLE"),
    const_cast<char*>("ENABLED"),
    const_cast<char*>("FAULT"),
    const_cast<char*>("MAX_STATE")
  };
  // clang-format on

  enum States : BYTE
  {
    ST_IDLE,
    ST_ENABLED,
    ST_FAULT,
    ST_MAX_STATES
  };

  uint8_t slave_position_;

  Winch winch_;
  PulleysSystem pulley_;

  grabrt::ThreadClock clock_;
  States prev_state_ = ST_IDLE;

  // Define the state machine state functions with event data type
  STATE_DECLARE(Actuator, Idle, NoEventData)
  GUARD_DECLARE(Actuator, GuardIdle, NoEventData)
  STATE_DECLARE(Actuator, Enabled, NoEventData)
  GUARD_DECLARE(Actuator, GuardEnabled, NoEventData)
  STATE_DECLARE(Actuator, Fault, NoEventData)
  GUARD_DECLARE(Actuator, GuardFault, NoEventData)

  // State map to define state object order. Each state map entry defines a state object.
  BEGIN_STATE_MAP_EX
  // clang-format off
    STATE_MAP_ENTRY_ALL_EX(&Idle, &GuardIdle, 0, 0)
    STATE_MAP_ENTRY_ALL_EX(&Enabled, &GuardEnabled, 0, 0)
    STATE_MAP_ENTRY_ALL_EX(&Fault, &GuardFault, 0, 0)
  // clang-format on
  END_STATE_MAP_EX

  void PrintStateTransition(const States current_state) const;
};

#endif // CABLE_ROBOT_ACTUATOR_H