
#include "moab/Core.hpp"
#include "moab/ScdInterface.hpp"
#include "moab/Interface.hpp"
//#include "moab/WriteVtk.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>  

#define PI 3.14159265

using namespace moab;
using namespace std;

void transf_point(double *in, double *out){
  out[0] = in[2] * cos(2 * PI * in[1]); // x
  out[1] = in[2] * sin(2 * PI * in[1]); // y
  out[2] = in[0]; // z
}

int main() 
{
  // I=z J=t K=r
  int I, J, K;
  I = 3; J = 9; K = 2;

  // 0. Instantiate MOAB and get the structured mesh interface
  Interface* mb = new (std::nothrow) Core;
  if (mb == NULL)
    return 1;
  ScdInterface *scdiface;
  ErrorCode rval = mb->query_interface(scdiface);MB_CHK_ERR(rval); // Get a ScdInterface object through moab instance

  // 1. Creates a IxJxK structured mesh, which includes I*J*K vertices and (I-1)*(J-1)*(K-1) hexes.
  ScdBox *box;
  vector<double> inCoords; //coord array with interleaved values xyzxyz...
  vector<double> cylCoords;

  // create a vector of coords ({rtz},{rtz},...)
  for (int k = 0; k < K; k++){
    for (int j = 0; j < J; j++){
      for (int i = 0; i < I; i++){
	cylCoords.push_back(i * 10);
	cylCoords.push_back(j * 0.125);
	cylCoords.push_back(k);
      }
    }
  }

  // for (int i = 0; i < cylCoords.size(); i++){
  //   std::cout << cylCoords[i] << " ";
  //   if ((i+1)%3==0)
  //     std::cout << std::endl;
  // }

  double in[3], out[3];
  for (int i = 0; i < 3*I*J*K; i+=3){
    in[0]=cylCoords[i];
    in[1]=cylCoords[i+1];
    in[2]=cylCoords[i+2];
    transf_point(in, out);
    inCoords.push_back(out[0]);
    inCoords.push_back(out[1]);
    inCoords.push_back(out[2]);
  }

  int lperiodic[3];
  lperiodic[0] = 0; lperiodic[1] = 0;  lperiodic[2] = 0;
  rval = scdiface->construct_box(HomCoord(0, 0, 0), HomCoord(I-1, J-1, K-1),
                                 &inCoords[0], 3*I*J*K, 
                                 box, lperiodic);MB_CHK_ERR(rval);

  // 2. Get the vertices and hexes from moab and check their numbers against I*J*K and (I-1)*(J-1)*(K-1), resp.
  Range verts, hexes;
  rval = mb->get_entities_by_dimension(0, 0, verts);MB_CHK_ERR(rval); // First '0' specifies "root set", or entire MOAB instance, second the entity dimension being requested
  rval = mb->get_entities_by_dimension(0, 3, hexes);MB_CHK_ERR(rval);

  std::cout << verts.size() << std::endl;
  std::cout << hexes.size() << std::endl;

  rval = mb->write_file("test_small_cyl.h5m");MB_CHK_ERR(rval);

  // std::vector<double> outCoords;
  // mb->get_vertex_coordinates(outCoords);

  /*

  // 3. Loop over elements in 3 nested loops over i, j, k; for each (i,j,k):
  vector<double> coords(8*3);

  vector<EntityHandle> connect;
  for (int k = 0; k < K - 1; k++) {
    for (int j = 0; j < J - 1; j++) {
      for (int i = 0; i < I - 1; i++) {
        // 3a. Get the element corresponding to (i,j,k)
        EntityHandle ehandle = box->get_element(i, j, k);
        if (0 == ehandle) return MB_FAILURE;
        // 3b. Get the connectivity of the element
        rval = mb->get_connectivity(&ehandle, 1, connect);MB_CHK_ERR(rval); // Get the connectivity, in canonical order
        // 3c. Get the coordinates of the vertices comprising that element
        rval = mb->get_coords(&connect[0], connect.size(), &coords[0]);MB_CHK_ERR(rval); // Get the coordinates of those vertices
      }
    }
  }
  */
  //EntityHandle set;
  //rval = mb->create_meshset(MESHSET_SET, set);MB_CHK_ERR(rval);

  // 4. Release the structured mesh interface and destroy the MOAB instance
  mb->release_interface(scdiface); // Tell MOAB we're done with the ScdInterface
  delete mb;

  return 0;
}
