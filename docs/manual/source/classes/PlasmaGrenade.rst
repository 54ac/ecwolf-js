.. index::
	single: Classes; Actor; PlasmaGrenade

.. _Classes:PlasmaGrenade:

Classes:PlasmaGrenade
=============

.. note:: Wait! Stop! Before you copy this actor's definition into your mod, remember the following things:

	1. You do **not** need to copy that actor, since it is already defined.
	2. In fact, it's not just useless, it's actually harmful as it can cause problems.
	3. If you want to modify it, or use a modified version, using inheritance is the way to go.
	4. The actor definitions here are put on the docs **for reference purpose only**. Learn from them, don't copy them.
	5. There is only one exception: if what you want is changing :ref:`Ammo <Classes:Ammo>` capacity, you need to create a new type from Ammo.

Overview
--------

TODO

DECORATE definition
-------------------

.. code:: DECORATE

	actor PlasmaGrenade : SmartAnimation
	{
	  seesound "plasmagrenade/fire"
	  deathsound "plasmagrenade/hit"
	  radius 10.5
	  speed 14
	  damage (random(40, 167))
	  PROJECTILE
	
	  action native A_PlasmaGrenadeCalcDuration();
	
	  states
	  {
	    Spawn:
	      GREN C 0 bright A_ScaleVelocity(frandom(1.0, 1.5))
	      GREN CCBABBCD 3.5 bright A_PlasmaGrenadeCalcDuration
	      GREN E 0 bright A_Scream
	      GREN E 0 bright A_Explode(random(50, 177), 128, 0)
	      GREN E 0 bright A_Stop
	      GREN E random(3, 6) bright A_InitSmartAnim(random(3, 6))
	      GREN FGH 3 bright A_SmartAnimDelay
	      goto Spawn
	    Death:
	      EXPL A 0 bright A_Explode(random(50, 177), 128, 0)
	      EXPL A random(1.5, 5) bright A_InitSmartAnim(random(3, 10))
	      EXPL BCDE 1.5 bright A_SmartAnimDelay
	      stop
	  }
	}
