// File: sx3_weapons.h
// Author: Marc Bryant
//
// Weapons (projectiles and explosions) header file

#ifndef SX3_WEAPONS_H
#define SX3_WEAPONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <physics.h>
#include "sx3_graphics.h"


enum Explosion_Type {
    No_Explosion,
    Boom_Expl_I,                 // standard round "scorch-like" expls
    Boom_Expl_II,
    Boom_Expl_III,
    Boom_Expl_IV,
    Nuke_Expl_I,                 // mushroom clouds
    Nuke_Expl_II,
    Nuke_Expl_III,
    Nuke_Expl_IV,
    Force_Wave_Expl,             // similar to "tribes"
    Satellite_Beam,              // see FF7
    Num_Explosion_Types
};

enum Projectile_Type  {
    Missile_I,
    Missile_II,
    Missile_III,
    Missile_IV,
    Cruise_Missle,
    Bouncer_I,
    Bouncer_II,
    Bouncer_III,
    Bouncer_IV,
    Roller_I,
    Roller_II,
    Roller_III,
    Roller_IV,
    Napalm_I,
    Napalm_II,
    Napalm_III,
    Napalm_IV,
    MIRV,
    Homing_MIRV,
    Nuke_MIRV,
    Napalm_MIRV,
    Nuke_I,
    Nuke_II,
    Nuke_III,
    Nuke_IV,
    Torpedoe,
    Flare,
    Smoke_Bomb_I,
    Smoke_Bomb_II,
    Mortar_I,
    Mortar_II,
    Mortar_III,
    Mortar_IV,
    Bazooka,
    Imploder,
    Decoy,
    Holy_Hand_Grenade,
    Force_Wave,
    Num_Projectile_Types
};

// NOTE: the main difference between the Explosions and the
// Projectiles is that the projectiles have a "static" model,
// whereas the explosions are probably going to be more
// dynamic, and their "shape" is going to be more programatically
// defined (depending on the explosion type).
struct Explosion {
    enum Explosion_Type            type;
    float                          elapsed_time;  // in seconds
    struct Physical_Properties     props;
};

struct Projectile {
    enum Projectile_Type           type;
    float                          elapsed_time;  // in seconds
     struct Object                 o;
};

// weapon_explosions maps weapon types onto explosion types -- this represents
// the explosion types that each weapon generates.
extern const enum Explosion_Type weapon_explosions[Num_Projectile_Types];

// explosion_radii maps explosion types to explosion radii -- this represents
// the radius of a given explosion type.
extern const float explosion_radii[Num_Explosion_Types];

// These functions operate on the global explosion and projectile lists
struct Explosion* new_explosion(struct Projectile *p);
void delete_explosion(struct Explosion *e);
struct Projectile* new_projectile(
    pVector direction,
    float magnitude,
    pVector position,
    enum Projectile_Type type);

#ifdef __cplusplus
}
#endif
#endif

