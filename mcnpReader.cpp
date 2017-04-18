#include "moab/Core.hpp"
#include "moab/ScdInterface.hpp"
#include "moab/Interface.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

//read file until "Tally bin boundaries:"
ErrorCode read_file_header(ifstream file){
  string line;
  // Get simulation date and time
  getline(file, line);
  // Get simulation title
  getline(file, line);
  // Get blank line
  getline(file, line);
  // Get number of histories
  getline(file, line);
  return MB_SUCCESS;
}

ErrorCode read_tally_header(ifstream file, particle &tally_particle){
  ErrorCode result;
  string line;
  // Get blank line
  getline(file, line);
  // Get tally number
  getline(file, line);
  // Get tally particle (optional)
  getline(file, line);
  result = get_tally_particle(line, tally_particle);
  if (MB_FAILURE == result){
    // This line is a tally comment
    // Get particle type
    getline(file, line);
    result = get_tally_particle(line, tally_particle);
  }
  return MB_SUCCESS;
}

ErrorCode get_tally_particle(string line, particle &tally_particle){
  if (std::string::npos != line.find("This is a neutron mesh tally.")){
    tally_particle = NEUTRON;
  }
  else if (std::string::npos != line.find("This is a photon mesh tally.")){
    tally_particle = PHOTON;
  }
  else if (std::string::npos != line.find("This is an electron mesh tally.")){
    tally_particle = ELECTRON;
  }
  else return MB_FAILURE;
  return MB_SUCCESS;
}

ErrorCode read_mesh_planes(ifstream file, vector<double> planes[3]){
  ErrorCode result;
  string line;
  // Get "Tally bin boundaries"
  getline(file, line);
  // Assume Cartesian
  // Get x planes
  getline(file, line);
  a = line.find("X direction:");
  if (std::string::npos != a){
    std::istringstream ss(line.substr(b + sizeof("X direction")));
    result = get_mesh_planes(ss, planes[0]);
  }
  // Get y planes
  getline(file,line);
  a = line.find("Y direction:");
  if (std::string::npos != a){
    std::istringstream ss(line.substr(b + sizeof("Y direction")));
    result = get_mesh_planes(ss, planes[1]);
  }
  // Get z planes
  getline(file,line);
  a = line.find("Z direction:");
  if (std::string::npos != a){
    std::istringstream ss(line.substr(b + sizeof("Z direction")));
    result = get_mesh_planes(ss, planes[2]);
  }
  return MB_SUCCESS;
}

ErrorCode get_mesh_planes(std::istringstream &ss,
			  std::vector<double> &plane){
  double val;
  while (!ss.eof()){
    ss >> val;
    plane.push_back(val);
  }
  return MB_SUCCESS;
}

ErrorCode read_element_values_and_errors(ifstream file,
					 double values[],
					 double errors[]){
  unsigned int index = 0;
  for (unsigned int i = 0; i < planes[0].size() - 1; i++){
    for (unsigned int j = 0; j < planes[1].size() - 1; j++){
      for (unsigned int k = 0; i < planes[2].size() - 1; k++){
	string line;
	getline(file,line);
	std::stringstream ss(line);
	double centroid[3];
	double energy;
	ss >> energy;
	ss >> centroid[0];
	ss >> centroid[1];
	ss >> centroid[2];
	ss >> values[index];
	ss >> errors[index];
	index++;
      }
    }
  }
  return MB_SUCCESS;
}

int main(){
  string filename;
  string line;
  ifstream file(filename);
  vector<double> coords, tallies, errors;
  //read file, assuming "col" format

  // Read the file header
  read_file_header(file);

  // Blank line
  getline(file, line);

  particle tally_particle;
  vector<double> planes[3];
  // Read tally header
  read_tally_header(file, tally_particle);

  // Blank line
  getline(file, line);

  // Read mesh planes
  read_mesh_planes(file, planes);

  // Get energy boundaries
  getline(file, line);
  
  // Blank line
  getline(file, line);

  // Column headers
  getline(file, line);

  // Read the values and errors
  unsigned int n_elements = (planes[0].size() - 1) * (planes[1].size() - 1) * (planes[2].size() - 1);
  read_element_values_and_errors(file, values, errors);
  
  int I = planes[0].size() - 1;
  int J = planes[1].size() - 1;
  int K = planes[2].size() - 1;
  Interface *mb = new (std::nothrow) Core;
  ScdInterface *scdiface;
  ErrorCode rval = mb->query_interface(scdiface);MB_CHK_ERR(rval);
  ScdBox *box;
  //assume Cartesian

  // Construct coords array with interleaved values xyzxyz...
  for (int i = 0; i < I; i++){
    for (int j = 0; j < J; j++){
      for (int k = 0; k < K; k++){
	coords.push_back(planes[0][i]);
	coords.push_back(planes[1][j]);
	coords.push_back(planes[2][k]);	
      }
    }
  }

  
  rval = scdiface->construct_box(HomCoord(0, 0, 0),
				 HomCoord(I-1, J-1, K-1),
				 &coords[0], 3*I*J*K,
				 box);MB_CHK_ERR(rval);

  Tag tally_tag, error_tag;
  result = MB->tag_get_handle("TALLY_TAG", 1, MB_TYPE_DOUBLE, tally_tag, MB_TAG_DENSE | MB_TAG_CREAT);
  result = MB->tag_get_handle("ERROR_TAG", 1, MB_TYPE_DOUBLE, error_tag, MB_TAG_DENSE | MB_TAG_CREAT);
  
  EntityHandle start_element = 0;
  Range element_range(start_element, start_element+n_elements-1);
  result = mb->tag_set_data(tally_tag, element_range, values);
  result = mb->tag_set_data(error_tag, element_range, errors);

  mb->release_interface(scdiface);
  return 0;
}
