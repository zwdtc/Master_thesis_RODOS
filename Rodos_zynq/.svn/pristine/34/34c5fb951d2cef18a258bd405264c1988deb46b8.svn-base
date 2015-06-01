some tips for generate the ROdos BSP for XIlinx SDK

1.delete the bcm2835.h

2.modified the one option of inside the .mld file  
OPTION APP_LINKER_FLAGS = "-Wl,--start-group,-lgcc,-lc,--end-group"; not sure should be -lgcc or -lg++.

3.one of the source file in the package qspips changed due to the uncompatible with the c++.

4.all the gcc should keep the same, not change to g++, in the Tcl file.