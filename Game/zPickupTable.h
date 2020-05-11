#ifndef ZPICKUPTABLE_H
#define ZPICKUPTABLE_H

struct zAssetPickup
{
    unsigned int pickupHash;
    unsigned char pickupType;
    unsigned char pickupIndex;
    unsigned short pickupFlags;
    unsigned int quantity;
    unsigned int modelID;
    unsigned int animID;
};

struct zAssetPickupTable
{
    unsigned int Magic;
    unsigned int Count;
};

void zPickupTableInit();

#endif