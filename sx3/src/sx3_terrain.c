// File: sx3_terrain.c
// Author: Marc Bryant
//
// This is the terrain module.
//
// Terrain file format:
//   The first 2 words in the terrain file are the dimensions (X,Y) of the
//   terrain matrix.  The rest of the file is composed of X*Y number of
//   WORD integers.  Each word represents the height for that coordinate.

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "sx3_terrain.h"
#include "sx3_global.h"
#include "sx3_math.h"
#include "sx3_gui.h"


// ===========================================================================
// Function declarations
// ===========================================================================

struct Color
calculate_vertex_color (
    float height
    );

int is_point_in_fov (
    struct Point *view_dir,
    struct Point *eye_point,
    struct Point *up_dir,
    float test_x,
    float test_y,
    float test_z,
    float fov
    );

SX3_ERROR_CODE
sx3_generate_terrain_mm (
    int start_x,
    int start_y,
    int end_x,
    int end_y
    );

int
calculate_terrain_mm_level (
    struct Point current_pos, 
    float x,
    float y
    );

SX3_ERROR_CODE
map_pt_to_mesh_pt (
    int           x,           // original map coords 
    int           y,
    int           level,       // 0=full detail, 1=half detail, 2=quarter det 
    int           skip,        // 1<<level 
    struct IPoint *new_p,      // Corrected point (in map coords) 
    struct Point  *new_gl_p,   // Corrected gl position 
    struct Point  *new_n       // Corrected gl normal 
    );

SX3_ERROR_CODE
interpolate_height (
    struct Point nw,
    struct Point ne,
    struct Point sw,
    struct Point se,
    struct Point *pt
    );

SX3_ERROR_CODE 
sx3_interpolated_terrain_point (
    float x,
    float z,
    int res,
    struct Point *pt,
    struct Point *norm
    );


// ===========================================================================
// Constatnts
// ===========================================================================

#define MAX_TERRAIN_MM_LEVEL 8
#define MIN_TERRAIN_MM_LEVEL 0
#define TILE_MOD(mac_x,mac_y) (((((mac_x))%((mac_y))) < 0) ?  (((mac_y))+(((mac_x))%((mac_y)))) : (((mac_x))%((mac_y))))
#define SNAP_TO_LOW_RES(x,y) (int)(floor((float)((x))/(float)((y))) * ((y)))


// ===========================================================================
// Global variables
// ===========================================================================

// scene lights 
// light 0 
GLfloat t_light0_ambient[] = { 0.3F, 0.3F, 0.3F, 0.0F};
GLfloat t_light0_diffuse[] = { 0.8F, 0.8F, 0.8F, 0.0F };
GLfloat t_light0_specular[] = { 1.0F, 1.0F, 1.0F, 0.0F };
GLfloat t_light0_position[] = { 0.25F, 1.0F, 0.0F, 0.0F };
GLfloat t_light0_direction[] = { 0.0F, -1.0F, 0.0F, 1.0F };
// light 1 
GLfloat t_light1_ambient[]   = { 0.3F,0.3F,0.3F,0.0F };
GLfloat t_light1_diffuse[]   = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light1_specular[]  = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light1_position[]  = { 0.0F,4.0F,0.0F,1.0F };
GLfloat t_light1_direction[] = { 0.0F,-1.0F,0.0F,1.0F };
// light 2 
GLfloat t_light2_ambient[]   = { 0.3F,0.3F,0.3F,0.0F };
GLfloat t_light2_diffuse[]   = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light2_specular[]  = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light2_position[]  = { -6.0F,11.0F,-6.0F,1.0F };
GLfloat t_light2_direction[] = { 0.9F,-1.0F,-0.9F,1.0F };
// light 3 
GLfloat t_light3_ambient[]   = { 0.3F,0.3F,0.3F,0.0F };
GLfloat t_light3_diffuse[]   = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light3_specular[]  = { 1.0F,1.0F,1.0F,0.0F };
GLfloat t_light3_position[]  = { 6.0F,11.0F,-6.0F,1.0F };
GLfloat t_light3_direction[] = { -0.9F,-1.0F,0.9F,1.0F };

int tex_num;  // Terrain texture number 
GLfloat yequalzero[] = {0.0, 1.0, 0.0, 0.0};  // Used for tex coord generation 

// Terrain surface material 
GLfloat terrain_ambient[] =   {0.0F,0.0F,0.0F,0.5F};
GLfloat terrain_diffuse[] =   {0.2F,1.0F,0.2F,0.5F};
GLfloat terrain_specular[] =  {0.0F,0.0F,0.0F,0.5F};
GLfloat terrain_shininess[] = {127.0};


// ===========================================================================
// Function definitions
// ===========================================================================

// sx3_load_terrain
//
// Loads the desired terrain and returns 0 if successful.
// 
// INPUT:
// OUTPUT:
// RETURN: SX3_ERROR_SUCCESS
//         SX3_ERROR_CANNOT_OPEN_FILE
SX3_ERROR_CODE sx3_load_terrain(
    char* terrainName, 
    float* buffer, 
    struct IPoint* terrainSize, 
    struct Point* normalBuffer, 
    struct Point* normalAvgBuffer)
{
    FILE* inFile;   // File containing terrain data 
    Sint16 *tmpbuf, *origbuf, dummy;
    Uint16 terrain_height, terrain_width;
    int i,j;
    struct Point v1,v2;
    float* mapPtr;
    struct Point* normalAvgPtr;
    int avgCount;

    // For the moment, assume that terrain name is the name of 
    // the file containing the terrain data 

    // Open file and return if error 
    if (!(inFile = fopen(terrainName,"rb")))
    {
        printf ("Unable to open file: %s!\n",terrainName);
        return SX3_ERROR_CANNOT_OPEN_FILE;
    }

    // Read the size of the terrain
    fread(&terrain_width,2,1,inFile);
    fread(&terrain_height,2,1,inFile);

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        terrainSize->x = SDL_Swap16(terrain_width);
        terrainSize->y = SDL_Swap16(terrain_height);
    } else {
        terrainSize->x = terrain_width;
        terrainSize->y = terrain_height;
    }

    printf("Loading the terrain\n");
    tmpbuf = origbuf = malloc(terrainSize->x*terrainSize->y*2);
    fread(tmpbuf, 2, terrainSize->x*terrainSize->y, inFile);
    mapPtr = buffer;
    for (j=0; j<terrainSize->y; j++) {
        for (i=0; i<terrainSize->x; i++) {
                if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    dummy = SDL_Swap16(*tmpbuf);
                else
                    dummy = *tmpbuf;
                *mapPtr = (float)dummy/MAX_FILE_TERRAIN_HEIGHT*MAX_TERRAIN_HEIGHT;
                mapPtr++;
                tmpbuf++;
        }
    }

    free(origbuf);
    fclose(inFile);

    // Calculate Terrain Normals (For each triangle) 
    mapPtr = buffer;
    for (j=0; j<terrainSize->y-1; j++)
    {
        for (i=0; i<terrainSize->x-1; i++)
        {
            v1.x = -1.0F/MAX_MAP_Y*MAX_TERRAIN_DEPTH;
            v1.y = *(mapPtr+terrainSize->x) - *(mapPtr);
            v1.z = 0;
            v2.x = 0;
            v2.y = *(mapPtr+1) - *(mapPtr);
            v2.z = 1.0F/MAX_MAP_X*MAX_TERRAIN_WIDTH;

            *(normalBuffer + 2*i + (terrainSize->x-1)*2*j) = sx3_norm_cross_prod(v1, v2);

            v1.x = 1.0F/MAX_MAP_Y*MAX_TERRAIN_DEPTH;
            v1.y = *(mapPtr+1) - *(mapPtr+terrainSize->x+1);
            v1.z = 0;
            v2.x = 0;
            v2.y = *(mapPtr+terrainSize->x) - *(mapPtr+terrainSize->x+1);
            v2.z = -1.0F/MAX_MAP_X*MAX_TERRAIN_WIDTH;

            *(normalBuffer + 1 + 2*i + (terrainSize->x-1)*2*j) = sx3_norm_cross_prod(v1, v2);
            
            mapPtr++;
        }
        mapPtr++;
    }

    // Calculate Terrain Normals (for each vertex)
    // these are calculated by averaging the normals for all 
    // surrounding triangles
    normalAvgPtr = normalAvgBuffer;
    for (j=0; j<terrainSize->y; j++)
        for (i=0; i<terrainSize->x; i++)
        {
            avgCount=0;
            normalAvgPtr->x = 0;
            normalAvgPtr->y = 0;
            normalAvgPtr->z = 0;

            // I know, I know: the following code is very ugly.  It is cut and
            // paste from the old code just above (commented out) that did
            // not tile the terrain map.  
            // In the future, I will consider beautifying this code, but for 
            // the moment it will suffice 
            normalAvgPtr->x += (normalBuffer+TILE_MOD(i-1,terrainSize->x)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            normalAvgPtr->x += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            avgCount += 2;

            normalAvgPtr->x += (normalBuffer+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            normalAvgPtr->x += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+1+TILE_MOD(i-1,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            avgCount += 2;

            normalAvgPtr->x += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            normalAvgPtr->x += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j-1,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            avgCount += 2;

            normalAvgPtr->x += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            normalAvgPtr->x += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->x;
            normalAvgPtr->y += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->y;
            normalAvgPtr->z += (normalBuffer+1+TILE_MOD(i,terrainSize->x-1)*2+TILE_MOD(j,terrainSize->x-1)*(terrainSize->x-1)*2)->z;
            avgCount += 2;

            normalAvgPtr->x /= avgCount;
            normalAvgPtr->y /= avgCount;
            normalAvgPtr->z /= avgCount;
    
            normalAvgPtr++;
        }  // Calculating average normal for each vertex 

    return SX3_ERROR_SUCCESS;
}  // sx3_load_terrain 

   
// sx3_draw_terrain
//
// Draws the terrain in OpenGL.  At this point, the terrain is not
// in a compiled list because we are only drawing the terrain right
// around the viewer (and not the whole thing), so the terrain being
// drawn will be constantly changing.
SX3_ERROR_CODE sx3_draw_terrain( 
    struct Point current_pos, 
    struct Point current_view_dir,
    struct Point current_up_vector)
{
    int x,y, startx, starty, nexty, endx, endy;
    int cutoff_startx, cutoff_endx, cutoff_starty, cutoff_endy;
    struct Color vColor = {0.0, 0.0, 0.0, 1.0};
    struct Point temp_point;
    float current_map_x, current_map_y;  // Map coords of user pos 
    struct IPoint map_pt;
    struct Point gl_pt;
    struct Point gl_norm;
    float cosfov = cos(g_fov);
    
    int skip, radius, cutoff, detail_level;

    // Pick material 
    glMaterialfv(GL_FRONT,GL_AMBIENT,terrain_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,terrain_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,terrain_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,terrain_shininess);
    glDisable(GL_TEXTURE_2D);

    current_map_x = (int)GL_Z_TO_MAP_X(current_pos.z);
    current_map_y = (int)GL_X_TO_MAP_Y(current_pos.x);

    for (detail_level=0; detail_level<g_terrain_detail_levels; detail_level+=g_terrain_detail_skip)
    {
        skip   = 1<<detail_level;
        radius = 1<<(g_terrain_detail_cutoff+detail_level);
        cutoff = 1<<(g_terrain_detail_cutoff+detail_level-g_terrain_detail_skip);

        // calculating start and cutoff_start values ends up being complicated.
        // Keep in mind that we need to place the terrain exactly centered on
        // our current eye_point 
        startx = SNAP_TO_LOW_RES((int)current_map_x - radius + (skip<<(g_terrain_detail_skip-1)), (skip<<g_terrain_detail_skip));
        starty = SNAP_TO_LOW_RES((int)current_map_y - radius + (skip<<(g_terrain_detail_skip-1)), (skip<<g_terrain_detail_skip));
        endx   = SNAP_TO_LOW_RES((int)current_map_x + radius + (skip<<(g_terrain_detail_skip-1)), (skip<<g_terrain_detail_skip));
        endy   = SNAP_TO_LOW_RES((int)current_map_y + radius + (skip<<(g_terrain_detail_skip-1)), (skip<<g_terrain_detail_skip));

        cutoff_startx = SNAP_TO_LOW_RES (current_map_x-cutoff + (skip>>1), skip); 
        cutoff_endx   = SNAP_TO_LOW_RES (current_map_x+cutoff + (skip>>1), skip);
        cutoff_starty = SNAP_TO_LOW_RES (current_map_y-cutoff + (skip>>1), skip); 
        cutoff_endy   = SNAP_TO_LOW_RES (current_map_y+cutoff + (skip>>1), skip);

        // temp_point is the eye_point for the terrain visibility cone:
        // ie:  what is in our fov?
        temp_point = current_pos;
        temp_point.x-=current_view_dir.x*6;
        temp_point.y-=current_view_dir.y*6;
        temp_point.z-=current_view_dir.z*6;
        current_view_dir = sx3_normalize (current_view_dir);

        // Begin terrain rendering, vertex by vertex ...
        for (y=starty; y<endy; y=nexty)
        {
            nexty = y + skip;

            // Start the rendering 
            glBegin(GL_TRIANGLE_STRIP);
                for (x=startx; x<=endx; x+=skip)
                {
                    // Skip the "middle section" 
                    if ( detail_level && // Detail level 0 will not have a hole in the middle 
                         x > cutoff_startx &&
                         x < cutoff_endx &&
                         nexty > cutoff_starty && // Don't forget - triag strips run West to East 
                         y < cutoff_endy )
                    {
                        glEnd ();
                        glBegin (GL_TRIANGLE_STRIP);
                        x+=((cutoff-skip)*2);
                        continue;
                    }

                    // Calculate Height Field index for the vertices to render 
                    // Interpolate the edges 
                    if (y==starty || x==startx || x==endx)
                    {
                        sx3_interpolated_terrain_point (
                            MAP_Y_TO_GL_X (y),
                            MAP_X_TO_GL_Z (x),
                            skip<<g_terrain_detail_skip,
                            &gl_pt,
                            &gl_norm);
                    }
                    else
                        map_pt_to_mesh_pt (x,y,
                            detail_level,skip,&map_pt,&gl_pt,&gl_norm);

                    // Make sure that the vertex is within our fov 
                    if (!is_point_in_fov(
                        &current_view_dir, &temp_point, &current_up_vector,
                        gl_pt.x, gl_pt.y, gl_pt.z, cosfov))
                    {
                        // FIX ME!! No need to call these functions so many times.
                        glEnd ();
                        glBegin (GL_TRIANGLE_STRIP);
                        continue;
                    }

                    // Render this vertex 
                    glNormal3f(gl_norm.x,
                               gl_norm.y,
                               gl_norm.z);
                    vColor = calculate_vertex_color(gl_pt.y);
                    glMaterialfv(GL_FRONT,GL_DIFFUSE, (GLfloat*)&(vColor.r));
                    glVertex3f (gl_pt.x,
                                gl_pt.y,
                                gl_pt.z);
        
                    // Interpolate the edges 
                    if (nexty==endy || x==startx || x==endx) 
                    {
                        sx3_interpolated_terrain_point (
                            MAP_Y_TO_GL_X (nexty),
                            MAP_X_TO_GL_Z (x),
                            skip<<g_terrain_detail_skip,
                            &gl_pt,
                            &gl_norm);
                    }
                    else
                        map_pt_to_mesh_pt (x,nexty,
                            detail_level,skip,&map_pt,&gl_pt,&gl_norm);

                    // Render the southern vertex 
                    glNormal3f(gl_norm.x,
                               gl_norm.y,
                               gl_norm.z);
                    vColor = calculate_vertex_color(gl_pt.y);
                    glMaterialfv(GL_FRONT,GL_DIFFUSE, (GLfloat*)&(vColor.r));
                    glVertex3f( gl_pt.x,
                                gl_pt.y,
                                gl_pt.z );
                }  // for x ... 
            glEnd();
        }  // for y .... 
    }  // for detail_level ... 

    return SX3_ERROR_SUCCESS;
}  // sx3_draw_terrain 


// calculate_vertex_color
struct Color calculate_vertex_color(float height)
{
    struct Color c;
    float heightRange = MAX_TERRAIN_HEIGHT;
    float height_normalized = (height)/heightRange;
    int selector = height_normalized * 10;

    c.a = 1.0;

    // The selector is ten times height_normalized, so a value of 0 means
    // that height_normalized is between 0.0 and 0.1, a value of 1 means that
    // height_normalized is between 0.1 and 0.2, etc.
    switch(selector) {
        case 0: // 0.0 to 0.1
            c.r = 0.2F;
            c.g = 0.6F;
            c.b = 0.2F;
            break;
        case 1: // 0.1 to 0.2
            c.r = 0.2F + (height_normalized - 0.1F)*10.0F*0.33F;
            c.g = 0.6F - (height_normalized - 0.1F)*10.0F*0.53F;
            c.b = 0.0F + (height_normalized - 0.1F)*10.0F*0.11F;
            RANGE_CHECK(c.r, 0.0, 1.0);
            RANGE_CHECK(c.g, 0.0, 1.0);
            RANGE_CHECK(c.b, 0.0, 1.0);
            break;
        case 2: // 0.2 to 0.3
            c.r = 0.53F;
            c.g = 0.47F;
            c.b = 0.31F;
            break;
        case 3: // 0.3 to 0.4
            c.r = 0.53F + (height_normalized - 0.3F)*10.0F*0.47F;
            c.g = 0.47F + (height_normalized - 0.3F)*10.0F*0.53F;
            c.b = 0.31F + (height_normalized - 0.3F)*10.0F*0.69F;
            RANGE_CHECK(c.r, 0.0, 1.0);
            RANGE_CHECK(c.g, 0.0, 1.0);
            RANGE_CHECK(c.b, 0.0, 1.0);
            break;
        default: // 0.4 and greater
            c.r = 1.0F;
            c.g = 1.0F;
            c.b = 1.0F;
    }

    return c;
}  // calculate_vertex_color 


// is_point_in_fov
int is_point_in_fov(
    struct Point *view_dir,
    struct Point *eye_point,
    struct Point *up_dir,
    float test_x,
    float test_y,
    float test_z,
    float cosfov)
{
    struct Point test_dir;
    float dot;
    float mag;

    // Find the direction of the test point.
    test_dir.x = test_x - eye_point->x;
    test_dir.y = test_y - eye_point->y;
    test_dir.z = test_z - eye_point->z;

    // Find the dot product of the test vector and the view vector.
    // This result is not normalized.
    dot = view_dir->x * test_dir.x +
          view_dir->y * test_dir.y +
          view_dir->z * test_dir.z;

    // Perform this test early to get better performance.
    if(dot <= 0)
        return 0;

    // Find the magnitude of the test vector.
    mag = test_dir.x*test_dir.x + test_dir.y*test_dir.y + test_dir.z*test_dir.z;
    if(mag == 0.0)
        return 0;
    // mag = sqrt(mag);
    mag = fast_sqrt(mag);

    // Now, return the result of the fov test.
    // Note that the dot product of two unit vectors is equal to the cosine
    // of the included angle.  The point is outside our fov if this included
    // angle is less than the value in fov.
    // return (acos(dot/mag)<fov);
    // return (fov_lookup[(int)(dot/mag*FOV_TABLE_SIZE)] < fov);
    return (dot/mag) > cosfov;
}  // is_point_in_fov 

// This one is for the physics engine 
float sx3_find_terrain_height(float x, float y) {
    return sx3_interpolated_terrain_height(x, y, 1);
}

// sx3_interpolated_terrain_height
//
// Returns the interpolated height of any particular point on the terrain.
float sx3_interpolated_terrain_height(
    float x,    // GL x coord 
    float z,    // GL y coord 
    int res     // Grid resolution level: 1  == every vert rendered,
    )           //                        16 == every 16th vert rendered, etc.
{
    // NOTE: North = Positive x axis
    //             = -y map grid coordinates
    float grid_x = GL_Z_TO_MAP_X (z);
    float grid_y = GL_X_TO_MAP_Y (x);
    int   int_x  = SNAP_TO_LOW_RES (grid_x, res);
    int   int_y  = SNAP_TO_LOW_RES (grid_y, res);
    float frac_x = grid_x - int_x;
    float frac_y = grid_y - int_y;
    struct Point nw, ne, sw, se;
    struct Point r, l;
    float alpha;

    // Do some greedy calculations to optimize 
    int TMxx = TILE_MOD(int_x, g_terrain_size.x);
    int TMrx = TILE_MOD(int_x + res, g_terrain_size.x);
    int TMyy = TILE_MOD(int_y, g_terrain_size.y);
    int TMry = TILE_MOD(int_y + res, g_terrain_size.y);

    // Calculate the index (into the heightfield) of the vertices 
    int nw_hf_index = (TMxx) + (TMyy) * g_terrain_size.x;
    int ne_hf_index = (TMrx) + (TMyy) * g_terrain_size.x;
    int sw_hf_index = (TMxx) + (TMry) * g_terrain_size.x;
    int se_hf_index = (TMrx) + (TMry) * g_terrain_size.x;

    // Do some more greedy calculations 
    float gl_x = MAP_Y_TO_GL_X(int_y);
    float gl_z = MAP_X_TO_GL_Z(int_x);
    float gl_xr = MAP_Y_TO_GL_X(int_y + res);
    float gl_zr = MAP_X_TO_GL_Z(int_x + res);

    if (res <= 0)
        printf ("NEGATIVE RES!!!!!!!!\n");

    // We are using bi-linear interpolation to find the correct height 

    // Which triangle are we processing 
    if ((res-frac_y)>frac_x)
    {
        // Process northwest triangle 
        nw.x    = gl_x;
        nw.z    = gl_z;
        nw.y    = g_terrain_vertex_height [nw_hf_index];

        ne.x    = gl_x;
        ne.z    = gl_zr;
        ne.y    = g_terrain_vertex_height [ne_hf_index];

        sw.x    = gl_xr;
        sw.z    = gl_z;
        sw.y    = g_terrain_vertex_height [sw_hf_index];

        // Calculate "left" vertex 
        alpha = (x-sw.x) / (nw.x-sw.x);
        l.y = (1-alpha)*sw.y + (alpha)*nw.y;
        l.x = x;
        l.z = nw.z;

        // Calculate "right" vertex 
        alpha = (x-sw.x) / (ne.x-sw.x);
        r.y = (1-alpha)*sw.y + (alpha)*ne.y;
        r.x = x;
        r.z = (1-alpha)*sw.z + (alpha)*ne.z;

    }  // End of northwest triangle 

    else
    {
        // Process southeast triangle 
        ne.x    = gl_x;
        ne.z    = gl_zr;
        ne.y    = g_terrain_vertex_height [ne_hf_index];

        sw.x    = gl_xr;
        sw.z    = gl_z;
        sw.y    = g_terrain_vertex_height [sw_hf_index];

        se.x    = gl_xr;
        se.z    = gl_zr;
        se.y    = g_terrain_vertex_height [se_hf_index];

        // Calculate "left" vertex 
        alpha = (x-sw.x) / (ne.x-sw.x);
        l.y = (1-alpha)*sw.y + (alpha)*ne.y;
        l.x = x;
        l.z = (1-alpha)*sw.z + (alpha)*ne.z;

        // Calculate "right" vertex 
        alpha = (x-se.x) / (ne.x-se.x);
        r.y = (1-alpha)*se.y + (alpha)*ne.y;
        r.x = x;
        r.z = ne.z;

    }  // End of southeast triangle 

    // Linearly interpolate between the left and right vertexes 
    alpha = (z-l.z) / (r.z-l.z);
    return (1-alpha)*l.y + (alpha)*r.y;
}  // sx3_interpolated_terrain_height 


// sx3_interpolated_terrain_point
SX3_ERROR_CODE sx3_interpolated_terrain_point(
    float x,
    float z,
    int res,
    struct Point *pt,
    struct Point *norm
    )
{
    float grid_x = GL_Z_TO_MAP_X (z);
    float grid_y = GL_X_TO_MAP_Y (x);

    switch (g_terrain_detail_alg)
    {
    case 0:
        pt->x = x;
        pt->z = z;    
        pt->y = sx3_interpolated_terrain_height (x, z, res);
        *norm = g_terrain_vertex_normal [(TILE_MOD((int)grid_x,g_terrain_size.x)) +
            ((TILE_MOD((int)grid_y,g_terrain_size.y)) * g_terrain_size.x)];    

        return SX3_ERROR_SUCCESS;
        break;
    }  // end switch 

    return SX3_ERROR_SUCCESS;
}  // sx3_interpolated_terrain_point 
    


// interpolate_height
//
// This function bi-linearly interpolates a height given 4 other heights
// BUGBUG: we are still assuming a square grid !!!!!
SX3_ERROR_CODE interpolate_height(
    struct Point nw,
    struct Point ne,
    struct Point sw,
    struct Point se,
    struct Point *pt)
{
    struct Point r, l;
    float alpha;

    // NOTE: North = Positive x axis
    //             = -y map grid coordinates
    
    // We are using bi-linear interpolation to find the correct height 

    // Which triangle are we processing 
    if ((sw.x-pt->x) > (sw.x-ne.x)/(ne.z-sw.z)*(pt->z-sw.z))
    {
        // Process northwest triangle 

        // Calculate "left" vertex 
        alpha = (pt->x-sw.x) / (nw.x-sw.x);
        l.y = (1-alpha)*sw.y + (alpha)*nw.y;
        l.x = pt->x;
        l.z = nw.z;

        // Calculate "right" vertex 
        alpha = (pt->x-sw.x) / (ne.x-sw.x);
        r.y = (1-alpha)*sw.y + (alpha)*ne.y;
        r.x = pt->x;
        r.z = (1-alpha)*sw.z + (alpha)*ne.z;
    }  // End of northwest triangle 

    else
    {
        // Process southeast triangle 

        // Calculate "left" vertex 
        alpha = (pt->x-sw.x) / (ne.x-sw.x);
        l.y = (1-alpha)*sw.y + (alpha)*ne.y;
        l.x = pt->x;
        l.z = (1-alpha)*sw.z + (alpha)*ne.z;

        // Calculate "right" vertex 
        alpha = (pt->x-se.x) / (ne.x-se.x);
        r.y = (1-alpha)*se.y + (alpha)*ne.y;
        r.x = pt->x;
        r.z = ne.z;
    }  // End of southeast triangle 

    // Linearly interpolate between the left and right vertexes 
    alpha = (pt->z-l.z) / (r.z-l.z);
    pt->y = (1-alpha)*l.y + (alpha)*r.y;
    return SX3_ERROR_SUCCESS;
}  // interpolate_height 


SX3_ERROR_CODE sx3_unload_terrain(void)
{
    return SX3_ERROR_SUCCESS;
}  // sx3_unload_terrain 


// sx3_draw_terrain_lights
// 
// "Draw" the scene lights.
void sx3_draw_terrain_lights()
{
  // set the scene default ambient 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, t_light0_ambient);
  
  // make specular correct for spots 
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

  // light 0 
  glLightfv(GL_LIGHT0, GL_AMBIENT, t_light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, t_light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, t_light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, t_light0_position);

  // enable the lights 
  glEnable(GL_LIGHT0);
}  // sx3_draw_terrain_lights  


// map_pt_to_mesh_pt
//
// This function takes as input a terrain map coordinate.  From this, we
// calculate the "corrected" coord and gl pos and gl norm.  The meaning
// of corrected depends on what LOD algorithm we are using.
//
// IMPORTANT: x and y MUST be divisible by skip!!!!!!!
// 
// INPUT:    x,y = Original map coords          
//            level = LOD (0=full, 1=half detail, 2=quarter detail
//            skip = 1<<level
// OUTPUT:    new_p = "corrected" point (in map coords)    
//            new_gl_p = "corrected" gl position 
//            new_n = "corrected" gl normal 
// RETURN:    SX3_ERROR_SUCCESS
SX3_ERROR_CODE map_pt_to_mesh_pt(
    int           x,           // original map coords 
    int           y,
    int           level,       // 0=full detail, 1=half detail, 2=quarter det 
    int           skip,        // 1<<level 
    struct IPoint *new_p,      // Corrected point (in map coords) 
    struct Point  *new_gl_p,   // Corrected gl position 
    struct Point  *new_n       // Corrected gl normal 
    )
{
    int v_index;

    switch (g_terrain_detail_alg)
    {
        case 0:  // Simple "square" LOD 
            // First, snap the coords to the correct level of detail 
            new_p->x = x;//SNAP_TO_LOW_RES (x, skip);
            new_p->y = y;//SNAP_TO_LOW_RES (y, skip);

            // Nex, calculate the gl position and normal of this point 
            v_index    = (TILE_MOD(new_p->x,g_terrain_size.x)) + 
                ((TILE_MOD(new_p->y,g_terrain_size.y)) * g_terrain_size.x);   
            new_gl_p->z = MAP_X_TO_GL_Z (new_p->x);
            new_gl_p->x = MAP_Y_TO_GL_X (new_p->y);
            new_gl_p->y = g_terrain_vertex_height [v_index];
            *new_n      = g_terrain_vertex_normal [v_index];
            
            return SX3_ERROR_SUCCESS;
            break;
    }  // end switch 
        
    return SX3_ERROR_SUCCESS;
}  // map_pt_to_mesh_pt 

// Deform the terrain by a sphere defined by a center (x,y,z) and radius r 
void deform_terrain(float x, float y, float z, float r) {
    // FIX ME!! I have no idea how to implement this function.  Marc??
}

