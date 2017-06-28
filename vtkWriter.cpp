
#include "moab/Core.hpp"
#include "moab/ScdInterface.hpp"
#include "moab/Interface.hpp"
#include "io/ReadMCNP5.hpp"
//#include "moab/WriteVtk.hpp"
#include <iostream>
#include <vector>
#include <fstream>

using namespace moab;
using namespace std;

void write_vtk_from_box(std::string name,
			ScdBox *box){

  ofstream file(name);

  // Get the I J K dimensions from the box
  int I, J, K;
  box->box_size(I,J,K);

  // Get the coordinates from the box
  std::vector<double> coords;
  box->impl()->get_vertex_coordinates(coords);

  // Get the tag data from the box
  Tag newTag;
  Range vertices;
  result = box->impl()->get_entities_by_dimension(0, 0, verts);
  result = box->impl()->tag_get_handle("tg",1,MB_TYPE_DOUBLE,newTag,
				     MB_TAG_CREAT | MB_TAG_DENSE);
  std::vector<double> tags(I*J*K,0);
  result = box->impl()->tag_get_data(newTag,vertices,&tags[0]);

  // Print header
  file << "# vtk DataFile Version 3.0" << endl;
  file << "rectilinear grid " << name << endl;
  file << "ASCII" << endl;
  file << "DATASET RECTILINEAR_GRID" << endl;

  // Print i j k
  file << "DIMENSIONS " << I << " " << J << " " << K << endl;

  // Print box coordinates
  file << "X_COORDINATES " << I << " float" << endl;
  for (int i = 0; i < I; i++)
    file << coords[i] << endl;
  file << "Y_COORDINATES " << J << " float" << endl;
  for (int j = I*J*K; j < (I*J*K)+(I*J); j += I)
    file << coords[j] << endl;
  file << "Z_COORDINATES " << K << " float" << endl;
  for (int k = 2*I*J*K; k < coords.size(); k += I*J)
    file << coords[k] << endl;

  // Print tag data
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


int main{
  Interface* mb = new (std::nothrow) Core;
  ReadMCNP5 reader(mb);
  char* filename;
  EntityHandle *input_meshet;
  FileOptions options;
  reader.load_file(filename,
		   input_meshet,
		   options);
  write_vtk_from_box("vtkTest1");
  return 0;
}
