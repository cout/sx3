/*
** physics.h
**
**   Physcis module header file
**
*/

#ifndef PHYSICS_H
#define PHYSICS_H

#include <matrix.h>
#include "pglobal.h"

struct Physical_Properties {
    float          mass;                               // in kg
    float          radius;                             // in meters
    float          surface_area;                       // in m^2
    Vector         position;                           // in meters
    Vector         velocity;                           // in m/s
    Vector         angular_position;                   // in radians
    Vector         angular_velocity;                   // in rads/s
    float          moment_coefficient;
    float          friction_coefficient;
    float          bounce_coefficient;
    int            can_roll;                           // T/F
    int            growth_direction;                   // +/-
};

enum Object_State {
    STATE_PROJECTILE,
    STATE_PROJECTILE_FINAL,
    STATE_ROLLING,
    STATE_ROLLING_FINAL,
    STATE_IMPACTED,

    NUM_OBJECT_STATES
};

struct Object {
    struct Physical_Properties props;
    Vector                     propelling_force;       // in Newtons
    float                      propelling_time;        // in seconds
    enum Object_State          state;
};

float next_object_state(struct Object* o, float t);
void set_terrain_height_func(float(*f)(float,float));

#endif
