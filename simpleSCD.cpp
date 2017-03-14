
#include "moab/Core.hpp"
#include "moab/ScdInterface.hpp"
#include "moab/Interface.hpp"
//#include "moab/WriteVtk.hpp"
#include <iostream>
#include <vector>
#include <fstream>

using namespace moab;
using namespace std;

void file_writer_custom(int I, int J, int K,
			vector<double> coords, vector<double> tags){
  int size = coords.size();
  ofstream file("simpleSCD.vtk");
  file << "# vtk DataFile Version 3.0" << endl;
  file << "rectilinear grid simpleSCD" << endl;
  file << "ASCII" << endl;
  file << "DATASET RECTILINEAR_GRID" << endl;
  file << "DIMENSIONS " << I << " " << J << " " << K << endl;
  file << "X_COORDINATES " << I << " float" << endl;
  for (int i = 0; i < I; i++)
    file << coords[i] << endl;
  file << "Y_COORDINATES " << J << " float" << endl;
  for (int j = I*J*K; j < (I*J*K)+(I*J); j += I)
    file << coords[j] << endl;
  file << "Z_COORDINATES " << K << " float" << endl;
  for (int k = 2*I*J*K; k < coords.size(); k += I*J)
    file << coords[k] << endl;
  file << "POINT DATA " << I*J*K << endl;
  file << "LOOKUP_TABLE default" << endl;
  file << "SCALARS scalars float" << endl;
  for(int i = 0; i < tags.size(); i++){
    file << tags[i] << " ";
    if ((i+1)%6==0)
      file << endl;
  }
  file.close();

}


int main() 
{
  int I, J, K;
  I = J = K = 5;

  // 0. Instantiate MOAB and get the structured mesh interface
  Interface* mb = new (std::nothrow) Core;
  if (mb == NULL)
    return 1;
  ScdInterface *scdiface;
  ErrorCode rval = mb->query_interface(scdiface);MB_CHK_ERR(rval); // Get a ScdInterface object through moab instance

  // 1. Creates a IxJxK structured mesh, which includes I*J*K vertices and (I-1)*(J-1)*(K-1) hexes.
  ScdBox *box;
  vector<double> inCoords; //coord array with interleaved values xyzxyz...
  for (int k = 0; k < K; k++) {
    for (int j = 0; j < J; j++) {
      for (int i = 0; i < I; i++) {
	inCoords.push_back((i+1)*0.1);
	inCoords.push_back((j+1)*1.1);
	inCoords.push_back((k+1)*2.1);
      }
    }
  }
  
  rval = scdiface->construct_box(HomCoord(0, 0, 0), HomCoord(I-1, J-1, K-1), // low, high box corners in parametric space
                                 &inCoords[0], 3*I*J*K, // specify coords
                                 box);MB_CHK_ERR(rval); // box is the structured box object providing the parametric
                                                     // structured mesh interface for this rectangle of elements

  // 2. Get the vertices and hexes from moab and check their numbers against I*J*K and (I-1)*(J-1)*(K-1), resp.
  Range verts, hexes;
  rval = mb->get_entities_by_dimension(0, 0, verts);MB_CHK_ERR(rval); // First '0' specifies "root set", or entire MOAB instance, second the entity dimension being requested
  rval = mb->get_entities_by_dimension(0, 3, hexes);MB_CHK_ERR(rval);

  std::vector<double> outCoords;
  mb->get_vertex_coordinates(outCoords);

  //write tags and read them back
  std::vector<double> inTagData;
  for (int i = 0; i < I; i++){
    for (int j = 0; j < J; j++){
      for (int k = 0; k < K; k++){
	inTagData.push_back((i*j*k)*0.1);
      }
    }
  }
  Tag newTag;
  rval = mb->tag_get_handle("tg",1,MB_TYPE_DOUBLE,newTag,
			    MB_TAG_CREAT | MB_TAG_DENSE);
  mb->tag_set_data(newTag,verts,&inTagData[0]);
  std::vector<double> outTagData(I*J*K,0);
  mb->tag_get_data(newTag,verts,&outTagData[0]);

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
  EntityHandle set;
  rval = mb->create_meshset(MESHSET_SET, set);MB_CHK_ERR(rval);
  //rval = mb->write_mesh("simpleSCD.vtk");MB_CHK_ERR(rval);

  file_writer_custom(I,J,K,outCoords,outTagData);
  // 4. Release the structured mesh interface and destroy the MOAB instance
  mb->release_interface(scdiface); // Tell MOAB we're done with the ScdInterface
  delete mb;

  return 0;
}
