#ifndef MD3_H
#define MD3_H

#pragma pack(push, enter_md2_include
#pragma pack(1)

#define Vec2    float[2];
#define Vec3    float[3];
#define Mat3x3    float[3][3];

typedef struct {
    char id[4];                /* id of file, always "IDP3"       */
    int  version;            /* i suspect this is a version     */
                            /* number, always 15               */
    char filename[68];        /* sometimes left Blank...         */
                            /* 65 chars, 32bit aligned ==      */
                            /* 68 chars                        */
    int  boneframe_num;        /* umber of BoneFrames             */
    int  tag_num;            /* number of 'tags' per BoneFrame  */
    int  mesh_num;            /* number of meshes/skins          */
    int  maxSkin_num;        /* maximum number of unique skins  */
                            /* used in md3 file                */
    int  header_length;        /* always equal to the length of   */
                            /* this header                     */
    int  tag_start;            /* starting position of            */
                            /* tag-structures                  */ 
    int  tag_end;            /* ending position of              */
                            /* tag-structures/starting         */
                            /*position of mesh-structures      */
    int  filesize;            /* size of file                    */
} md3_header_t;

typedef struct {
    char    name[64];
    Vec3    Position;
    Vec3x3    Rotation
} md3_tag_t;

typedef struct {
    float    mins[3];
    float    maxs[3];
    float    position[3];
    float    scale;
    char    creator[16];
} md3_boneframe_t;

#pragma pack(pop, enter_md2_include

#endif
