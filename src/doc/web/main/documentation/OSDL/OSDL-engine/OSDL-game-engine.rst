====
Orge
====

--------------------
OSDL RPG Game Engine
--------------------

:Author: Olivier Boudeville
:Contact: olivier.boudeville@esperide.com

:status: This is a work in progress
:copyright: 2008 Olivier Boudeville. Both this document and the game system it describes are released under a `Creative Commons <http://creativecommons.org>`_ license whose name is *Attribution-Noncommercial-Share Alike 3.0*. See `License for the game model (a.k.a. rules)`_ for further details.

:Dedication:

    For Orge users (scenario writers, game masters, players) & system co-designers.

:abstract:

    This document describes the **OSDL RPG Game Engine**, which is a role-playing game system, mainly dedicated to video games in an heroic-fantasy setting. It could be easily adapted as well for table-top playing and for others settings.
	
	**Orge** is a game system that tries to focus on generic yet lightweight guidelines and rules, by minimizing the number of concepts and associating them in a logical manner. It includes a model (a set of simulation rules) and its corresponding implementation. 

.. meta::
   :keywords: game system, role-playing, RPG, engine, video game, OSDL, Orge
   :description lang=en: A description of the Orge game system, containing examples of all basic rules and many advanced ones.

.. role:: raw-html(raw)
   :format: html
   
.. role:: raw-latex(raw)
   :format: latex

.. contents:: Table of Contents
.. section-numbering::


Overview
========

Orge, the **OSDL RPG Game Engine**, is a set of game rules to be used in role-playing games (RPG). 

Here we designate by *RPG Game Engine* the set of high-level rules that are used to simulate a virtual world, manually (table-top games with a real Game Master) or, more importantly here, with the help of a computer (notably in the case of a video game, with an emulated Game Master).

Concerns like rendering, input handling, etc. are deemed outside of the scope of the game engine: if the game was a networked one, we would focus here on the server-side simulation of the game actors only.

Our goal is to define here:

 - what are the general requirements of most of the Role-Playing Games we aim at, regarding the underlying game engine
 - what is the solution we preferred to apply in order to fulfill these needs, and why
 - how to use that solution (Orge) in practise





Some definitions first
----------------------

These following central terms will be used everywhere in this document.


Types of games
..............

Table-top Game
	This is the classical `role-playing game <http://en.wikipedia.org/wiki/Role-playing_game>`_, also known as *pen and paper* game, where participants meet in real life with little or no technical help (only documentation, dice, a cardboard screen, etc.).   

RPG Video Game, or Computer role-playing game (`CRPG <http://en.wikipedia.org/wiki/Computer_role-playing_game>`_)
	This was, originally, mostly a translation attempt of table-top games into a video game, even if it resulted in a rather different game genre.
	



Game Roles 
..........


System Designer (SD)
	This is the author of the Game System, i.e. of the generic conventions and rules that can be applied in all the kinds of supported games. Typically here the SD is us, the Orge authors, writers of the document you are reading. 

Scenario Writer (SW)
	This is the demiurge and story teller, who defines the world in which the action will take place, with its associated plots, quests, characters, etc. He will rely on the Game System provided by the SD to establish the basic rules of interaction, so that he can focus on the core of its subjects, the story. Note that the Scenario Writer defines a story only once, but it can result in any number of game instances based on that story.

Game Master (GM)
	This is the one that allows the players to interact with the game world, in the context of a story. His role is to animate a game instance based on a story created by a SW. 

Player 
	The player is interpreting the role of a character involved in the world recreated by the GM (*roleplay*). 


Of course one can cumulate multiple game roles: for example some GM prefer pre-cooked scenario, whereas others are SW as well. 	

The link between SW and GM exists for CRPG too, as the tools for scenario creation (editors for characters, objects, quests, maps, etc.) must be compatible with the actual RPG engine.

	
	
Characters
..........

		
Character
	Generally speaking, a Character corresponds to a sentient being. The definition has to be rather vague, as, beyond the classical men or women, a Character could be as well an alien or an advanced computer endowed with reason. Creatures deemed not smart enough to have elaborate feelings are not characters, they are sometimes called monsters.   
	
Player Character (PC)
	A PC is a Character that is interpreted by a player, who assumes his role and take control over many of his actions
	
Non-Player Character (NPC)
	A NPC is a Character that is not interpreted by a player, it has thus to be managed by the GM


Wrapping around

Usually there is exactly one Game Master, a few players, each interpreting only one particular Player Character, and a large number of beings, including monsters and NPC.

The game session may or may be split into sessions.

In the case of table-top games, a *game* follows a story line made usually of a set of *campaigns*, which are made of a series of *adventures*, which, for convenience reasons (due to duration above a few hours) are broken into a series of *game sessions*.

For RPG video games, either the world instance is created especially for a group of players, and then the world exist only while they are playing (be it a single player or multiplayer game), or the world instance lives continuously, i.e. is persistent be there players or not, in the case of a `MMORPG <http://en.wikipedia.org/wiki/MMORPG>`_ (*Massively Multiplayer Online Role-Playing Game*).

In both table-top and video games, an adventure includes generally a set of *quests*, or missions.

All participants are expected to create and follow *collaboratively* a story based on roleplaying.




Operating Principle
-------------------

The role of a game engine like Orge is mostly to *emulate* a Game Master, during a game session with real (human) player(s). 

The task includes:

 - describing to the players the world surrounding their characters
 - offering the players the means to interpret their character
 - resolving the corresponding actions 
 - evaluating the state and behaviour of all game elements besides the ones controlled by the players


The GM task can be described as an interactive simulator operating on game elements chosen, defined and arranged by the SW, based on the modelling provided by the SD. 

in a simulation.

server-side 

.. Note:: Although there are presumably only a few CRPG (actually we do not know  any of such games) that offer to a human player the technical possibility of being the GM instead of controlling a character, this possibility could be interesting and thus could be supported in the future by the Orge implementation.  


Video Games versus Table-top Games
----------------------------------


Orge is a game system primarily dedicated to video games. 

However it can be used as well for table-top games, since most computations remain simple thanks to the  abacuses (simplified precomputations to be used graphically).


Place of Random
---------------

We chose to introduce some randomness when resolving actions, as it creates a kind of "dramatic tension". It forces oneself to evaluate every possible outcome and to ponder them painfully.


Realistic versus Epic Setting
-----------------------------
Depending on the interactive story to be told, some actions might be totally impossible or, only, very unlikely (this is a difference of nature). The more unlikely outcomes may happen, the most epic the story will be.

With Orge we prefer to keep a certain level of "epicness", should such word exist. Thus extraordinary actions are always possible (notably thanks to the modifier system we use), even if they are very unlikely.


Tragedy versus Comedy
---------------------

We preferred letting the game system be, as much as possible, tone-agnostic: the choice of the mode of fiction is left to the scenario writer. See also: `Place of Death`_.
 

Medieval, Contemporary, Futuristic, etc.
----------------------------------------
The majority of popular MMORPGs are based on traditional fantasy themes, often occurring in an in-game universe comparable to that of Dungeons & Dragons

Some employ hybrid themes that either merge or substitute fantasy elements with those of science fiction, sword and sorcery, or crime fiction.


Place of Death
---------------
permadeath
Death And Afterlife




Client/Server, Model/View/Control and al
----------------------------------------


Recurrent Declaration about the Importance of Roleplay
--------------------------------------------------

Each and every game system, after dozens of pages involving equations and algorithms, ends up with the finding that all that matters is storytelling (on the GM side) and roleplaying (on the player side). There will be no exception here.

Citation




Creature Characterization
=========================

Not depending on a creature being a humanoid, a sentient being, a NPC, a monster or a PC, it will be described in an uniform way, by the means of *character's statistics*.

Traits
------

Races
-----

Classes
-------
direct characterization ny archetypal characters
 stereotyping 


Characteristics & Attributes
----------------------------
 
tree

 
Skills
------ 

Skill Tree

Progression System 
------------------

A powerful "moteur" for RPG players is to make their character progress. The usual scheme implies that the player is rewarded by a permanent increase in the abilities of his character after successful actions, which will be detailed below.

 ( etc.).


Usually a character starts a game unexperienced, young, at least partially untrained. Generally the lower the better, since it increases the character room for progression and, therefore, the game lifespan and the player pleasure (often more or less proportional to his character development).

Thus many CRPG are based on a training stage/tutorial involving a butchery of rats (in the best cases for the player).

For long-lived games, such as MMORPG, the character progression is ideally never-ending, otherwise players grow tired of the game.

Therefore a crucial point is to find the subtle balance between 



measure of character development
`experience points <http://en.wikipedia.org/wiki/Experience_points>`_ 


Levels considered harmful?
..........................

Speaking of realism, levels should be avoided due to the threshold effect they induce: instead of a continuous progress, levelling up results in a sharp rise of the potential of a character that corresponds to nothing in the real life.

We chose however to keep a level-based system for creatures. Why? One might call it nostalgia of old-school games, but the actual motivation of that choice is *player  pleasure*. We love when our character gets stronger, and if there is no particular event that underlines it, we feel frustated.

A bit like the epic nature of stories, with Orge the trade-off between realism and entertainment has been geared towards the former instead of the latter. 


Causes of Experience Gains
..........................

The convention here is that experience can only increase over time, except some very rare story events.

Following actions results in an experience increase:
 #. fulfilling quests
 #. slaughtering monsters
 #. finding rare objects
 #. solving an enigma, a challenge, a puzzle

The model we chose is that associated with each of these events, a experience bonus is defined (its value is to be chosen by the Scenario Writer). 


Consequences of Experience Gains
................................



Resolving Actions
=================

What for ?
----------

In a game, the success of all kinds of attempts of actions have to be evaluated, like when: 

 - attacking a foe
 - parrying an attack 
 - climbing a wall, etc.


The game system has to choose whether each action succeeds or fails.


How ?
-----

We based our system on the one Hirinkaël described `here < ../../../club/game/numericBook/ThresholdSimulationSystem.pdf>`_ (in French), most choices and remarks regarding the evaluation of actions are just taken from this document, adapted a bit and translated. 


It is done by evaluating the probability p of success (say, between 0 and 100%), and drawing a random value r in, ``[0,100]``. If ``r<p``, then the action succeeds, if ``r>p`` it fails, otherwise ``r=p`` and the outcome is neither a success nor a failure (for the sake of simplicity we will consider from now on that the action is then a success).

The core of the problem is to have the game system select an accurate, realistic value for p, the probability of success for that specific action.

The intended outcome of this part of the rule system is not a predetermined "one size fits all" probability of success: we want a probability that matches closely the set of specific and hardly predictable circumstances that exist at this moment of the game.

As the game creator cannot precompute all the possible contexts (as they are too numerous, due to the combinatorial explosion of possible in-world variations), the game system has to rely on rules that can determine on the fly, in the light of a specific context, a global probability of success.
 

We chose to rely on `Hirinkaël's system < ../../../club/game/numericBook/ThresholdSimulationSystem.pdf>`_ because:

 - it can easily take into account all kinds of situations
 - it is logical, consistent, symmetrical and fair
 - it is original
 - it is quite simple to understand and use
 - it can be used both in video games (thus being computer-based) and in table-top games (with dice and an abacus)
 
 
The reason we like this system lies mainly in the first advantage listed (flexibility, adaptability), as it offers us a way of adapting to an actual situation (real-life, complex, full of context) the chances of success of a given action which was modelled under normal circumstances (nominal conditions) only.


Two inputs: base probability & modifiers
---------------------------------------- 

Let's take, from the document we just mentioned, the example of a character wanting to attack another, and let's suppose we can evaluate the base probability, i.e. the probability of success of this action *under normal circumstances*.

The problem is that, in real cases, we are never in that stereotypical situation: the characters might be fighting in the dark, or one may be wounded, or there may be multiple opponents, etc. How to obtain, from the probability evaluated under normal circumstances, a custom probability reflecting all these numerous elements of context ?

What we really need actually is to be able to take into account probability modifiers that:

 - can be defined once for all, not depending on the base probability nor on any other modifier
 - can convert the base probability into the target context-dependent probability realistically (ex: it must be more difficult indeed to hit when wounded) and correctly (ex: probabilities must remain in the [0,1] range) 


One output: the actual context-dependent probability
---------------------------------------------------- 

To handle modifiers we retained the function suggested in the aforementionned paper:

.. image: probability-modifier-formula.png


This function, when given a base probability (p, abscissa) and a modifier (m, select the corresponding curve), determines the resulting modified probability (pm, ordinate):
 [probability-modifier.png] 


The wider curves, representing modifiers of -50%, 0% and 50%, allow to find easily the curves for intermediate modifiers, as they are paced every 10%.

One can see the symmetry of modifiers: the value of the increase in probability due to a given modifier p is equal to the value of the decrease in probability due to a modifier of -p.

The [probability-modifier.py script http://osdl.svn.sourceforge.net/viewvc/osdl/OSDL/trunk/src/doc/web/main/documentation/OSDL/OSDL-engine/probability-modifier.py?view=markup] gives some more indications:

::

  For a base probability of success of 0.0 %, with a modifier of:
    -30.0 %, modified probability is 0.0 %.
    -20.0 %, modified probability is 0.0 %.
    -10.0 %, modified probability is 0.0 %.
    0.0 %, modified probability is 0.0 %.
    10.0 %, modified probability is 0.0 %.
    20.0 %, modified probability is 0.0 %.
    30.0 %, modified probability is 0.0 %.

  For a base probability of success of 25.0 %, with a modifier of:
    -30.0 %, modified probability is 8.63 %.
    -20.0 %, modified probability is 12.5 %.
    -10.0 %, modified probability is 17.9 %.
    0.0 %, modified probability is 25.0 %.
    10.0 %, modified probability is 33.6 %.
    20.0 %, modified probability is 43.5 %.
    30.0 %, modified probability is 54.0 %.

  For a base probability of success of 50.0 %, with a modifier of:
    -30.0 %, modified probability is 22.0 %.
    -20.0 %, modified probability is 30.1 %.
    -10.0 %, modified probability is 39.6 %.
    0.0 %, modified probability is 50.0 %.
    10.0 %, modified probability is 60.3 %.
    20.0 %, modified probability is 69.8 %.
    30.0 %, modified probability is 77.9 %.

  For a base probability of success of 75.0 %, with a modifier of:
    -30.0 %, modified probability is 45.9 %.
    -20.0 %, modified probability is 56.4 %.
    -10.0 %, modified probability is 66.3 %.
    0.0 %, modified probability is 75.0 %.
    10.0 %, modified probability is 82.0 %.
    20.0 %, modified probability is 87.4 %.
    30.0 %, modified probability is 91.3 %.

  For a base probability of success of 100. %, with a modifier of:
    -30.0 %, modified probability is 100. %.
    -20.0 %, modified probability is 100. %.
    -10.0 %, modified probability is 100. %.
    0.0 %, modified probability is 100. %.
    10.0 %, modified probability is 100. %.
    20.0 %, modified probability is 100. %.
    30.0 %, modified probability is 100. %.

  For a base probability of success of 60.0 %, with first modifier being 20.0 %,
   with second modifier being 15.0 %, modified probability is 86.7 %.
  For a base probability of success of 60.0 %, with first modifier being 15.0 %,
   with second modifier being 20.0 %, modified probability is 86.7 %.
  For a base probability of success of 60.0 %, with a modifier of 35.0 %,
  modified probability is 86.7 %.


We can see that if the base probability is 100% or 0%, the action will be always respectively a success or a failure, not depending on the modifier. 

Moreover, multiple modifiers can be applied, applying m1 then m2 results in the same as applying m2 then m1, which is (in practice) the same as applying directly m3 = m1 + m2. 


How to choose the rule inputs ?
-------------------------------

If determining the base probability is part of the usual tasks of the seasoned Game Master, modifiers, as used here, are specific to this system.

The numerical rules have been calibrated so that, when base probability is at 50%, a modifier equal to m will result in a final probability equal to about ``50+m %``. 


So, to evaluate the value of a modifier modelling a specific element of context, the Game Master just has to imagine the case where:
 - we are under normal circumstances 
 - and the action is to be performed by a candidate who has 50% of chance of succeeding, 50% of chance of failing
 
 
Then, should the studied element of context occur, what would be the new probability of success (pnew) ? Once that value has been evaluated by the Game Master, the modifier is just equal to that value minus 50%: ``m = pnew - 50%``.

More precisely, the previous numerical recipe (``50+m %``) is an approximation, mostly accurate for small values of m. The real relation is: if the Game Master chooses, for the aforementioned probability of success, pnew, then ``m = 1 / (1+ exp(-pnew))``.

It leads to this abacus, helping to evaluate this modifier:
 [modifier-abacus.png] 

We can see that in most situations it is perfectly safe to stick to ``m = pnew - 50%``.


Let's illustrate with a simple example
--------------------------------------


Let's suppose that a given dark ninja has a nominal probability of climbing a given wall successfully equal to 80% (ninjas are somewhat gifted for that kind of exercise). Let's suppose as well that we are at night (note that it is a bonus for our dark ninja) but it is raining (malus) and sadly he has just been wounded by an alligator (big malus).  

As modifiers are made to be context-free, we need here to evaluate only the impact of a given modifier against the nominal situation, without having to bother with any other modifier (we suppose here all modifiers are independent, although one may argue it is an oversimplification here). 

Let's take the malus due to the wound. We may consider that a wounded dark-ninja at daylight, dry weather, incures a 35% penalty when trying to climb that kind of wall.

The same exercise may result in a 10% bonus for the climbing of an unwounded ninja, dry weather, but in the night, and in a 5% malus for the climbing of an unwounded ninja, during the day, but under the rain.

So, what is the overall probability of successful climbing, for our wounded ninja under the rain at night time ? Modifiers have to be summed, it results in a global modifier of ``-35+10-5=-30``. We then look at our curves: the intersection of the ninja base probability of success (80%) with the curve of the -30% modifier gives us a final probability of about 53%. 0ur ninja finally has little more chance to arrive at the top of the wall than to fall miserably in the flowers.



A Focus on Conflicts & Combats
==============================

One concern: to avoid that experienced characters based on fighting skills (barbarians, sword masters, ninjas, etc.) become significantly weaker, limited and dull to play than magic users, notably in late game.

Solutions: 
 - battle disciplines/maneuvers



Magic
=====

common: spell, mana

Wizzard/mage

cleric


License
=======
This document describes a game system. It includes a model and an implementation of it.


License for the game model (a.k.a. rules)
-----------------------------------------

The game model is released under a `Creative Commons <http://creativecommons.org>`_ license whose name is *Attribution-Noncommercial-Share Alike 3.0*, also known as *by-nc-sa*: 


.. image:: ../../../../images/by-nc-sa-3.0.png
   :target: http://creativecommons.org/licenses/by-nc-sa/3.0/
   :align: center

This license lets you remix, tweak, and build upon this work non-commercially, as long as you credit us and license your new creations under the identical terms.

You can thus download, redistribute, translate, make remixes and produce new systems and games based on this work, as long as you mention us and link back to us, but you cannot use this work commercially.

All new work based on yours will carry the same license, so any derivatives will also be non-commercial in nature.

Any of the above conditions can be waived if you get permission from the copyright holder.

See also a summary of `what can be done with the game model <http://creativecommons.org/licenses/by-nc-sa/3.0//>`_.


License for the game implementation
-----------------------------------

The Orge implementation is released under the `GNU General Public License <http://www.gnu.org/licenses/gpl.html>`_ (GPL), version 3 or any later version.



Bibliography
============


Other well-known game systems
-----------------------------

 - Castles & Crusades, `A Guide and Rules System for Fantasy Roleplaying <http://www.trolllord.com/newsite/downloads/pdfs/cnc_qs.pdf>`_ [PDF]
 - `Spiderweb Software <http://www.spiderwebsoftware.com/>`_, notably their latest games, `Avernum 5 <http://www.avernum.com/avernum5/index.html>`_ (see `these hints <http://www.ironycentral.com/cgi-bin/ubb/ubb/ultimatebb.php?ubb=get_topic;f=23;t=000013>`_ about its game system) and `Geneforge 4 <http://www.spiderwebsoftware.com/geneforge4/index.html>`_
 - GURPS
 - FUDGE
 - Starcomm
 - `Diablo II <http://www.battle.net/diablo2exp/>`_


References & Sources
--------------------

 - `Wikipedia article <http://en.wikipedia.org/wiki/Role-playing_game_system>`_ about role-playing game systems

 - `Hirinkaël document about modifier management <../../../club/game/numericBook/ThresholdSimulationSystem.pdf>`_ (in French)

 - An exciting series of articles of Matt Barton, `The History of Computer Role-Playing Games <http://www.gamasutra.com/features/20070223a/barton_01.shtml>`_
 
 
