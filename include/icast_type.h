#ifndef ICAST_TYPE_H
#define ICAST_TYPE_H

#include "stdint.h"

#pragma pack(push, 1)

typedef struct
{
    uint8_t data;
} command_t;

typedef struct
{
    uint8_t data;
} mode_base_t;

typedef struct
{
    uint8_t data;
} style_t;

typedef struct
{
    int16_t x;
    int16_t y;
    float theta;
} target_manual_t;

typedef struct
{
    int16_t x;
    int16_t y;
    float theta;
} offset_robot_t;

typedef struct
{
    uint8_t mux_1;
    uint8_t mux_2;
    uint8_t mux_control;
} data_mux_t;

typedef struct
{
    uint8_t translation_vel[5];
    uint8_t rotation_vel[5];
    uint8_t kick_power[5];
} trim_t;

typedef struct
{
    uint8_t data;
} pass_counter_t;

typedef struct
{
    int16_t x;
    int16_t y;
    float theta;
} pos_t;

typedef struct
{
    int16_t x;
    int16_t y;
    float theta;
} vel_t;

typedef struct
{
    uint8_t is_caught;
    uint8_t is_visible;
    int16_t x[5];
    int16_t y[5];
    int16_t vx[5];
    int16_t vy[5];
} ball_t;

typedef struct
{
    int16_t robot_condition;
    int16_t fsm_0;
    int16_t fsm_1;
    int16_t fsm_2;
} state_machine_t;

typedef struct
{
    uint8_t data;
    int16_t target_x;
    int16_t target_y;
} passing_t;

typedef struct
{
    int16_t pcl_x[180];
    int16_t pcl_y[180];
} obstacle_t;

typedef struct
{
    int16_t voltage;
} battery_t;

typedef struct
{
    int16_t target_x;
    int16_t target_y;
} keeper_on_field_t;

typedef struct
{
    int16_t ball_x;
    int16_t ball_y;
} prediction_t;

typedef struct
{
    uint8_t data;
} epoch_t;

//=============================================

typedef struct
{
    command_t command;
    mode_base_t mode_base;
    style_t style;
    target_manual_t target_manual;
    offset_robot_t offset_robot;
    data_mux_t data_mux;
    trim_t trim;
    pass_counter_t pass_counter;
} agent0_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    state_machine_t state_machine;
    passing_t passing;
    obstacle_t obstacle;
    battery_t battery;
    keeper_on_field_t keeper_on_field;
    prediction_t prediction;
    epoch_t epoch;
} agent1_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    state_machine_t state_machine;
    passing_t passing;
    obstacle_t obstacle;
    battery_t battery;
    keeper_on_field_t keeper_on_field;
    prediction_t prediction;
    epoch_t epoch;
} agent2_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    state_machine_t state_machine;
    passing_t passing;
    obstacle_t obstacle;
    battery_t battery;
    keeper_on_field_t keeper_on_field;
    prediction_t prediction;
    epoch_t epoch;
} agent3_t;

//=============================================

typedef struct
{
    agent0_t agent0;
    agent1_t agent1;
    agent2_t agent2;
    agent3_t agent3;
} icast_bus_t;

#pragma pack(pop)
#endif
