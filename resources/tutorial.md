# FOR MAPS

## `i mapID`
The unique identifier for the map. Max value is MAP_MAX_COUNT - 1.

## `p portalTargetMapID0,...`
The IDs of the maps that the portals lead to, from top to bottom, left to right. Count must be MAP_PORTALS_MAX_COUNT. Put -1 for empty slots.

## `n npcID0,...`
The IDs of the NPCs on the map, from top to bottom, left to right. Count must be MAP_NPCS_MAX_COUNT. Put -1 for empty slots.

## `d\nmapData` 
The data for the map, which includes the layout, interactive elements. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT. Put d in the line before map data.

### Player
Place the player on the map using the `@` character. The character can be changed from `include/config.h`. Only one player can be placed on the map.

### Portals
Place portals on the map using the `O` character. The character can be changed from `include/config.h`. The number of portals must match the number of (non -1) portal targets specified in the `p` line.

### NPCs
Place NPCs on the map using the `I` character. The character can be changed from `include/config.h`. The number of NPCs must match the number of (non -1) NPC IDs specified in the `n` line.

## NPC Nodes
Define the dialogue trees for the NPCs mapped in the `n` line. Place these before the `d` line.

### `N npcID`
Starts the definition of an NPC. The `npcID` must match one of the IDs provided in the `n` line.

### `v reqFlag "dialogueText"`
Defines a dialogue node for the current `N`.
- `reqFlag`: The player flag index required to see this node. Put `-1` for no requirement.
- `dialogueText`: The rest of the line is the text displayed in the text field.

### `c reqFlag,setFlag,nextNodeIndex "choiceText"`
Defines a choice option for the most recently defined `v` node. Must be max INPUT_FIELD_SELECTION_COUNT choices per node.
- `reqFlag`: Player flag required to see this choice. Put `-1` for none.
- `setFlag`: Player flag to set to `1` when chosen. Put `-1` for none.
- `nextNodeIndex`: The 0-based index of the node to jump to. Put `-1` to end the conversation.
- `choiceText`: The rest of the line is the option's text.

# FOR PORTRAITS

## `i mapID,NPCIndex`
The map and NPC index that this portrait corresponds to. The map index must match one of the `i` lines in the map files, and the NPC index must be between 0 and MAP_NPCS_MAX_COUNT - 1.

## `portraitData` 
The data for the portrait, basically pixels for the image. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT.
