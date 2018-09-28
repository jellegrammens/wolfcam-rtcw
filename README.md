# wolfcam-rtcw

This RTCW mod is a modification of the wolfcam 0.0.5 RTCW mod from Brugal. I've made these changes for myself for my upcoming RTCW history movie. 

# Context
There are a lot of different moviemods or viewing mods for RTCW such as: wolfcam, uvMovieMod, Demoviewer and Viewpoint are the most known ones. However, all these mods have their strenghts, weaknesses and own features. I only want to use 1 mod for all RTCW versions with all features 'One RTCW moviemod to rule them all'.

# Changes Made:
- Obituaries ("Player1 killed Player2 with MP40") support for older RTCW versions 1.0 and 1.33 (protocol 57, 58 and 59):
- Added uvMovieMod and Demoviewer features
- Added some own made features

# Commands implemented:

### Wolfcam 0.0.5:
- /players - to get a list of players and client ids
- /follow [clientid] - follow selected client. /follow -1 follows original demo POV
- /wolfcam_avoidDemoPov [0/1] - avoids trying to follow demo POV
- /wolfcam_tryToStickWithTeam [0/1] - tries to stick with team of demo POV
- /wolfcam_wh [0/1] - activates a wallhack, but isn't perfectly implemented.

### UvMovieMod:
- /mm_demoCrosshair [0/1] - draw a crosshair while in cg_draw2d 0
- /mm_demoPopUp [0/1] - draws popups when in cg_draw2d 0
- /mm_drawYourOwnFragsOnly [0/1] - draws only popups for player you are following
- /mm_drawTeamkills [0/1] - draws teamkill popups 
- /mm_vapYourselfe [0/1] - draws vaporized, grenaded himself popups 
- /mm_bloodflash [0/1] - draws blood or not
- /mm_shadowedPopups [0/1] - adds a shadow behind popups
- /mm_freecamConsole [0/1] -
- /freecam - toggle, enables/disables freecam

### DemoViewer 4.1:
- /demo_numPopups [int] - number of maximum popups (obituaries) drawn
- /demo_popupHeight [int] - vertical size of popups
- /demo_popupWidth [int] -  horizontal size of popups

### Commands i have added:
- mm_follow [clientnum]: Special command to force showing obituaries from a clientnum when in /freecam.
- mm_wtvchat [0/1]: Draw WolfTV chat or not.


# How to launch this mod:
Launch RTCW with: wolfmp.exe +set fs_game wolfcam-rtcw

# Upcoming features to implement:
- Add OSP colour support (turn on/off feature)
- Add a command that can remove centerprints from game events such as "Sea Wall Breached!"
- Add viewpoint features (follow grenade, panzerfaust, etc...)

# Thanks to:
- brugal: for releasing his source code
- crumbs: for his moviemod source code and (mental) support he gave me for coding this
- hannes: for creating uvMovieMod
