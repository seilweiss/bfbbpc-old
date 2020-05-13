#ifndef ZDISCOFLOOR_H
#define ZDISCOFLOOR_H

#include "xBase.h"
#include "xMath3.h"

struct zEntSimpleObj;

struct z_disco_floor_asset : xBaseAsset
{
    unsigned int flags;
    struct
    {
        float transition;
        float state;
    } interval;
    struct
    {
        unsigned int off;
        unsigned int transition;
        unsigned int on;
    } prefix_offset;
    unsigned int state_mask_size;
    unsigned int states_offset;
    unsigned int states_size;
};

struct z_disco_floor : xBase
{
    struct tile_data
    {
        unsigned char culled;
        zEntSimpleObj *ent;
        xSphere sphere;
    };

    struct
    {
        bool enabled;
        bool forward;
        bool culled;
        bool glow_culled;
    } flag;
    z_disco_floor_asset *asset;
    unsigned char **state_masks;
    unsigned char *active_state_mask;
    unsigned char *next_state_mask;
    char *prefix[3];
    tile_data *tiles[3];
    unsigned int tiles_size;
    unsigned int min_state;
    unsigned int max_state;
    unsigned int state;
    unsigned int next_state;
    unsigned int state_counter;
    float transition_delay;
    float state_delay;
    float transition_time;
    float state_time;
    xSphere bound;
    float pulse_time;
    float pulse_glow[3];
    float cull_dist_glow;
    float cull_dist_update;
    float glow_fade;
    float sound_delay;
    int curr_note;

    static void init();
    static void post_setup();
    static void init(void *ent, void *asset);

    void setup();
};

#endif