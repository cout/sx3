// File: sx3_tanks.h
// Author: Marc Bryant
//
// Header containing the tank definition structures and the
// character structures

#ifndef SX3_TANKS_H
#define SX3_TANKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <physics.h>
#include <models.h>
#include "sx3_weapons.h"
#include "sx3_misc.h"

// ===========================================================================
// Global macros
// ===========================================================================

// FIX ME!! This should be a static const variable.
#define MAX_TANKS 32

// ===========================================================================
// Data types
// ===========================================================================

enum Armor_Type {
    Light,
    Medium,
    Heavy,
    Kevlar
};

struct Tank_Model {
    char                        model_file[PATH_MAX];
    char                        turret_file[PATH_MAX];
    char                        weapon_file[PATH_MAX];
    char                        model_tex_file[PATH_MAX];
    char                        turret_tex_file[PATH_MAX];
    char                        weapon_tex_file[PATH_MAX];

    model_t                     model;
    Vector                      scale;
    Vector                      trans;
    Vector                      rotate;

    model_t                     turret;
    Vector                      turret_base;
    Vector                      turret_trans;
    Vector                      turret_rot;
    
    model_t                     weapon;
    Vector                      weapon_base;
    Vector                      weapon_trans;
    Vector                      weapon_rot;
};

struct Tank_Equipment {
    enum Armor_Type             armor;
    long int                    turbo:1;
    long int                    snow_tires:1;
    long int                    floatation_buoys:1;
    long int                    water_proof:1;
};

struct Tank_Abilities {
    float       max_speed;        // in meters/second
    float       max_acceleration; // in meters/second^2
    float       traction;     
    float       water_resist;     // in meters under 0
    short int   floating;         // 0 or 1
    float       max_power;
    float         max_energy;
    float       max_angle;        // in degrees
    float       min_angle;        // in degrees
    float       rotation_angle;   // ~"fov" in degrees
    long int    max_hp;
};

enum Shield_Type {
    None,
    Energy_I,
    Energy_II,
    Magnetic,
    Laser,
    Num_Shield_Types
};


// The max_power in this struct is the effective max power (after damage
// to the tank has been taken into consideration).  The max_power in the
// abilities struct is the max power of the tank itself.
struct Tank_Stats {
    float                        turret_angle;
    float                        weapon_angle;
    float                        max_power;
    float                        power;
    float                        energy;
    float                        max_energy;
    float                        temp_damage;
};

struct Tank {
    struct Object                o;
    struct Tank_Model            m;
    struct Tank_Stats            s;
    struct Tank_Equipment        equipment;
    struct Tank_Abilities        abilities;
    unsigned char                name[64];
    enum Shield_Type             active_shield;
    long int                     experience;
    long int                     level;
    long int                     hp;
    long int                     id;  // CANNOT be -1
};

enum Misc_Item_Type {
    Parachute,
    Balistic_Guidance,
    Fuel,
    Power_Cells,
    Num_Misc_Item_Types
};

struct Character_Items {
    long int   weapons[Num_Projectile_Types];
    long int   shields[Num_Shield_Types];
    long int   misc_items[Num_Misc_Item_Types];
};

enum Alliance {
    Federation,
    Republic,
    Alliance,
    Freelance,
    Neutral
};

struct Character {
    char                    name[64];
    char                    call_sign[64];
    char                    password[64];
    long int                money;
    long int                experience;
    long int                level;
    enum Alliance           alliance;
    long int                num_tanks;
    struct                  Tank *tanks;
    struct Character_Items  items;
};


// ===========================================================================
// Function declarations
// ===========================================================================

SX3_ERROR_CODE
sx3_init_tanks (
    void
    );

SX3_ERROR_CODE
sx3_cleanup_tanks (
    void
    );

SX3_ERROR_CODE
sx3_add_tank (
    const struct Tank *new_tank
    );

SX3_ERROR_CODE
sx3_remove_tank (
    const struct Tank *tank
    );

SX3_ERROR_CODE
sx3_retrieve_tank (
    struct Tank *tank,   // Pointer to the tank structure to be filled
    long int tank_id,    // Only use tank_id or tank_index (not both)
    long int tank_index  // The non-used value must be -1
    );

SX3_ERROR_CODE sx3_load_tank_model(const char *f, struct Tank_Model *m);

#ifdef __cplusplus
}
#endif

#endif
