#include <stdio.h>
#include <string.h>
#include "physics.h"

int main() {
    struct Object o;
    Vector position = {384, 2.69478703, 384, 0};
    Vector velocity = {0, 0, 1000, 0};
    int counter;
    double t;

    // Initialize data to simulate freshman physics
    world_data.gravity = -9.8;
    world_data.air_density = 0.0;
    world_data.air_viscosity = 0.0;
    world_data.wind_x = 0.0;
    world_data.wind_z = 0.0;

    // Initialize the object -- more freshman physics values
    o.props.mass = 1.0;
    o.props.radius = 1.0;
    vv_cpy(o.props.position, position);
    vv_cpy(o.props.velocity, velocity);
    v_zero(o.props.angular_velocity);
    v_zero(o.props.angular_velocity);
    o.props.moment_coefficient = 1.0;
    o.props.friction_coefficient = 0.0;
    o.props.bounce_coefficient = 0.0;
    o.props.can_roll = 0;
    o.props.surface_area = 0.0;
    v_zero(o.propelling_force);
    o.propelling_time = 0.0;
    o.state = STATE_PROJECTILE;

    // now for a trial run
    counter = 0;
    t = 0.0;
    while(o.state != STATE_IMPACTED) {
        printf("t = %.3f   x = [ %7.3f %7.3f %7.3f ]   v = [ %7.3f %7.3f %7.3f ]\n", t,
            o.props.position[0], o.props.position[1], o.props.position[2],
            o.props.velocity[0], o.props.velocity[1], o.props.velocity[2]);
        t += next_object_state(&o, 0.1);
        counter++;
        if(counter == 20) {
            printf("Press Enter for next screen...");
            getchar();
            counter = 0;
        }
    }
    printf("t = %.3f   x = [ %7.3f %7.3f %7.3f ]   v = [ %7.3f %7.3f %7.3f ]\n", t,
        o.props.position[0], o.props.position[1], o.props.position[2],
        o.props.velocity[0], o.props.velocity[1], o.props.velocity[2]);

    return 0;
}
