Megan Fox's PhysDemo

'1' - creates a block
'2' - creates a small sphere
'3' - creates a big sphere
'4' - creates a stack of blocks (in a neato flashy way)

'6' - drops a block from the camera's position
'7' - drops a sphere from the camera's position
'8' - creates a block stack at the position under the camera

'p' - creates an entire row (or two) of stacked blocks straight across the floor, using the camera's position as a reference.

'w' - creates a (very) large wall, using the camera's position as a reference.

'0' - creates an explosion, useful for clearing away a pile of objects.

'spacebar' - fires a super-massive bullet from the camera in the direction the camera is looking


Debug Controls:
Pg up/pg down - Adjust the global CFM, which makes objects softer as it goes up.
Home/end - Adjust the global ERP, which has some effect on the stability of the sim.

Insert/delete - Adjust the FPS Cutoff value.  What this does if, if something in the world would cause the FPS to drop below the cutoff, the application forces updates at the rate of the cutoff value.  This means that FPS never drops below the specified value, but the world appears to move in slow-motion.


Any objects falling significantly below the floor plate will be destroyed, so if the system starts lagging (and it will if you make a pile of too many blocks/etc), hit explosion a few times and send the objects hogging your system to their doom.


Have fun!
- Megan Fox
Elium Project


(reader's note: this is intended as a demo of proper Nebula 2 application-creation, and of proper usage of nOpende, enlight's thin Neb2 wrapper for ODE)