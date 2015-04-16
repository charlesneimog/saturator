New: build for winxp linked to pd-exteded 0.40.3
This build was made using MinGW

I have provided builds for some DARWIN platforms. Check out the build directory.
If this doesn't work for you, you'll need to make this.

Edit makefile and add set the DESTINATION folder.
If you're on linux run:
make pd_linux

If you're on Mac OS X you need to install X Code which should come on the optional disc.
As with linux, on a terminal in saturator~ folder run:
make pd_darwin

On Windows:
You must have MinGW or hack the makefile.
The makefile also needs to the location of pd.dll or pd.lib file

You must load a saturator object to load saturator~ and saturator_vu~ classes.
You can automatically do so by loading saturator at startup:
pd -lib saturator

(C) 2008, Paulo Casaes irmaosaturno@gmail.com
GPL v2
