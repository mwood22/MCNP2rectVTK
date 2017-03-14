#include moab/build/moab.make

CC = g++
MOAB_INCLUDES = -I/home/matthew/opt/moab/include
MOAB_LIBS_LINK = -L/home/matthew/opt/moab/lib 

#simpleSCD:
#	$(CC) $(MOAB_INCLUDES) $(MOAB_LIBS_LINK) simpleSCD.cpp -o simpleSCD

simpleSCD: simpleSCD.cpp
	$(CC) $(MOAB_INCLUDES) $(MOAB_LIBS_LINK)  simpleSCD.cpp -lMOAB -o simpleSCD

clean:
	rm simpleSCD
