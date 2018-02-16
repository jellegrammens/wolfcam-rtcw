Return To Castle Wolfenstein
Mod source release
Linux Notes
=============================

A few notes about the RTCW mod source release on Linux:

- The build system is using cons:
http://www.dsmit.com/cons/
cons is a perl-based replacement for GNU make
you need to have recent version perl installed
cons 2.3.1 minimum is required, a working cons script is provided in unix/ directory

- Build options:
you can select between release and debug builds, as well as various other options
do ./cons -h to get a list of cons options
Ex: for a release build, ./cons -- release

- how to develop on a mod easily?
I would suggest doing the following:
build once to have a debug-x86-Linux-2.2 tree,
symlink debug-x86-Linux-2.2/out/main to ~/.wolf/yourmod

- how does mp_bin.pk3 work?
this file is used to enforce sv_pure checksum
client and server need to have the same version of this file
the client will use it to make sure it's loading the right cgame and ui .so

<ttimo@idsoftware.com>
