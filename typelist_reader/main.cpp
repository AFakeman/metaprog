#include <iostream>

#include "reader.hpp"
#include "typelist.hpp"

using MyTypeList = type_list::TypeList<int, float, char, short>;

int main() {
  std::cout << sizeof(int) + sizeof(float) + sizeof(char) + sizeof(short)
            << std::endl;
  std::cout << type_list::TypeListSizeof<MyTypeList>() << std::endl;
}
