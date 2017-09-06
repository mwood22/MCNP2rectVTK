from math import pi, cos, sin

R = 3
T = 30
Z = 40

print "# vtk DataFile Version 2.0\nReally cool data\nASCII\nDATASET STRUCTURED_GRID"
print "DIMENSIONS ",str(R+1),str(T+1),str(Z+1)
print "POINTS ",str((R+1)*(T+1)*(Z+1)), " float"
      
for z in range(Z+1):
    for t in range(T+1):
        for r in range(R+1):
            print str(r*cos(2*(t%T)*pi/T)),str(r*sin(2*(t%T)*pi/T)),str(z)    

print "CELL_DATA ", str(R*T*Z), "\nSCALARS foo float 1\nLOOKUP_TABLE default"
data = 1
for r in range (R):
    for t in range(T):
        for z in range(Z):
            print (str(data))
            data = data + 1
            
