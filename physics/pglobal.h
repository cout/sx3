#ifndef PGLOBAL_H
#define PGLOBAL_H

/* Global data for "physics-land" */

struct World_Data {
    float gravity;
    float air_density;
    float air_viscosity;
    float wind_x;
    float wind_z;
};

extern struct World_Data world_data;

#endif
