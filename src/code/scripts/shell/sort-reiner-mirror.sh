#!/bin/sh

USAGE="
Usage : "`basename $0`" <Reiner's mirror> <target repository for sorted content> : sorts automatically a mirror of Reiner's Tileset and put the sorted content in specified directory."


# Recommended usage :
# 1. Perform a very slow wget (to avoid disturbing anybody) on Reiner's site :
# wget --limit-rate=10k --wait=5 --random-wait --retry-connrefused --recursive --level=10 http://reinerstileset.4players.de
# 2. Archive it in ~/Archive/Content/<date>-reiners-tileset-mirror.tar.bz2
# 3. Extract a copy of this archive to be used here as Reiner's mirror 
# (its content will be removed)


if [ $# -neq 2 ] ; then
	echo "
	
	Error, two arguments expected. $USAGE
	
	" 1>&2
	exit 1
fi

MIRROR_ROOT="$1"

if [ ! -d "${MIRROR_ROOT}" ] ; then
	echo "
	
	Error, no directory named <${TARGET_ROOT}> exists for mirror. $USAGE
	
	" 1>&2
	exit 2
fi

TARGET_ROOT="$2"

if [ ! -d "${TARGET_ROOT}" ] ; then
	echo "
	
	Error, no directory named <${TARGET_ROOT}> exists for target repository. $USAGE
	
	" 1>&2
	exit 3
fi
	
echo "

	Will sort Reiner's tileset mirror now
	
	"

if [ ! -f "englisch.htm" ] ; then
	echo "
	
	Error, not in Reiner mirror root ?
	" 1>&2
	exit 4
fi


# Anticipated checkings :
CORRECTER=`dirname $0`/correctPathsInTree.sh

if [ ! -x "${CORRECTER}" ] ; then
	echo "
	
	Error, no executable correcter script found (searched ${CORRECTER}).
	
	" 1>&2
	exit 5

fi


cd ${MIRROR_ROOT}

echo "  + removing non-wanted content"


# M_ are MMF files :
find . \( -name '*.exe' -o -name '*.htm*' -o -name '*.wmv' -o -name 'M*.zip'  \) -exec /bin/rm -f '{}' ';'

echo "  + normalizing paths"
${CORRECTER} .

for f in `find . -name 'T-*.zip'` ; do
	corrected=`echo $f | tr '-' '_'`
	mv -f $f $corrected	
done 

# No Tgrundvari.zip, we want T_grundvari.zip :
for f in `find . -name 'T*.zip'` ; do
	if [ `echo $f | cut -b 4` != '_' ] ; then
		corrected=`echo $f | sed 's|^./T|./T_|1'`
		mv -f $f $corrected
	fi
done 

rm -rf forum uvmappingtut_-htm_files screenshots buttons handshaped_3d_tree_htm_files ext mmfabsbeg

rm AAspritesharpborder1-4.zip bass.zip enemie-ai-for-more-enemies-final-.zip gesleiste.zip isotiletuteng.zip T_Sbonestut.zip

mv 8Dshooting-fish.zip T_shooting_fish.zip
mv T_könig.zip T_konig.zip
mv ACTanithings-draho-1.zip T_various_objects.zip
mv ASdreh.zip T_rotating_chain_trap.zip
mv ASindy.zip T_nailed_ball_trap.zip
mv Bgesch.zip T_cannon.zip
mv Bjackbox.zip T_jack_in_the_box.zip
mv boxen.zip T_gui_boxen.zip
mv Bminen.zip T_ground_mines.zip
mv Braumschiffe.zip T_spaceship.zip
mv buttons2.zip T_buttons2.zip
mv mecha.zip T_mechanisms.zip
mv SBitemchest.zip T_item_chest.zip
mv SBspacepir.zip T_space_set.zip
mv SBvoscheu.zip T_scarecrow.zip
mv schriften.zip T_other_buttons.zip
mv symbols.zip T_gui_symbols.zip
mv T_ACTjewelpicker.zip T_jewel-picker-gui.zip
mv T_ani_fir.zip T_ani_fir_tree.zip


##########################################################################

# Directory definitions :

# In root :

GRAPHICAL_ROOT=${TARGET_ROOT}/Graphics
mkdir ${GRAPHICAL_ROOT}

SOUND_ROOT=${TARGET_ROOT}/Sounds
mkdir ${SOUND_ROOT}


# In Graphics :

CREATURES=${GRAPHICAL_ROOT}/Creatures
mkdir ${CREATURES}

OBJECTS=${GRAPHICAL_ROOT}/Objects
mkdir ${OBJECTS}

ENVIRONMENTS=${GRAPHICAL_ROOT}/Environments
mkdir ${ENVIRONMENTS}

SPRITES=${GRAPHICAL_ROOT}/Sprites
mkdir ${SPRITES}

TILESETS=${GRAPHICAL_ROOT}/Tilesets
mkdir ${TILESETS}


# In Creatures :

HUMANS=${CREATURES}/Humans
mkdir ${HUMANS}

MONSTERS=${CREATURES}/Monsters
mkdir ${MONSTERS}

ANIMALS=${CREATURES}/Animals
mkdir ${ANIMALS}


# In Humans :

ARCHERS=${HUMANS}/Archers
mkdir ${ARCHERS}

KNIGHTS=${HUMANS}/Knights
mkdir ${KNIGHTS}

CAVEMEN=${HUMANS}/Cavemen
mkdir ${CAVEMEN}

MAGES=${HUMANS}/Mages
mkdir ${MAGES}

BARBARIANS=${HUMANS}/Barbarians
mkdir ${BARBARIANS}

NINJAS=${HUMANS}/Ninjas
mkdir ${NINJAS}

FISHERMEN=${HUMANS}/Fishermen
mkdir ${FISHERMEN}

BEEKEEPERS=${HUMANS}/Beekeepers
mkdir ${BEEKEEPERS}

MILLERS=${HUMANS}/Millers
mkdir ${MILLERS}

SOLDIERS=${HUMANS}/Soldiers
mkdir ${SOLDIERS}

ROYALTY=${HUMANS}/Royalty
mkdir ${ROYALTY}

SWORDSMEN=${HUMANS}/Swordsmen
mkdir ${SWORDSMEN}

CARRIERS=${HUMANS}/Carriers
mkdir ${CARRIERS}

FARMERS=${HUMANS}/Farmers
mkdir ${FARMERS}

HUNTERS=${HUMANS}/Hunters
mkdir ${HUNTERS}

MINERS=${HUMANS}/Miners
mkdir ${MINERS}

THIEVES=${HUMANS}/Thieves
mkdir ${THIEVES}

BAKERS=${HUMANS}/Bakers
mkdir ${BAKERS}

CHILDREN=${HUMANS}/Children
mkdir ${CHILDREN}

BUILDERS=${HUMANS}/Builders
mkdir ${BUILDERS}

DOCTORS=${HUMANS}/Doctors
mkdir ${DOCTORS}

CHARACTERS=${HUMANS}/NamedCharacters
mkdir ${CHARACTERS}


# In NamedCharacters :

VLAD=${CHARACTERS}/Vlad
mkdir ${VLAD}

WILLY=${CHARACTERS}/Willy
mkdir ${WILLY}

BJORN=${CHARACTERS}/Bjorn
mkdir ${BJORN}

STAN=${CHARACTERS}/Stan
mkdir ${STAN}

FREYA=${CHARACTERS}/Freya
mkdir ${FREYA}

HORST=${CHARACTERS}/Horst
mkdir ${HORST}

KATIE=${CHARACTERS}/Katie
mkdir ${KATIE}

ANNA=${CHARACTERS}/Anna
mkdir ${ANNA}

ARNO=${CHARACTERS}/Arno
mkdir ${ARNO}

BILLY=${CHARACTERS}/Billy
mkdir ${BILLY}

BURRA=${CHARACTERS}/Burra
mkdir ${BURRA}


# In Animals :

FISH=${ANIMALS}/Fish
mkdir ${FISH}

INSECTS=${ANIMALS}/Insects
mkdir ${INSECTS}

WOLVES=${ANIMALS}/Wolves
mkdir ${WOLVES}

SPIDERS=${ANIMALS}/Spiders
mkdir ${SPIDERS}

LIONS=${ANIMALS}/Lions
mkdir ${LIONS}

SHEEP=${ANIMALS}/Sheep
mkdir ${SHEEP}


# In Insects :

WASPS=${INSECTS}/Wasps
mkdir ${WASPS}


# In Monsters :

DRAGONS=${MONSTERS}/Dragons
mkdir ${DRAGONS}

DWARVES=${MONSTERS}/Dwarves
mkdir ${DWARVES}

GNOMES=${MONSTERS}/Gnomes
mkdir ${GNOMES}

TROLLS=${MONSTERS}/Trolls
mkdir ${TROLLS}

SKELETONS=${MONSTERS}/Skeletons
mkdir ${SKELETONS}

ZOMBIES=${MONSTERS}/Zombies
mkdir ${ZOMBIES}

RATS=${MONSTERS}/Rats
mkdir ${RATS}

CROCODILES=${MONSTERS}/Crocodiles
mkdir ${CROCODILES}

DINOSAURS=${MONSTERS}/Dinosaurs
mkdir ${DINOSAURS}

DOTEATERS=${MONSTERS}/DotEaters
mkdir ${DOTEATERS}


# In Objects :

TRAPS=${OBJECTS}/Traps
mkdir ${TRAPS}

VEHICLES=${OBJECTS}/Vehicles
mkdir ${VEHICLES}

FOOD=${OBJECTS}/Food
mkdir ${FOOD}

MECHANISMS=${OBJECTS}/Mechanisms
mkdir ${MECHANISMS}

WEAPONS=${OBJECTS}/Weapons
mkdir ${WEAPONS}

FURNITURE=${OBJECTS}/Furniture
mkdir ${FURNITURE}


# In Vehicles :

BOATS=${VEHICLES}/Boats
mkdir ${BOATS}

SPACESHIPS=${VEHICLES}/Spaceships
mkdir ${SPACESHIPS}

CARS=${VEHICLES}/Cars
mkdir ${CARS}

TRAINS=${VEHICLES}/Trains
mkdir ${TRAINS}


# In Environments :

CASTLES=${ENVIRONMENTS}/Castles
mkdir ${CASTLES}

HILLS=${ENVIRONMENTS}/Hills
mkdir ${HILLS}

WALLS=${ENVIRONMENTS}/Walls
mkdir ${WALLS}

BUILDINGS=${ENVIRONMENTS}/Buildings
mkdir ${BUILDINGS}

PLANTS=${ENVIRONMENTS}/Plants
mkdir ${PLANTS}

GROUNDS=${ENVIRONMENTS}/Grounds
mkdir ${GROUNDS}

SETS=${ENVIRONMENTS}/Sets
mkdir ${SETS}


# In Plants :

TREES=${PLANTS}/Trees
mkdir ${TREES}

FLOWERS=${PLANTS}/Flowers
mkdir ${FLOWERS}


# In Walls :

INSIDE=${WALLS}/Inside
mkdir ${INSIDE}

OUTSIDE=${WALLS}/Outside
mkdir ${OUTSIDE}


# In Sprites :

GUI=${SPRITES}/GUI
mkdir ${GUI}



##########################################################################

# Transfer rules :

mv sounds/*   ${SOUND_ROOT}
mv sprites/*  ${SPRITES}
mv *menu* *button* *gui* *butscr* ${GUI}
mv tilesets/* ${TILESETS}


# Humans :

mv *archer*         ${ARCHERS}
mv *knight*         ${KNIGHTS}
mv *caveman*        ${CAVEMEN}
mv *mage*           ${MAGES}
mv *barbar*         ${BARBARIANS}
mv *ninja*          ${NINJAS}
mv *fisherman*      ${FISHERMEN}
mv *beekeeper*      ${BEEKEEPERS}
mv *miller*         ${MILLERS}
mv *soldier*        ${SOLDIERS}
mv *konig* *prince* ${ROYALTY}
mv *swordsman*      ${SWORDSMEN}
mv *carrier*        ${CARRIERS}
mv *farmer*         ${FARMERS}
mv *hunter*         ${HUNTERS}
mv *miner*          ${MINERS}
mv *thief*          ${THIEVES}
mv *baker.*         ${BAKERS}
mv *boy* *girl*     ${CHILDREN}
mv *builder*        ${BUILDERS}
mv *doc.*           ${DOCTORS}


# Named characters :

mv *vlad*  ${VLAD}
mv *willy* ${WILLY}
mv *bjorn* ${BJORN}
mv *stan*  ${STAN}
mv *freya* ${FREYA}
mv *horst* ${HORST}
mv *katie* ${KATIE}
mv *anna*  ${ANNA}
mv *arno*  ${ARNO}
mv *billy* ${BILLY}
mv *burra* ${BURRA}


# Animals :

mv *donkey* *bat* *chicken* *cow* *crow* *deer* ${ANIMALS}

mv *dragonfly* *butterfly* ${INSECTS}
mv *fish*   ${FISH}
mv *wasp*   ${WASPS}
mv *spider* ${SPIDERS}
mv *wolf*   ${WOLVES}
mv *lion*   ${LIONS}
mv *sheep*  ${SHEEP}


# Monsters :

mv *dragon*   ${DRAGONS}
mv *dwarf*    ${DWARVES}
mv *gnome*    ${GNOMES}
mv *troll*    ${TROLLS}
mv *skel*     ${SKELETONS}
mv *zombie*   ${ZOMBIES}
mv *ratz*     ${RATS}
mv *crocy*    ${CROCODILES}
mv *dino*     ${DINOSAURS}
mv *doteater* ${DOTEATERS}

mv *devil* *ogre* ${MONSTERS}


# Objects :


# Weapons :
mv *cannon* *arrow* *tnt* ${WEAPONS}
 
# Mechanisms : 
mv *mecha* *lever* ${MECHANISMS}

# Traps :

mv *trap*  ${TRAPS}
mv *mines* ${TRAPS}


mv *flamewall* *doormask* *wallpictures* *objects* *box* *item* *scare* *aura* *things* *gym* ${OBJECTS}


# Pieces of furniture :
mv *beds* *carpet* *chest* *door* ${FURNITURE}


# Vehicles :

# Boats :
mv *schiff* *boot* ${BOATS}

# Spaceships :
mv *spaceship* ${SPACESHIPS}

# Cars :
mv *rallye* ${CARS}

# Trains :
mv *waggon* ${TRAINS}



# Food :
mv *meat* ${FOOD}



# Environments :

mv *castle* ${CASTLES}

mv *hills* ${HILLS}

mv *set* *anitown* *beautiful_town* *casino* *cavestuff* ${SETS}

# Walls :

mv *inside_wall* ${INSIDE}
mv *wall* *parapet* ${OUTSIDE}


# Buildings :

mv *house* *mill* *bakery* *barracks* *chapel* *crabuil* *building* ${BUILDINGS}


# Plants :

mv *tree* *bush* ${TREES}
mv **             ${FLOWERS}

# Grounds :
mv *earth* *wall* *beach* *cobble* *creek* *sand* *ground* *stonepath* ${GROUNDS}

