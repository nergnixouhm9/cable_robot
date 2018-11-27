#ifndef CABLE_ROBOT_TYPES_H
#define CABLE_ROBOT_TYPES_H

#include <cmath>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief The MotorStatus struct
 */
struct MotorStatus
{
  MotorStatus() : motor_position(0), motor_speed(0), motor_torque(0), op_mode(0) {}
  MotorStatus(const int8_t _op_mode, const int32_t motor_pos, const int32_t _motor_speed,
              const int16_t _motor_torque)
    : motor_position(motor_pos), motor_speed(_motor_speed), motor_torque(_motor_torque),
      op_mode(_op_mode)
  {
  }

  int32_t motor_position;
  int32_t motor_speed;
  int16_t motor_torque;
  int8_t op_mode;
};

/**
 * @brief The WinchStatus struct
 */
struct WinchStatus : MotorStatus
{
  WinchStatus() : aux_position(0), cable_length(0.0) {}
  WinchStatus(const int8_t _op_mode, const int32_t motor_pos, const int32_t _motor_speed,
              const int16_t _motor_torque, const double cable_len, const int aux_pos)
    : MotorStatus(_op_mode, motor_pos, _motor_speed, _motor_torque),
      aux_position(aux_pos), cable_length(cable_len)
  {
  }

  int aux_position;
  double cable_length;
};

/**
 * @brief The ActuatorStatus struct
 */
struct ActuatorStatus : WinchStatus
{
  ActuatorStatus() : id(0), pulley_angle(0.0) {}
  ActuatorStatus(const int8_t _op_mode, const int32_t motor_pos,
                 const int32_t _motor_speed, const int16_t _motor_torque,
                 const double cable_len, const int aux_pos, const uint8_t _id,
                 const double pulley_ang)
    : WinchStatus(_op_mode, motor_pos, _motor_speed, _motor_torque, cable_len, aux_pos),
      id(_id), pulley_angle(pulley_ang)
  {
  }

  size_t id;
  double pulley_angle;
};

#endif // CABLE_ROBOT_TYPES_H
