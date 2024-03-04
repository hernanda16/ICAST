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
    uint8_t agent;
    float x;
    float y;
    float theta;
} pos_offset_t;

typedef struct
{
    uint8_t agent;
    float translation;
    float rotation;
} vel_scale_t;

typedef struct
{
    float x;
    float y;
    float theta;
} pos_t;

typedef struct
{
    float x;
    float y;
    float theta;
} vel_t;

typedef struct
{
    uint8_t is_caught;
    uint8_t is_visible;
    float x[5];
    float y[5];
    float vx[5];
    float vy[5];
} ball_t;

typedef struct
{
    float distance[144];
} laserscan_t;

//=============================================

typedef struct
{
    command_t command;
    pos_offset_t pos_offset;
    vel_scale_t vel_scale;
} agent0_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    laserscan_t laserscan;
} agent1_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    laserscan_t laserscan;
} agent2_t;

typedef struct
{
    pos_t pos;
    vel_t vel;
    ball_t ball;
    laserscan_t laserscan;
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
