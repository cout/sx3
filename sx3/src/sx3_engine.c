// File: sx3_engine.c
// Author: Paul Brannan
//
// This is where we manipulate all the objects in our scene (projectiles,
// explosions, etc.)

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sx3_global.h"
#include "sx3_weapons.h"
#include "sx3_tanks.h"
#include "sx3_audio.h"
#include "sx3_files.h"
#include "sx3_math.h"

// update_projectile updates a single projectile object
float update_projectile(struct Projectile *p, float dt)
{
    switch(p->type)
    {
        case Missile_I:
        case Missile_II:
        case Missile_III:
        case Missile_IV:
            return next_object_state(&p->o, dt);
        default:
            // No other projectile types defined yet 
            assert(0);
            return 0;
        }
}

// update_explosion updates a single explosion object
float update_explosion(struct Explosion *e, float dt) {
    switch(e->type) {
    case Boom_Expl_I:
    case Boom_Expl_II:
    case Boom_Expl_III:
    case Boom_Expl_IV:
        // We should probably multiply dt by a constant to get a different
        // growth rate for the explosion.
        e->props.radius += (e->props.growth_direction)*dt;
        if(e->props.radius > explosion_radii[e->type]) {
            e->props.growth_direction = -1;
            e->props.radius -= (e->props.radius - explosion_radii[e->type]);
        } else if(e->props.radius <= 0.0) {
            // The explosion has completed -- now delete it
            delete_explosion(e);
        }
        break;
    default:
        // No other explosion types defined yet
        assert(0);
        return 0.0f;
    }
    return dt;
}

// modify_scene modifies the global variables g_projectiles and g_explosions
// during the course of an attack sequence.  It takes a parameter dt which
// represents the amount of time between frames.
// Return: the number of items left to animate.
int modify_scene(float dt)
{
    int i, j, num_impacted;
    float t;
    struct Projectile *p;
    struct Explosion *e;
    struct Tank *tank;
    Vector v;
    float d;

    // Go through all the explosions and update them according to the
    // amount of time passed.
    for(j = 0; j < g_num_explosions; j++)
    {
        e = &g_explosions[j];
        t = update_explosion(e, dt);
    }

    // Go through all the projectiles and handle the ones that have not yet
    // been impacted.
    for(j = 0, num_impacted = 0; j < g_num_projectiles; j++)
    {
        p = &g_projectiles[j];
        if(p->o.state != STATE_IMPACTED)
        {
            t = update_projectile(p, dt);
            if(p->o.state == STATE_IMPACTED)
            {
                printf("Projectile %d has impacted.\n", j);
                // If the projectile has impacted, an explosion needs to be
                // created which is appropriate to the projectile which has
                // impacted, and which takes on properties of the former
                // projectile.
                e = new_explosion(p);

                // Now, using the remaining time that did not get used on the
                // projectile, update the new explosion.
                t = update_explosion(e, dt - t);

                // And play a sound to match
                sx3_play_sound(SX3_AUDIO_EXPLOSION);
            }
        }
        if(p->o.state == STATE_IMPACTED) num_impacted++;
    }

    // FIX ME!! These two checks don't work properly if we have a low
    // framerate.
    for(j = 0; j < g_num_explosions; j++)
    {
        e = &g_explosions[j];
        for(i = 0; i < g_num_tanks; i++)
        {
            tank = &g_tanks[i];
            // Don't check this tank if it's already been hit
            // FIX ME!! This should be on a per-projectile/explosion basis
            if(tank->s.temp_damage != 0.0) continue;

            // Find the distance between the tank and the explosion, and test
            vv_cpy(v, tank->o.props.position);
            vv_sub(v, e->props.position);
            d = v_mag(v);

            if(d < tank->o.props.radius + e->props.radius)
            {
                // A tank has been hit!
                sx3_play_sound(SX3_AUDIO_HIT);
                printf("Tank %d hit by explosion %d\n", i, j);
                // FIX ME!! This should not be constant damage.
                tank->s.temp_damage = 20.0;
            }
        }
    }

    for(j = 0; j < g_num_projectiles; j++)
    {
        p = &g_projectiles[j];
        if(p->o.state == STATE_IMPACTED) continue;
        for(i = 0; i < g_num_tanks; i++)
        {
            tank = &g_tanks[i];
            // Don't check this tank if it's already been hit
            // FIX ME!! This should be on a per-projectile/explosion basis
            if(tank->s.temp_damage != 0.0) continue;

            // Find the distance between the tank and the projectiles, and test
            vv_cpy(v, tank->o.props.position);
            vv_sub(v, p->o.props.position);
            d = v_mag(v);

            if(d < tank->o.props.radius + p->o.props.radius)
            {
                // A tank has been hit!
                sx3_play_sound(SX3_AUDIO_HIT);
                printf("Tank %d hit by projectile %d\n", i, j);
                // FIX ME!! This should not be constant damage.
                tank->s.temp_damage = 40.0;
            }
        }
    }
    return g_num_explosions + g_num_projectiles - num_impacted;
}

// init_scene initializes the global variable g_projectiles immediately after
// a shot has been fired.
void init_scene()
{
    struct Tank *t = &g_tanks[g_current_tank];
    Vector dir = {
        sin(D2R(t->s.turret_angle))*cos(D2R(t->s.weapon_angle)),
        sin(D2R(t->s.weapon_angle)),
        cos(D2R(t->s.turret_angle))*cos(D2R(t->s.weapon_angle)),
    };
    Vector pos;

    // FIX ME!! Is this the right way to handle tank angle?
    // vv_add(dir, t->o.props.angular_position);
    v_norm(dir);

    // FIX ME!! We do not fire from the end of the barrel
    vv_cpy(pos, t->o.props.position);
    vv_add(pos, t->m.turret_base);
    vv_add(pos, t->m.weapon_base);

    printf("New projectile\n");
    printf("Position: %f %f %f\n", pos[0], pos[1], pos[2]);
    printf("Direction: %f %f %f\n", dir[0], dir[1], dir[2]);
    printf("Tank pos: %f %f %f\n",
        t->o.props.position[0], t->o.props.position[1], t->o.props.position[2]);

    new_projectile(
        dir,                                // Direction
        t->s.power,                         // Magnitude
        pos,                                // Position
        Missile_I                           // Type
    );
}
