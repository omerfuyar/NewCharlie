# FOR MAPS

## `MAP_ID mapID`
The unique identifier for the map. Max value is MAP_MAX_COUNT - 1.

## `PORTALS portalTargetMapID0,...`
The IDs of the maps that the portals lead to, from top to bottom, left to right. Max count is MAP_PORTALS_MAX_COUNT.

## `NPCS npcID0,...`
The IDs of the NPCs on the map, from top to bottom, left to right. Max count is MAP_NPCS_MAX_COUNT.

## `GRID` 
The data for the map, which includes the layout, interactive elements. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT. Put GRID in the line before map data.

### Player
Place the player on the map using the `@` character. The character can be changed from `include/config.h`. Only one player can be placed on the map.

### Portals
Place portals on the map using the `O` character. The character can be changed from `include/config.h`. The number of portals must match the number of (non -1) portal targets specified in the `PORTALS` line.

### NPCs
Place NPCs on the map using the `I` character. The character can be changed from `include/config.h`. The number of NPCs must match the number of (non -1) NPC IDs specified in the `NPCS` line.

## NPC Nodes
Define the dialogue trees for the NPCs mapped in the `NPCS` line. Place these before the `GRID` line.

### `NPC <npcID> PORTRAIT <npcPortraitIndex>`
Starts the definition of an NPC. The `npcID` must match one of the IDs provided in the `NPCS` line. The `npcPortraitIndex` is the index of the portrait to display when talking to this NPC. Put -1 for no portrait.

### `NODE <dialogueText>`
Defines a dialogue node for the current `NPC`.
- `dialogueText`: The rest of the line is the text displayed in the text field.

### `CHOICE req:<reqFlagMask> set:<actionCode> goto:<nextNodeIndex> <choiceText>`
Defines a choice option for the most recently defined `NODE` node. Must be max INPUT_FIELD_SELECTION_COUNT choices per node.
- `reqFlagMask`: A 32-bit bitmask of player flags required to see this choice. Every flag is 8 bit so it can store `4` flags inside. Put `0` for none.
- `actionCode`: A 32-bit integer packed with instruction data to execute when chosen. Put `0` for no action.
  - **Bits 24-31:** Action Type (e.g., 0 = None, 1 = Set Flag, 2 = Teleport, 3 = Edit Map Tile)
  - **Bits 16-23:** Argument 1 (e.g., Flag ID, Target Map ID, etc.)
  - **Bits 8-15:** Argument 2 (e.g., X coordinate)
  - **Bits 0-7:** Argument 3 (e.g., Y coordinate or ASCII char)
  - *Example:* `0x02030A05` = Action 2 (Teleport) to Map 3, X=10, Y=5.
- `nextNodeIndex`: The 0-based index of the node to jump to. Put `-1` to end the conversation.
- `choiceText`: The rest of the line is the option's text.

# FOR PORTRAITS

## `portraitIndex`
The unique identifier for the portrait. Max value is PORTRAIT_MAX_COUNT - 1.

## `portraitData` 
The data for the portrait, basically pixels for the image. Size must be MAP_MAX_WIDTH x MAP_MAX_HEIGHT.
