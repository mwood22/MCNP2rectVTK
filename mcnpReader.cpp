#include "moab/ReadMCNP.cpp"
#include <vector>

int main(){
  ReadMCNP5* read;
  char* filename;
  std::vector<EntityHandle> input_meshet;
  //FileOpts==NULL unless averaging
  read->load_file(filename, &input_meshset[0], NULL);
  return 0;
}
