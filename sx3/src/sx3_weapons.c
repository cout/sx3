// File: sx3_weaposn.c
// Author: Marc Bryant

#include <string.h>
#include <stdlib.h>
#include "sx3_global.h"
#include "sx3_weapons.h"

const enum Explosion_Type weapon_explosions[Num_Projectile_Types] = {
    Boom_Expl_I,                      // Missile_I
    Boom_Expl_II,                     // Missile_II
    Boom_Expl_III,                    // Missile_III
    Boom_Expl_IV,                     // Missile_IV
    Boom_Expl_II,                     // Cruise_Missile
    Boom_Expl_I,                      // Bouncer_I
    Boom_Expl_II,                     // Bouncer_II
    Boom_Expl_III,                    // Bouncer_III
    Boom_Expl_IV,                     // Bouncer_IV
    Boom_Expl_I,                      // Roller_I
    Boom_Expl_II,                     // Roller_II
    Boom_Expl_III,                    // Roller_III
    Boom_Expl_IV,                     // Roller_IV
    No_Explosion,                     // Napalm_I
    No_Explosion,                     // Napalm_II
    No_Explosion,                     // Napalm_III
    No_Explosion,                     // Napalm_IV
    No_Explosion,                     // MIRV,
    No_Explosion,                     // Homing_MIRV,
    No_Explosion,                     // Nuke_MIRV,
    No_Explosion,                     // Napalm_MIRV,
    No_Explosion,                     // Nuke_I,
    No_Explosion,                     // Nuke_II,
    No_Explosion,                     // Nuke_III,
    No_Explosion,                     // Nuke_IV,
    No_Explosion,                     // Torpedoe,
    No_Explosion,                     // Flare,
    No_Explosion,                     // Smoke_Bomb_I,
    No_Explosion,                     // Smoke_Bomb_II,
    No_Explosion,                     // Mortar_I,
    No_Explosion,                     // Mortar_II,
    No_Explosion,                     // Mortar_III,
    No_Explosion,                     // Mortar_IV,
    No_Explosion,                     // Bazooka,
    No_Explosion,                     // Imploder,
    No_Explosion,                     // Decoy,
    No_Explosion,                     // Holy_Hand_Grenade,
    No_Explosion                      // Force_Wave,
};

// FIX ME!! These values aren't scaled properly.
const float explosion_radii[Num_Explosion_Types] = {
    0.0,                              // No_Explosion
    10.0,                             // Boom_Expl_I
    20.0,                             // Boom_Expl_II
    30.0,                             // Boom_Expl_III
    40.0,                             // Boom_Expl_IV
    10.0,                             // Nuke_Expl_I
    20.0,                             // Nuke_Expl_II
    30.0,                             // Nuke_Expl_III
    40.0,                             // Nuke_Expl_IV
    25.0,                             // Force_Wave_Expl
    25.0,                             // Satellite_Beam
};

// Creates a new explosion and adds it to the global explosion list which
// takes on the properties of projectile P.
struct Explosion* new_explosion(struct Projectile *p)
{
    struct Explosion *e;

    // Create a new explosion.  Note that realloc is really expensive, and so this
    // should probably be changed later.
    g_num_explosions++;
    g_explosions = realloc(g_explosions, sizeof(*g_explosions) * g_num_explosions);
    e = &g_explosions[g_num_explosions-1];

    // Set the explosion's physical properties
    e->props.mass = 0.0;
    e->props.radius = 0.0;
    e->props.surface_area = 0.0;
    vv_cpy(e->props.position, p->o.props.position);
    v_zero(e->props.velocity);
    vv_cpy(e->props.angular_position, p->o.props.angular_position);
    v_zero(e->props.angular_velocity);
    e->props.friction_coefficient = 0.0;
    e->props.bounce_coefficient = 0.0;
    e->props.can_roll = 0;
    e->props.growth_direction = 1;

    // Now set the elapsed time
    e->elapsed_time = 0;

    // Finally, set the explosion type
    e->type = weapon_explosions[p->type];

    return e;
}

// delete explosion deletes an explosion from the global explosion list.
void delete_explosion(struct Explosion *e)
{
    int j;

    // Find the number of the explosion
    // FIX ME!! This is a linear search and could definitely be optimized better.
    for(j = 0; j < g_num_explosions; j++) if(&g_explosions[j] == e) break;
    if(j == g_num_explosions) return; // not found in the list

    // Delete the value but don't reallocate memory
    g_num_explosions--;
    memmove(e, e+1, (g_num_explosions - j)*sizeof(struct Explosion));
}

struct Projectile* new_projectile(
    pVector direction,
    float magnitude,
    pVector position,
    enum Projectile_Type type)
{
    struct Projectile *p;
    struct Physical_Properties *props;

    // Create a new projectile.  Note that realloc is really expensive, and so this
    // should probably be changed later.
    g_num_projectiles++;
    g_projectiles = realloc(g_projectiles, sizeof(*g_projectiles) * g_num_projectiles);
    p = &g_projectiles[g_num_projectiles-1];

    // FIX ME!!
    // Set the projectile's physical properties -- this should DEFINITELY be based
    // on the weapon type (i.e. don't hardcode the below values).
    props = &p->o.props;
    props->mass = 1.0;
    props->radius = 1.0;
    props->surface_area = 1.0;
    vv_cpy(props->position, position);
    vv_cpy(props->velocity, direction);
    vc_mul(props->velocity, magnitude);
    v_zero(props->angular_position);
    v_zero(props->angular_velocity);
    props->friction_coefficient = 0.0;
    props->bounce_coefficient = 0.0;
    props->moment_coefficient = 0.0;
    props->can_roll = 0;
    props->growth_direction = 0;

    // Set the object properties
    v_zero(p->o.propelling_force);
    p->o.propelling_time = 0.0;
    p->o.state = STATE_PROJECTILE;

    // Now set the elapsed time
    p->elapsed_time = 0;

    // Finally, set the explosion type
    p->type = type;

    return p;
}

