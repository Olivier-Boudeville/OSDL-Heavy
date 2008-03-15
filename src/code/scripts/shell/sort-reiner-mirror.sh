#!/bin/sh

USAGE="
Usage: "`basename $0`" <Reiner's mirror> <target repository for sorted content>: sorts automatically a mirror of Reiner's Tileset and put the sorted content in specified directory."

MV=/bin/mv
RM=/bin/rm
MKDIR=/bin/mkdir
RMDIR=/bin/rmdir

ERROR_OUTPUT="2>/dev/null"

# Recommended usage:
# 1. Perform a very slow wget (to avoid disturbing anybody) on Reiner's site:
# wget --limit-rate=10k --wait=5 --random-wait --retry-connrefused --recursive --level=10 --reject 'M_*.zip' --exclude-directories=forum http://reinerstileset.4players.de 
# 2. Archive it in ~/Archive/Content/<date>-reiners-tileset-mirror.tar.bz2
# 3. Extract a copy of this archive to be used here as Reiner's mirror 
# (its content will be removed)

if [ ! $# -eq 2 ] ; then
	echo "
	Error, two arguments expected. $USAGE
	
	" 1>&2
	exit 1
fi

MIRROR_ROOT="$1"

if [ ! -d "${MIRROR_ROOT}" ] ; then
	echo "
	Error, no directory named <${MIRROR_ROOT}> exists for mirror. $USAGE
	
	" 1>&2
	exit 2
fi

TARGET_ROOT="$2"

# Absolute path only:
first_char=`echo ${TARGET_ROOT} | cut -b 1`
if [ ${first_char} != "/" ] ; then
	TARGET_ROOT=`pwd`/${TARGET_ROOT}
fi


if [ ! -d "${TARGET_ROOT}" ] ; then
	echo "
	Error, no directory named <${TARGET_ROOT}> exists for target repository. $USAGE
	
	" 1>&2
	exit 3
fi
	

# Look in Ceylan sources:
CEYLAN_LOANI_OFFSET="../../../../../../../ceylan/Ceylan/trunk/src/code/scripts/shell/"

# Anticipated checkings:
CORRECTER=`pwd`"/"`dirname $0`"/${CEYLAN_LOANI_OFFSET}/correctPathsInTree.sh"

cd ${MIRROR_ROOT}

if [ ! -x "${CORRECTER}" ] ; then
	echo "
	Error, no executable correcter script found (searched ${CORRECTER}).
	
	" 1>&2
	exit 5

fi


if [ ! -f "englisch.htm" ] ; then
	echo "
	Error, not in Reiner mirror root ? (here is "`pwd`")
	" 1>&2
	exit 4
fi


echo "

	Will sort Reiner's tileset mirror now...
	
	"

echo "  + removing non-wanted content"


# M_* are MMF files:
find . -depth \( -name '*.exe' -o -name '*.htm*' -o -name '*.wmv' -o -name 'M*.zip'  \) -exec ${RM} -f '{}' ';'

# Supress non-wanted directories:
${RM} -rf forum *uvmappingtut* screenshots buttons handshaped_3d_tree_htm_files ext mmfabsbeg

# Supress non-wanted files:
${RM} AAspritesharpborder1-4.zip bass.zip *'enemie ai'*.zip gesleiste.zip isotiletuteng.zip *Tutorial* *Sbonestut.zip *jumpengine* 2>/dev/null


echo "  + normalizing paths"

echo "    - correcting paths"
${CORRECTER} . | grep -v 'left unchanged'

echo "    - correcting underscores #1"
# 'T ' became 'T-', we want 'T_' for uniformity:
for f in `find . -name 'T-*.zip'` ; do
	corrected=`echo $f | tr '-' '_'`
	${MV} -f $f $corrected	
done 

echo "    - correcting underscores #2"
# No 'Tgrundvari.zip', we want 'T_grundvari.zip':
for f in `find . -depth -name 'T*.zip'` ; do
	fourth_char=`echo $f | cut -b 4`
	if [ "${fourth_char}" != "_" ] ; then
		corrected=`echo $f | sed 's|^./T|./T_|1'`
		${MV} -f $f $corrected
	fi
done 


# Rename for convenience some files:
${MV} 8Dshooting-fish.zip T_shooting_fish.zip
${MV} T_könig.zip T_konig.zip
${MV} ACTanithings-draho-1.zip T_various_objects.zip
${MV} ASdreh.zip T_rotating_chain_trap.zip
${MV} ASindy.zip T_nailed_ball_trap.zip
${MV} Bgesch.zip T_cannon.zip
${MV} Bjackbox.zip T_jack_in_the_box.zip
${MV} boxen.zip T_gui_boxen.zip
${MV} Bminen.zip T_ground_mines.zip
${MV} Braumschiffe.zip T_spaceship.zip
${MV} buttons2.zip T_buttons2.zip
${MV} mecha.zip T_mechanisms.zip
${MV} SBitemchest.zip T_item_chest.zip
${MV} SBspacepir.zip T_space_set.zip
${MV} SBvoscheu.zip T_scarecrow.zip
${MV} schriften.zip T_other_buttons.zip
${MV} symbols.zip T_gui_symbols.zip
${MV} T_ACTjewelpicker.zip T_jewel-picker-gui.zip
${MV} T_ani_fir.zip T_ani_fir_tree.zip
${MV} T_john_doe.zip T_john_doe_santa.zip



##########################################################################

# Directory definitions (sorted alphabetically):

# In root:

GRAPHICAL_ROOT=${TARGET_ROOT}/Graphics
${MKDIR} ${GRAPHICAL_ROOT}

SOUND_ROOT=${TARGET_ROOT}/Sounds
${MKDIR} ${SOUND_ROOT}


# In Graphics:

CREATURES=${GRAPHICAL_ROOT}/Creatures
${MKDIR} ${CREATURES}

ENVIRONMENTS=${GRAPHICAL_ROOT}/Environments
${MKDIR} ${ENVIRONMENTS}

OBJECTS=${GRAPHICAL_ROOT}/Objects
${MKDIR} ${OBJECTS}

SPRITES=${GRAPHICAL_ROOT}/Sprites
${MKDIR} ${SPRITES}

TILESETS=${GRAPHICAL_ROOT}/Tilesets
${MKDIR} ${TILESETS}


# In Creatures:

ANIMALS=${CREATURES}/Animals
${MKDIR} ${ANIMALS}

HUMANS=${CREATURES}/Humans
${MKDIR} ${HUMANS}

MONSTERS=${CREATURES}/Monsters
${MKDIR} ${MONSTERS}



# In Humans:

ARCHERS=${HUMANS}/Archers
${MKDIR} ${ARCHERS}

BAKERS=${HUMANS}/Bakers
${MKDIR} ${BAKERS}

BARBARIANS=${HUMANS}/Barbarians
${MKDIR} ${BARBARIANS}

BEEKEEPERS=${HUMANS}/Beekeepers
${MKDIR} ${BEEKEEPERS}

SMITHS=${HUMANS}/Blacksmiths
${MKDIR} ${SMITHS}

BUILDERS=${HUMANS}/Builders
${MKDIR} ${BUILDERS}

CARRIERS=${HUMANS}/Carriers
${MKDIR} ${CARRIERS}

CAVEMEN=${HUMANS}/Cavemen
${MKDIR} ${CAVEMEN}

CHILDREN=${HUMANS}/Children
${MKDIR} ${CHILDREN}

DOCTORS=${HUMANS}/Doctors
${MKDIR} ${DOCTORS}

FARMERS=${HUMANS}/Farmers
${MKDIR} ${FARMERS}

FISHERMEN=${HUMANS}/Fishermen
${MKDIR} ${FISHERMEN}

HUNTERS=${HUMANS}/Hunters
${MKDIR} ${HUNTERS}

KNIGHTS=${HUMANS}/Knights
${MKDIR} ${KNIGHTS}

LUMBERJACKS=${HUMANS}/Lumberjacks
${MKDIR} ${LUMBERJACKS}

MAGES=${HUMANS}/Mages
${MKDIR} ${MAGES}

MILLERS=${HUMANS}/Millers
${MKDIR} ${MILLERS}

MINERS=${HUMANS}/Miners
${MKDIR} ${MINERS}

MONKS=${HUMANS}/Monks
${MKDIR} ${MONKS}

CHARACTERS=${HUMANS}/NamedCharacters
${MKDIR} ${CHARACTERS}

NINJAS=${HUMANS}/Ninjas
${MKDIR} ${NINJAS}

PIRATES=${HUMANS}/Pirates
${MKDIR} ${PIRATES}

PROFESSORS=${HUMANS}/Professors
${MKDIR} ${PROFESSORS}

ROYALTY=${HUMANS}/Royalty
${MKDIR} ${ROYALTY}

SOLDIERS=${HUMANS}/Soldiers
${MKDIR} ${SOLDIERS}

SWORDSMEN=${HUMANS}/Swordsmen
${MKDIR} ${SWORDSMEN}

THIEVES=${HUMANS}/Thieves
${MKDIR} ${THIEVES}



# In NamedCharacters:

ANNA=${CHARACTERS}/Anna
${MKDIR} ${ANNA}

ARNO=${CHARACTERS}/Arno
${MKDIR} ${ARNO}

BILLY=${CHARACTERS}/Billy
${MKDIR} ${BILLY}

BJORN=${CHARACTERS}/Bjorn
${MKDIR} ${BJORN}

BURRA=${CHARACTERS}/Burra
${MKDIR} ${BURRA}

ELSA=${CHARACTERS}/Elsa
${MKDIR} ${ELSA}

FREYA=${CHARACTERS}/Freya
${MKDIR} ${FREYA}

HORST=${CHARACTERS}/Horst
${MKDIR} ${HORST}

JOHN=${CHARACTERS}/JohnDoe
${MKDIR} ${JOHN}

KATIE=${CHARACTERS}/Katie
${MKDIR} ${KATIE}

LILLY=${CHARACTERS}/Lilly
${MKDIR} ${LILLY}

LUIGI=${CHARACTERS}/Luigi
${MKDIR} ${LUIGI}

LUZIA=${CHARACTERS}/Luzia
${MKDIR} ${LUZIA}

MRSRUIN=${CHARACTERS}/MrsRuin
${MKDIR} ${MRSRUIN}

ROSALILA=${CHARACTERS}/Rosalila
${MKDIR} ${ROSALILA}

STAN=${CHARACTERS}/Stan
${MKDIR} ${STAN}

VLAD=${CHARACTERS}/Vlad
${MKDIR} ${VLAD}

WILLY=${CHARACTERS}/Willy
${MKDIR} ${WILLY}



# In Animals:

FISH=${ANIMALS}/Fish
${MKDIR} ${FISH}

INSECTS=${ANIMALS}/Insects
${MKDIR} ${INSECTS}

LIONS=${ANIMALS}/Lions
${MKDIR} ${LIONS}

SHEEP=${ANIMALS}/Sheep
${MKDIR} ${SHEEP}

SPIDERS=${ANIMALS}/Spiders
${MKDIR} ${SPIDERS}

WOLVES=${ANIMALS}/Wolves
${MKDIR} ${WOLVES}


# In Insects:

WASPS=${INSECTS}/Wasps
${MKDIR} ${WASPS}


# In Monsters:

CYCLOPS=${MONSTERS}/Cyclops
${MKDIR} ${CYCLOPS}

DINOSAURS=${MONSTERS}/Dinosaurs
${MKDIR} ${DINOSAURS}

DOTEATERS=${MONSTERS}/DotEaters
${MKDIR} ${DOTEATERS}

DRAGONS=${MONSTERS}/Dragons
${MKDIR} ${DRAGONS}

DWARVES=${MONSTERS}/Dwarves
${MKDIR} ${DWARVES}

GNOMES=${MONSTERS}/Gnomes
${MKDIR} ${GNOMES}

RATS=${MONSTERS}/Rats
${MKDIR} ${RATS}

REPTILES=${MONSTERS}/Reptiles
${MKDIR} ${REPTILES}

SKELETONS=${MONSTERS}/Skeletons
${MKDIR} ${SKELETONS}

TROLLS=${MONSTERS}/Trolls
${MKDIR} ${TROLLS}

ZOMBIES=${MONSTERS}/Zombies
${MKDIR} ${ZOMBIES}



# In Objects:


FOOD=${OBJECTS}/Food
${MKDIR} ${FOOD}

FURNITURE=${OBJECTS}/Furniture
${MKDIR} ${FURNITURE}

INTANGIBLE=${OBJECTS}/Intangible
${MKDIR} ${INTANGIBLE}

MECHANISMS=${OBJECTS}/Mechanisms
${MKDIR} ${MECHANISMS}

TRAPS=${OBJECTS}/Traps
${MKDIR} ${TRAPS}

VEHICLES=${OBJECTS}/Vehicles
${MKDIR} ${VEHICLES}

WEAPONS=${OBJECTS}/Weapons
${MKDIR} ${WEAPONS}



# In Vehicles:

BOATS=${VEHICLES}/Boats
${MKDIR} ${BOATS}

SPACESHIPS=${VEHICLES}/Spaceships
${MKDIR} ${SPACESHIPS}

CARS=${VEHICLES}/Cars
${MKDIR} ${CARS}

TRAINS=${VEHICLES}/Trains
${MKDIR} ${TRAINS}

ARMORED=${VEHICLES}/Armored
${MKDIR} ${ARMORED}


# In Environments:

CASTLES=${ENVIRONMENTS}/Castles
${MKDIR} ${CASTLES}

HILLS=${ENVIRONMENTS}/Hills
${MKDIR} ${HILLS}

WALLS=${ENVIRONMENTS}/Walls
${MKDIR} ${WALLS}

STAIRS=${ENVIRONMENTS}/Stairs
${MKDIR} ${STAIRS}

BUILDINGS=${ENVIRONMENTS}/Buildings
${MKDIR} ${BUILDINGS}

PLANTS=${ENVIRONMENTS}/Plants
${MKDIR} ${PLANTS}

GROUNDS=${ENVIRONMENTS}/Grounds
${MKDIR} ${GROUNDS}

SETS=${ENVIRONMENTS}/Sets
${MKDIR} ${SETS}


# In Plants:

TREES=${PLANTS}/Trees
${MKDIR} ${TREES}



# In Walls:

INSIDE=${WALLS}/Inside
${MKDIR} ${INSIDE}

OUTSIDE=${WALLS}/Outside
${MKDIR} ${OUTSIDE}


# In Sprites:

GUI=${SPRITES}/GUI
${MKDIR} ${GUI}



##########################################################################

# Transfer rules:

${MV} sounds/*   ${SOUND_ROOT} 
${MV} sprites/*  ${SPRITES} 
${MV} tilesets/* ${TILESETS} 
${RMDIR} sounds sprites tilesets

${MV} *menu* *button* *gui* *butscr* *levelup* *pointrunes* ${GUI}  


# Creatures:

${MV} *groggy* ${CREATURES} 


# Humans:

${MV} *flying_soul*    ${HUMANS} 

${MV} *archer*          ${ARCHERS} 
${MV} *baker.*          ${BAKERS} 
${MV} *barbar*          ${BARBARIANS} 
${MV} *beekeeper*       ${BEEKEEPERS} 
${MV} *boy* *girl*      ${CHILDREN} 
${MV} *builder*         ${BUILDERS} 
${MV} *carrier*         ${CARRIERS} 
${MV} *caveman*         ${CAVEMEN} 
${MV} *doc.*            ${DOCTORS} 
${MV} *farmer*          ${FARMERS} 
${MV} *fisherman*       ${FISHERMEN} 
${MV} *hunter*          ${HUNTERS} 
${MV} *knight*          ${KNIGHTS} 
${MV} *konig* *prince*  ${ROYALTY} 
${MV} *lumberjack*      ${LUMBERJACKS} 
${MV} *mage* *magier*   ${MAGES} 
${MV} *miller*          ${MILLERS} 
${MV} *miner*           ${MINERS} 
${MV} *monk*            ${MONKS} 
${MV} *ninja*           ${NINJAS} 
${MV} *pirsa* *nobeard* ${PIRATES} 
${MV} *professor*       ${PROFESSORS} 
${MV} *_smith.*         ${SMITHS} 
${MV} *soldier*         ${SOLDIERS} 
${MV} *swordsman*       ${SWORDSMEN} 
${MV} *thief*           ${THIEVES} 


# Named characters:

${MV} *anna*  ${ANNA} 
${MV} *arno*  ${ARNO} 
${MV} *billy* ${BILLY} 
${MV} *bjorn* ${BJORN} 
${MV} *burra* ${BURRA} 
${MV} *elsa*  ${ELSA} 
${MV} *freya* ${FREYA} 
${MV} *horst* ${HORST} 
${MV} *john_doe_santa* ${JOHN} 
${MV} *katie* ${KATIE} 
${MV} *lilly* ${LILLY} 
${MV} *luigi* ${LUIGI} 
${MV} *luzia* ${LUZIA} 
${MV} *mrs_ruin* ${MRSRUIN} 
${MV} *rosalila* ${ROSALILA} 
${MV} *stan*  ${STAN} 
${MV} *vlad*  ${VLAD} 
${MV} *willy* ${WILLY} 

# Animals:

${MV} *donkey* *bat.* *chicken* *cow* *crow* *deer* *_mouse.* *rabbit* *roebuck* *rooster* *schwein* ${ANIMALS} 

${MV} *dragonfly* *butterfly* ${INSECTS} 
${MV} *fish*   ${FISH} 
${MV} *wasp*   ${WASPS} 
${MV} *spider* ${SPIDERS} 
${MV} *wolf*   ${WOLVES} 
${MV} *lion*   ${LIONS} 
${MV} *sheep*  ${SHEEP} 

# Monsters:

${MV} *doteater*     ${DOTEATERS} 

${MV} *devil* *ogre* *fairy* *skull* *ghost* *theorc* *mummy* *mushy* *puff* *Sworm* ${MONSTERS} 

${MV} *dragon*       ${DRAGONS} 
${MV} *dwarf*        ${DWARVES} 
${MV} *gnome*        ${GNOMES} 
${MV} *troll*        ${TROLLS} 
${MV} *zombie*       ${ZOMBIES} 
${MV} *ratz*         ${RATS} 
${MV} *dino* *pteri* ${DINOSAURS} 
${MV} *mister_eye*   ${CYCLOPS} 
${MV} *crocy* *swampthing* ${REPTILES} 
${MV} *skel* *mister_death* ${SKELETONS} 



# Objects:


# Weapons:
${MV} *cannon* *arrow* *tnt* *ammo* *pickup_weapon* *rotating_spike* *spear_catapult* ${WEAPONS} 
 
# Mechanisms: 
${MV} *mecha* *lever* *rotating_switch* ${MECHANISMS} 

# Traps:
${MV} *trap* *mines* *spikefield* *spiketrench* ${TRAPS} 

# Intangible:
${MV} *flamewall* *aura* *feuer* *bubble* *leafstorm* *magic_ani* ${INTANGIBLE} 

# Pieces of furniture:
${MV} *beds* *carpet* *chest* *door* *furniture* *hedge_column* *keycolumn* *kitchen* *library* *pool_* *Sdrahomix* *shelfes* *shops_2* *throne_room* ${FURNITURE} 

${MV} *doormask* *wallpictures* *objects* *fields* *box* *item* *scare* *things* *gym* *lantern* *pickup_armor* *pickup_strategy* *_barrel* *rolling_stone* *sarkophag* *soulstones* *stoneheap* ${OBJECTS} 


# Vehicles:

# Boats:
${MV} *schiff* *boot* *grokanbo* *grpirschi* *klpirschi* ${BOATS} 

# Spaceships:
${MV} *spaceship* ${SPACESHIPS} 

# Cars:
${MV} *rallye* ${CARS} 

# Trains:
${MV} *waggon* *lore.* *steam_engine* *woodwag* ${TRAINS} 

# Armored:
${MV} *tank* ${ARMORED} 

# Food:
${MV} *meat* ${FOOD} 



# Environments:

${MV} *castle* ${CASTLES} 

${MV} *hill* ${HILLS} 

${MV} *set* *anitown* *beautiful_town* *casino* *cavestuff* *pipeline* *Sdraho* *Sdungeon* *thingmachine_all* *actrumb* ${SETS} 


# Walls:

${MV} *inside_wall* *quarry* *shops_1* *ug_fence* ${INSIDE} 
${MV} *wall* *parapet* *msw* *palisade* *woodenfence* ${OUTSIDE} 

# Stairs:
${MV} *stairs* ${STAIRS} 

# Buildings:

${MV} *house* *mill* *bakery* *barracks* *chapel* *crabuil* *building* *lodge* *hotel* *hut* *market_place* *_mine* *palace* *pottery* *_pub* *railroad* *ruins* *shipyard* *temple* *smithy* *_stable.* *triumph_arc* ${BUILDINGS}  


# Plants:

${MV} *tree* *bush* *growing_fir* ${TREES} 
${MV} *growing* *reef* ${PLANTS} 


# Grounds:
${MV} *earth* *wall* *beach* *cobble* *creek* *sand* *ground* *stonepath* *slope* *grund* *mountain* *pathway* *pfad* *quai* *_road_* *_rocks_* *tiled_floor* *weg_mit_randstein* *ani_water* ${GROUNDS} 


echo "End of mirror sorting."

