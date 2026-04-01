# FOR MAPS

## `i mapID`
The unique identifier for the map. Max value is MAP_MAX_COUNT - 1.

## `p portalTargetMapID0,...`
The IDs of the maps that the portals lead to, from top to bottom, left to right. Count must be MAP_PORTALS_MAX_COUNT. Put -1 for empty slots.

## `n npcID0,...`
The IDs of the NPCs on the map, from top to bottom, left to right. Count must be MAP_NPCS_MAX_COUNT. Put -1 for empty slots.

## mapData 
The data for the map, which includes the layout, interactive elements. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT.

# FOR PORTRAITS

## portraitData 
The data for the portrait, basically pixels for the image. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT.
