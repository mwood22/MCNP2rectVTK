#include moab/build/moab.make

CC = g++
MOAB_INCLUDES = -I/home/matt/opt/moab/include
MOAB_LIBS_LINK = -L/home/matt/opt/moab/lib 


vtkWriter: vtkWriter.cpp
	$(CC) $(MOAB_INCLUDES) $(MOAB_LIBS_LINK)  vtkWriter.cpp -lMOAB -o vtkWriter

clean:
	rm vtkWriter
