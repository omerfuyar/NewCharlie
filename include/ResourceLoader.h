#pragma once
#include "config.h"

// loads .map files in the given directory, returns the number of maps loaded, or 0 on error
int loadMaps(const char *directory, Map *maps, int maxMaps);

// loads .por files in the given directory, returns the number of portraits loaded, or 0 on error
int loadPortraits(const char *directory, Portrait *portraits, int maxPortraits);

// loads .npc files in the given directory, returns the number of npcs loaded, or 0 on error
int loadNPCs(const char *file, NPC *retNPCs, int maxNPCs);