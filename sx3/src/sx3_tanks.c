// File: sx3_tanks.c
// Author: Marc Bryant
//
// This is the tank module.

#include <stdlib.h>
#include <string.h>
#include "sx3_tanks.h"
#include "sx3_global.h"
#include "sx3_terrain.h"
#include "sx3_files.h"

// For the moment, the list of tanks is implemented as an
// static array.  We might change this in the future.

// sx3_init_tanks initializes the list of tanks.
SX3_ERROR_CODE sx3_init_tanks(void)
{
    struct Tank temp_tank;
    
    g_tanks = (struct Tank *) malloc (MAX_TANKS * sizeof (struct Tank));
    g_num_tanks = 0;
    g_tanks [0].id = -1;
    
    // Initialize a couple test tanks
    strcpy (temp_tank.name, "Tasha");
    temp_tank.id = 0;
    temp_tank.o.props.position[0] = g_terrain_size.y/2 * METERS_PER_MAP_GRID;
    temp_tank.o.props.position[2] = g_terrain_size.x/2 * METERS_PER_MAP_GRID;
    temp_tank.o.props.position[1] = sx3_interpolated_terrain_height (temp_tank.o.props.position[0],temp_tank.o.props.position[2], 1) + g_view_altitude;
    temp_tank.o.props.velocity[0] = 0.0F;
    temp_tank.o.props.velocity[1] = 0.0F;
    temp_tank.o.props.velocity[2] = 0.0F;
    temp_tank.o.props.angular_position[0] = 0.0F;
    temp_tank.o.props.angular_position[1] = 0.0F;
    temp_tank.o.props.angular_position[2] = 0.0F;
    temp_tank.o.props.angular_velocity[0] = 0.0F;
    temp_tank.o.props.angular_velocity[1] = 0.0F;
    temp_tank.o.props.angular_velocity[2] = 0.0F;
    temp_tank.o.props.mass = 1000.0F;
    temp_tank.o.props.radius = 30.0F;
    temp_tank.o.props.surface_area = 18.8F;
    temp_tank.o.props.moment_coefficient = 1.0F;
    temp_tank.o.props.friction_coefficient = 1.0F;
    temp_tank.o.props.can_roll = 0;
    temp_tank.s.turret_angle = 0.0;
    temp_tank.s.weapon_angle = 0.0;
    temp_tank.s.power = 20.0;
    temp_tank.s.max_power = 1000.0;
    temp_tank.s.energy = 100.0;
    temp_tank.s.max_energy = 100.0;
    temp_tank.s.temp_damage = 0.0;
    sx3_load_tank_model(SX3_DEFAULT_TANK, &temp_tank.m);
    sx3_add_tank (&temp_tank);

    strcpy (temp_tank.name, "Abel");
    temp_tank.id = 1;
    temp_tank.o.props.position[0] = g_terrain_size.y/2 * METERS_PER_MAP_GRID + 300;
    temp_tank.o.props.position[2] = g_terrain_size.x/2 * METERS_PER_MAP_GRID;
    temp_tank.o.props.position[1] = sx3_interpolated_terrain_height (temp_tank.o.props.position[0],temp_tank.o.props.position[2], 1) + g_view_altitude;
    temp_tank.o.props.velocity[0] = 0.0F;
    temp_tank.o.props.velocity[1] = 0.0F;
    temp_tank.o.props.velocity[2] = 0.0F;
    temp_tank.o.props.angular_position[0] = 0.0F;
    temp_tank.o.props.angular_position[1] = 0.0F;
    temp_tank.o.props.angular_position[2] = 0.0F;
    temp_tank.o.props.angular_velocity[0] = 0.0F;
    temp_tank.o.props.angular_velocity[1] = 0.0F;
    temp_tank.o.props.angular_velocity[2] = 0.0F;
    temp_tank.o.props.mass = 1000.0F;
    temp_tank.o.props.radius = 30.0F;
    temp_tank.o.props.surface_area = 18.8F;
    temp_tank.o.props.moment_coefficient = 1.0F;
    temp_tank.o.props.friction_coefficient = 1.0F;
    temp_tank.o.props.can_roll = 0;
    temp_tank.s.turret_angle = 0.0;
    temp_tank.s.weapon_angle = 0.0;
    temp_tank.s.power = 20.0;
    temp_tank.s.max_power = 1000.0;
    temp_tank.s.energy = 100.0;
    temp_tank.s.max_energy = 100.0;
    temp_tank.s.temp_damage = 0.0;
    sx3_load_tank_model(SX3_DEFAULT_TANK, &temp_tank.m);
    sx3_add_tank (&temp_tank);

    return SX3_ERROR_SUCCESS;
}


// sx3_cleanup_tanks -- do any cleanup work (this is the complement of
// sx3_init_tanks) 
SX3_ERROR_CODE sx3_cleanup_tanks(void)
{
    // FIX ME!! We need to delete the tank model first!
    free (g_tanks);

    // FIX ME!! We should also free the shield list

    return SX3_ERROR_SUCCESS;
}


// sx3_add_tank adds a tank to the list of tanks
SX3_ERROR_CODE sx3_add_tank(const struct Tank *new_tank)
{
    int count=0;

    while (g_tanks [count].id != -1)
        count++;

    // Remember that the last element is used as a marker
    if (count >= (MAX_TANKS-1))  
        return SX3_ERROR_MEM_ALLOC;

    // FIX ME!! This won't work if we have pointers in the tanks structure
    g_tanks [count] = *new_tank;
    g_tanks [count+1].id = -1;
    g_num_tanks++;

    return SX3_ERROR_SUCCESS;
}


// sx3_remove_tank removes a tank from the list of tanks
SX3_ERROR_CODE sx3_remove_tank(const struct Tank *tank)
{ 
    // FIX ME!! This is not implemented yet!
    return SX3_ERROR_SUCCESS;
}


// sx3_retrieve_tank retrieves a tank from the list of tanks.  The tank can be
// referenced either by tank_id or by tank_index.
SX3_ERROR_CODE
sx3_retrieve_tank (
    struct Tank *tank,   // Pointer to the tank structure to be filled
    long int tank_id,    // Only use tank_id or tank_index (not both)
    long int tank_index  // The non-used value must be -1
    )
{
    long int count = 0;

    if (tank_id==-1)
    {  // Look up the tank based on the tank index
        if (tank_index >= g_num_tanks)
            return SX3_ERROR_BAD_PARAMS;
        *tank = g_tanks [tank_index];
        return SX3_ERROR_SUCCESS;
    }
    else if (tank_index==-1)
    {  // Look up the tank based on the tank id
        while (g_tanks [count].id != tank_id)
            count++;
        if (count>=g_num_tanks)
            return SX3_ERROR_BAD_PARAMS;
        *tank = g_tanks [count];
        return SX3_ERROR_SUCCESS;
    }
    else
        return SX3_ERROR_BAD_PARAMS;
}
    
// Load a tank model into a tank model structure
// FIX ME!! This should use a generic configuration file module
SX3_ERROR_CODE sx3_load_tank_model(const char *f, struct Tank_Model *m) {
    FILE *fp;
    char s[1024];
    char *cmd, *arg, *tmp;

    if((fp = fopen(f, "rt")) == NULL)
        return SX3_ERROR_CANNOT_OPEN_FILE;

    // Read the tank file
    while(!feof(fp))
    {
        fgets(s, sizeof(s), fp);

        // Here's our own little strtok
        cmd = arg = s;
        while(*arg != 0 && *arg != ' ' && *arg != '\t') arg++;
        if(*arg == 0) continue; // error condition, what should we do?
        *arg = 0;
        arg++;
        while(*arg == '\t' || *arg == ' ') arg++;

        // Now strip the \n from arg
        tmp = arg + strlen(arg) - 1;
        while(*tmp == '\n' || *tmp == '\r') {
            *tmp = 0;
            tmp--;
        }

        // Models
        if(!strcasecmp(cmd, "Model")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->model_file, "");
            } else {
                strcpy(m->model_file, arg);
            }

        } else if(!strcasecmp(cmd, "Turret")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->turret_file, "");
            } else {
                strcpy(m->turret_file, arg);
            }

        } else if(!strcasecmp(cmd, "Weapon")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->weapon_file, "");
            } else {
                strcpy(m->weapon_file, arg);
            }
        
        // Textures
        } else if(!strcasecmp(cmd, "ModelTex")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->model_tex_file, "");
            } else {
                strcpy(m->model_tex_file, arg);
            }
            
        } else if(!strcasecmp(cmd, "TurretTex")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->turret_tex_file, "");
            } else {
                strcpy(m->turret_tex_file, arg);
            }

        } else if(!strcasecmp(cmd, "WeaponTex")) {
            if(!strcasecmp(arg, "None")) {
                strcpy(m->weapon_tex_file, "");
            } else {
                strcpy(m->weapon_tex_file, arg);
            }

        // Turret info
        } else if(!strcasecmp(cmd, "TurretBase")) {
            sscanf(arg, "%f %f %f",
                &m->turret_base[0], &m->turret_base[1], &m->turret_base[2]);
        
        } else if(!strcasecmp(cmd, "TurretTrans")) {
            sscanf(arg, "%f %f %f",
                &m->turret_trans[0], &m->turret_trans[1], &m->turret_trans[2]);

        } else if(!strcasecmp(cmd, "TurretRot")) {
            sscanf(arg, "%f %f %f",
                &m->turret_rot[0], &m->turret_rot[1], &m->turret_rot[2]);

        // Weapon info
        } else if(!strcasecmp(cmd, "WeaponBase")) {
            sscanf(arg, "%f %f %f",
                &m->weapon_base[0], &m->weapon_base[1], &m->weapon_base[2]);
        
        } else if(!strcasecmp(cmd, "WeaponTrans")) {
            sscanf(arg, "%f %f %f",
                &m->weapon_trans[0], &m->weapon_trans[1], &m->weapon_trans[2]);

        } else if(!strcasecmp(cmd, "WeaponRot")) {
            sscanf(arg, "%f %f %f",
                &m->weapon_rot[0], &m->weapon_rot[1], &m->weapon_rot[2]);

        // Model info
        } else if(!strcasecmp(cmd, "Scale")) {
            sscanf(arg, "%f %f %f",
                &m->scale[0], &m->scale[1], &m->scale[2]);
        
        } else if(!strcasecmp(cmd, "Trans")) {
            sscanf(arg, "%f %f %f",
                &m->trans[0], &m->trans[1], &m->trans[2]);

        } else if(!strcasecmp(cmd, "Rotate")) {
            sscanf(arg, "%f %f %f",
                &m->rotate[0], &m->rotate[1], &m->rotate[2]);
        }
    }

    // Now load the models
    // FIX ME!! We should check the error codes here.
    if(*m->model_file) {
        parse_model(m->model_file, m->model_tex_file, &m->model);
    } else {
        m->model.skin = 0;
        m->model.framestart = 0;
        m->model.numframes = 0;
    }
    if(*m->turret_file) {
        parse_model(m->turret_file, m->turret_tex_file, &m->turret);
    } else {
        m->turret.skin = 0;
        m->turret.framestart = 0;
        m->turret.numframes = 0;
    }
    if(*m->weapon_file) {
        parse_model(m->weapon_file, m->weapon_tex_file, &m->weapon);
    } else {
        m->weapon.skin = 0;
        m->weapon.framestart = 0;
        m->weapon.numframes = 0;
    }

    printf("Model: %s (%d)\n", m->model_file, m->model.framestart);
    printf("Turret: %s (%d)\n", m->turret_file, m->turret.framestart);
    printf("Weapon: %s (%d)\n", m->weapon_file, m->weapon.framestart);
    printf("Textures: %d %d %d\n",
        m->model.skin, m->turret.skin, m->weapon.skin);

    fclose(fp);
    return SX3_ERROR_SUCCESS;

}
