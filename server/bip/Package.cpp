
#include <iostream>

void my_print(const char *message, int &adata){
  std::cout << "Someone says: " << message << " with data=" << adata << std::endl;
}

void my_modify(int &adata){
  adata = 999;
}


bool const_my_guard(int &adata){
  return adata > 0;
}
