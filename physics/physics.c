// Physics functions
// Please use a tab size of 4 when reading this file.

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <matrix.h>
#include "physics.h"
#include "zeroin.h"

#ifndef M_PI
#define M_PI 3.141592653579323843383
#endif

// Function prototypes
static void net_force_projectile(pVector f, const struct Object* o, float t);
static void frictional_force(pVector f, float c1, float c2, const pVector n);
static void gravitational_force(pVector f, const struct Object* o);
static void propulsion_force(pVector f, const struct Object *o, float t);
static void air_resistance_force(pVector f, const pVector F0, const struct Object* o, float t);
static float zero_terrain_height(float x, float z);

// Globals
static float(*terrain_height)(float,float) = zero_terrain_height;

// set_terrain_height_func sets the function used to calculate the height of
// the terrain.  set_terrain_height_func takes a function pointer as an
// argument and returns nothing.
void set_terrain_height_func(float(*f)(float,float)) {
    terrain_height = f;
}

// a terrain_height_func returns the height of the terrain at point x,z.
// zero_terrain_height returns 0 for all points (uniform terrain).
static float zero_terrain_height(float x, float z) {
    return 0.0;
}

// terrain_delta calculates the distance between the location of
// an object at time t and the height of the terrain at the object's
// x,z coordinates.  Uses Brent's zeroin function in combination with
// this function to determine at what time (and thus at what
// coordinates) a projectile impacted with the ground.
// Before calling this function, initialize an Object struct named
// base_object, which represents the object at time 0, at some
// location before the impact.
// THIS FUNCTION IS NOT THREAD-SAFE.
struct Object base_object;
float terrain_delta(float t) {
    struct Object o;
    memcpy(&o, &base_object, sizeof(struct Object));
    next_object_state(&o, t);
    return o.props.position[1] - (*terrain_height)(o.props.position[0],
        o.props.position[2]);
}

// next_object_state calculates the next point that an object will be at.
// o is a pointer to the object.  o->props is updated to reflect the
// new position and other new properties of the object.
// d is a delta value specifying how coarse the calculations are to
// be.  Smaller values mean more accurate calculations (smaller
// differences between points), and larger values mean less CPU
// usage.  It should be possible for the driver to vary this
// depending on how fast the computer is that the program is running
// on.
// Return value is the actual amount of time elapsed.
float next_object_state(struct Object* o, float t) {
    Vector tmp, position, velocity;
    struct Physical_Properties *p = &o->props;
    float non_propelled_time;

    switch(o->state) {
    case STATE_PROJECTILE:
    case STATE_PROJECTILE_FINAL:
        non_propelled_time = 0.0;

        // check for a propelling force
        // if the propelling force is still active (i.e. propelling_time > 0),
        // then we must check to see if the entire session here will be
        // propelled.  If the propellant will run out before we are done
        // here, then this function must be executed in two pieces, one
        // propelled, and one non-propelled, below.
        if(o->propelling_time > 0.0 && t > o->propelling_time) {
            non_propelled_time = t - o->propelling_time;
            t = o->propelling_time;
        }

        // start by calculating the force on the projectile
        net_force_projectile(tmp, o, t);       // tmp = F
        vc_div(tmp, p->mass);                  // tmp = F/m = a

        // find the new position -- this could be combined with the velocity
        // calculation for more efficiency
        vc_mul(tmp, t);                        // tmp = a*t
        vv_cpy(velocity, p->velocity);         // velocity = v0
        vv_add(velocity, tmp);                 // velocity = v0 + a*t
        vc_mul(tmp, 0.5);                      // tmp = 0.5*a*t
        vv_add(tmp, p->velocity);              // tmp = v0 + 0.5*a*t
        vc_mul(tmp, t);                        // tmp = v0*t + 0.5*a*t^2
        vv_add(tmp, p->position);              // tmp = x0 + v0*t + 0.5*a*t^2
        vv_cpy(position, tmp);                 // position = tmp

        // check for impact with the ground
        // if we know we've hit, then don't do this twice!
        if(position[1] < (*terrain_height)(position[0], position[2]) &&
            o->state == STATE_PROJECTILE) {
            
            // we've impacted, now find the point of impact
            // our search space is time=0 to time=t
            o->state = STATE_PROJECTILE_FINAL;
            memcpy(&base_object, o, sizeof(struct Object));
            t = zeroin(0.0, t, terrain_delta, 0.0);

            // and recalculate the final position based on the new time
            t = next_object_state(o, t);
            o->state = STATE_IMPACTED;
            return t;
        }

        // update position and velocity
        vv_cpy(p->position, position);
        vv_cpy(p->velocity, velocity);

        // update angular position
        vv_cpy(tmp, p->angular_velocity);      // tmp = omega
        vc_mul(tmp, t);                        // tmp = omega*t
        vv_add(p->angular_position, tmp);      // theta = theta0 + omega*t
        while(p->angular_position[0] < 0)       p->angular_position[0] += 2*M_PI;
        while(p->angular_position[0] >= 2*M_PI) p->angular_position[0] -= 2*M_PI;
        while(p->angular_position[1] < 0)       p->angular_position[1] += 2*M_PI;
        while(p->angular_position[1] >= 2*M_PI) p->angular_position[1] -= 2*M_PI;
        while(p->angular_position[2] < 0)       p->angular_position[2] += 2*M_PI;
        while(p->angular_position[2] >= 2*M_PI) p->angular_position[2] -= 2*M_PI;

        // if we found above that there is not enough propellant for this
        // session, then we must execute in two parts; this is the second part.
        if(non_propelled_time != 0.0) {
            o->propelling_time = 0.0;
            t += next_object_state(o, non_propelled_time);
        } else {
            o->propelling_time -= t;
        }
        break;
    default:
        /* do nothing */
        break;
    }

    return t;
}

// net_force calculates the net force on an object.  This includes gravitational
// force, frictional force, force due to wind resistance, internal acceleratory
// force, and other miscellaneous force.  The exact forces used is determined by
// the type of object.  f is a pointer to a 3D vector that represents the return
// value for the force.  o is a pointer to the object.
static void net_force_projectile(pVector f, const struct Object* o, float t) {
    Vector total;
    Vector temp;
    
    gravitational_force(total, o);             // total = g

    propulsion_force(temp, o, t);              // temp = P
    vv_add(total, temp);                       // total = total + P

    air_resistance_force(temp, total, o, t);   // temp = air resistance
    vv_add(total, temp);                       // total += air resistance

    vv_cpy(f, total);                          // f = total
}

// frictional_force calculates the frictional force between two surfaces.
// f is a pointer to a 3D vthat represents the return value for the
// force.  c1 is the coefficient of friction for one surface, and c2 is the
// coefficient of friction for the second surface.  n is a vector representing
// the normal force between the two surfaces (should be m*g, divided into
// components based on the angle of the surfaces).
static void frictional_force(pVector f, float c1, float c2, const pVector n) {
    v_zero(f);
}

// wind_force calculates the force on an object due to wind resistance.  f is
// a pointer to a 3D vector that represents the return value for the force, and
// F0 is the amount of force currently being exerted on the object.
static void air_resistance_force(pVector f, const pVector F0, const struct Object* o, float t) {
    Vector tmp;
    float c = -(world_data.air_viscosity*world_data.air_density);
    Vector v0 = {-world_data.wind_x, 0, -world_data.wind_z, 0};

    vv_add(v0, o->props.velocity);             // v0 = velocity - wind_velocity
    
    vv_cpy(tmp, F0);                           // k = F0
    vv_div(tmp, v0);                           // k = F0/v0
    vc_add(tmp, c);                            // k = F0/v0 - b*nu
    vc_div(tmp, o->props.mass);                // k = F0/(m*v0) - b*nu/m
                                               //   = a0/v0 - b*nu/m

    vc_mul(tmp, t);                            // tmp = k*t
    v_exp(tmp);                                // tmp = e^(k*t)
    vv_mul(tmp, v0);                           // tmp = v0*e^(k*t)
    vc_mul(tmp, c);                            // tmp = -v0*(b*nu)*e^(k*t)

    // if velocity is zero, then there should be no force in that direction
    if(fabs(v0[0]) < 0.000001) tmp[0] = 0.0;
    if(fabs(v0[1]) < 0.000001) tmp[1] = 0.0;
    if(fabs(v0[2]) < 0.000001) tmp[2] = 0.0;
    tmp[3] = 0.0;

    vv_cpy(f, tmp);                            // f = tmp
}

// gravitational_force returns the force on an object due to gravity.  f is a
// pointer to a 3D vector that represents the return value for the force.
static void gravitational_force(pVector f, const struct Object *o) {
    Vector g = {0.0, world_data.gravity * o->props.mass, 0.0, 0.0};
    vv_cpy(f, g);
}

// propulsion_force calculates the force on an object due to the object
// accelerating (propelling) itself.  f is a pointer to a 3D vector that
// represents the return value for the force.
static void propulsion_force(pVector f, const struct Object *o, float t) {
    if(o->propelling_time <= 0.0) {
        v_zero(f);
    } else {
        vv_cpy(f, o->propelling_force);
    }
}
