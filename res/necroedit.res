# NecroEdit Tiles/Objects configuration file

[Tiles]

# Tile Format: TileID,TileName,Zone1Variant,Zone2Variant,...
# Tile Clone Format (for tiles that copy the looks of others): TileID,TileName,=TileBaseID
# Tile Variant Format: ImageFile:CropX:CropY:CropWidth:CropHeight:YOffset:BaseTileID:Opacity

# Variant order: Zone 1, Zone 2, Zone 3 Hot, Zone 3 Cold, Zone 4, Boss, Zone 1 Cracked, Zone 2 Cracked, Zone 3 Hot Cracked, Zone 3 Cold Cracked, Zone 4 Cracked
# Prefix a name with ! to hide it in the selection panel.
# Filenames are specified without "level/" prefix or ".png" suffix.

# System Tiles

-3,Torch,wall_torch:0:0:12:26:-24
-2,Unchanged,floor_x:0:0:24:24
-1,Erase Tile,end_of_world:0:0:24:24

# Floors

0,Floor,floor_dirt1,zone2_floorA,zone3_floor,zone3_floorB,zone4_floor:1:27:24:24,boss_floor_A,zone5_floor
1,!Floor 2,=0
2,Stairs,stairs:0:0:24:24
3,Shop Floor,TEMP_shop_floor
4,Water,TEMP_floor_water:1:2:24:24:0:0:128
5,Deep Water,TEMP_floor_water:1:2:24:24
6,Locked Stairs (Lobby),stairs_locked:0:0:24:24
7,Lobby Upgrades Floor,TEMP_npc_floor
8,Tar,floor_tar
9,Locked Stairs (Miniboss),stairs_locked_miniboss:0:0:24:24
10,Hot Coals,TEMP_floor_hotcoal
11,Ice,TEMP_floor_ice
12,Crystal Tile,TEMP_floor_crystal
13,Geyser,TEMP_floor_geyser:53:1:24:24
14,!Floor 3,=0
15,Locked Stairs (3 Diamonds),stairs_locked_diamond3:0:0:24:24
16,Locked Stairs (9 Diamonds),stairs_locked_diamond9:0:0:24:24
17,Ooze,TEMP_floor_magnetic
18,Lava,floor_lava
19,!Floor 4,=0
20,Wire,wire:360:0:24:24:0:0

# Walls

100,Dirt Wall,wall_dirt_crypt,wall_zone2,zone3_wall_dirt_hot,zone3_wall_dirt_cold,zone4_wall_dirt,boss_wall,zone5_wall_dirt,zone1_wall_dirt_cracked,zone2_wall_dirt_crack,zone3_wall_dirt_hot_crack,zone3_wall_dirt_cold_crack,zone4_wall_dirt_crack,boss_wall,zone5_wall_dirt_crack
101,!Dirt Wall 2,=100
102,Level Border,end_of_world
103,Door,door_front:0:0:24:29:-15:0
104,Shop Wall,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt,wall_shop_crypt_cracked,wall_shop_crypt_cracked,wall_shop_crypt_cracked,wall_shop_crypt_cracked,wall_shop_crypt_cracked,wall_shop_crypt_cracked,wall_shop_crypt_cracked
105,Unbreakable Wall,wall_shop_crypt_dark
106,Locked Door,door_locked_front:0:0:24:32:-15:0
107,Stone Wall,wall_stone_crypt,zone2_wall_stone,zone3_wall_stone_hot,zone3_wall_stone_cold,zone4_wall_rock_A,boss_wall,zone5_wall_stone_A,zone1_wall_stone_cracked,zone2_wall_stone_crack,zone3_wall_stone_hot_cracked,zone3_wall_stone_cold_cracked,zone4_wall_rock_A_crack,boss_wall,zone5_wall_stone_crack
108,Catacomb Wall,wall_catacomb_crypt1,wall_catacomb_crypt1,wall_catacomb_crypt1,wall_catacomb_crypt1,zone4_wall_catacomb_A,wall_catacomb_crypt1,zone5_wall_catacomb,zone1_catacomb_cracked,zone1_catacomb_cracked,zone1_catacomb_cracked,zone1_catacomb_cracked,zone4_wall_catacomb_A_crack,zone1_catacomb_cracked,zone5_wall_catacomb_crack
109,Boss Wall,boss_wall
110,!Cracked Shop Wall,wall_shop_crypt_cracked
111,Metal Door,door_metal_front:0:0:24:29:-15:0
112,NecroDancer Stage Green,necrodancer_stage:0:0:24:61:-36
113,NecroDancer Stage Turquoise,necrodancer_stage:24:0:24:61:-36
114,NecroDancer Stage Cyan,necrodancer_stage:48:0:24:61:-36
115,NecroDancer Speaker 1,necrodancer_stage:72:0:24:61:-36
116,NecroDancer Speaker 2,necrodancer_stage:96:0:24:61:-36
117,NecroDancer Speaker 3,necrodancer_stage:120:0:24:61:-36
118,Wired Door,door_front:0:0:24:29:-15:20

[Traps]

# Objects and Object Variants are defined similarly to Tiles and Tile Variants,
# with the exception of the variant image definition: ImageFile:CropX:CropY:CropWidth:CropHeight:Flipped

# If the Flipped parameter is set to "flipped", the image is mirrored horizontally.

1,Bounce Trap,bouncetrap:0:16:14:16:flipped,bouncetrap:0:16:14:16,bouncetrap:0:32:14:16,bouncetrap:0:0:14:16,diagonal_bouncetrap:0:16:14:16,diagonal_bouncetrap:0:0:14:16,diagonal_bouncetrap:0:48:14:16,diagonal_bouncetrap:0:32:14:16,bouncetrap_omni:0:0:14:16,ab_teleporttrap:0:0:14:16
2,Spike Trap,spiketrap:0:0:34:20
3,Trapdoor,trapdoor:0:0:25:20
4,Confuse Trap,confusetrap:14:0:14:16
5,Teleport Trap,teleporttrap:14:0:14:16
6,Slowdown Trap,slowdowntrap:14:0:14:16
7,Speedup Trap,speeduptrap:14:0:14:16
8,Travel Rune,null,travelrune1:24:0:24:24,travelrune2:24:0:24:24,travelrune3:24:0:24:24,travelrune4:24:0:24:24,travelrune5:24:0:24:24,travelrune6:24:0:24:24,travelrune7:24:0:24:24,travelrune8:24:0:24:24
9,Bomb Trap,bombtrap:14:0:14:16
10,Fireball Trap,firetrap:34:0:34:23,null,firetrap:34:0:34:23:flipped
14,Scatter Trap,scattertrap:14:0:14:16

[Crates]

0,Crate,crate:0:0:24:24
1,Barrel,barrel:0:0:28:27
2,Teh Urn,teh_urn:0:0:24:28

[Chests]

1,Red Chest,chest:0:0:24:24
2,Purple Chest,chest_white:0:0:24:24
3,Black Chest,chest_black:0:0:24:24
4,Blue Chest,chest_locked:0:0:24:24

[Shrines]

0,Shrine of Blood,shrine_blood:0:0:35:52
1,Shrine of Darkness,shrine_darkness:0:0:35:52
2,Shrine of Glass,shrine_glass:0:0:35:52
3,Shrine of Peace,shrine_peace:0:0:35:52
4,Shrine of Rhythm,shrine_rhythm:0:0:35:52
5,Shrine of Risk,shrine_risk:0:0:35:52
6,Shrine of Sacrifice,shrine_sacrifice:0:0:35:52
7,Shrine of Space,shrine_space:0:0:35:52
8,Shrine of War,shrine_war:0:0:35:52
9,Shrine of No Return,shrine_no_return:0:0:35:52
10,Shrine of Phasing,shrine_phasing:0:0:35:52
11,Shrine of Pace,shrine_pace:0:0:35:52
12,Shrine of Chance,shrine_chance:0:0:35:52
13,Boss Shrine,shrine_boss:0:0:30:46
14,Shrine of Pain,shrine_pain:0:0:22:50
15,Shrine of Uncertainty,shrine_uncertainty:0:0:24:49

[Characters]

# Format: CharacterID,CharacterName,HeadImage,BodyImage
# Both HeadImage and BodyImage use the format: ImageFile:CropX:CropY:CropWidth:CropHeight

-1,Any,player4_heads:0:0:24:24,player4_armor_body:0:0:24:24
0,Cadence,player1_heads:0:0:24:24,player1_armor_body:0:0:24:24
1,Melody,char1_heads:0:0:24:24,char1_armor_body:0:0:24:24
2,Aria,char2_heads:0:0:24:24,char2_armor_body:0:0:24:24
3,Dorian,char3_heads:4:4:24:24,char3_armor_body:4:4:24:24
4,Eli,char4_heads:4:4:24:24,char4_armor_body:4:4:24:24
5,Monk,char5_heads:0:0:24:24,char5_armor_body:0:0:24:24
6,Dove,char6_heads:0:0:24:24,char6_armor_body:0:0:24:24
7,Coda,char7_heads:4:6:24:24,char7_armor_body:4:6:24:24
8,Bolt,char8_heads:0:0:24:24,char8_armor_body:0:0:24:24
9,Bard,char9_heads:0:0:24:24,char9_armor_body:0:0:24:24
10,Nocturna,char10_heads:1:3:24:24,char10_armor_body:1:3:24:24
11,Diamond,char11_heads:0:0:24:24,char11_armor_body:0:0:24:24
12,Mary,char12_heads:0:0:24:24,char12_armor_body:0:0:24:24
13,Tempo,char13_heads:0:0:24:24,char13_armor_body:0:0:24:24
#14,Reaper,char14_heads:0:0:24:24,char14_armor_body:0:0:24:24

[Bosses]

# Format: BossID,BossName

-1,No Boss
12,King Conga 1
13,King Conga 2
14,King Conga 3
15,King Conga 4
16,King Conga 5
17,Death Metal 1
18,Death Metal 2
19,Death Metal 3
20,Death Metal 4
21,Death Metal 5
22,Deep Blues 1
23,Deep Blues 2
24,Deep Blues 3
25,Deep Blues 4
26,Deep Blues 5
27,Coral Riff 1
28,Coral Riff 2
29,Coral Riff 3
30,Coral Riff 4
31,Coral Riff 5
32,Fortissimole 1
33,Fortissimole 2
34,Fortissimole 3
35,Fortissimole 4
36,Fortissimole 5

[Music]

# Format: MusicID,MusicName

-1,No Music
0,1-1
1,1-2
2,1-3
3,2-1
4,2-2
5,2-3
6,3-1
7,3-2
8,3-3
9,4-1
10,4-2
11,4-3
12,5-1
13,5-2
14,5-3

[Zones]

Zone 1
Zone 2
Zone 3 Hot
Zone 3 Cold
Zone 4
Boss
Zone 5

[Actions]

# Action buttons for the toolbar.
# Format: Action,IconEntry,Label

file_new,43,New Dungeon
file_open,44,Open Dungeon...
file_save,45,Save Dungeon
file_save_as,46,Save Dungeon As...
reset_camera,97,Reset Camera

[Tools]

# Tool buttons for the toolbar.
# Format: Tool,IconEntry,Label

#select_rect,82,Rectangular Selection
place_tile,91,Place Tiles
place_enemy,93,Place Enemies
place_item,95,Place Items
place_misc,94,Place Objects
set_player_spawn,96,Set Player Spawn Point
