#include <fstream>
#include <iostream>

#include "reader.hpp"
#include "typelist.hpp"

using MyTypeList = type_list::TypeList<int, float, char, short>;

const char kFilename[] = "input.txt";

void print_buffer(void *raw_buf, size_t len) {
  unsigned char *buf = static_cast<unsigned char*>(raw_buf);
  std::cout << std::hex;
  for (size_t i = 0; i < len; ++i) {
    std::cout << static_cast<int>(buf[i]) << " " << std::endl;
  }
  std::cout << std::endl << std::dec;
}

int main() {
  std::fstream file(kFilename, std::fstream::in);
  void *buf = type_list::TypeListReader<MyTypeList>::Read(file);
  void *buf2 = type_list::TypeListReader<MyTypeList>::Read(file);
  print_buffer(buf, type_list::TypeListSizeof<MyTypeList>());
  print_buffer(buf2, type_list::TypeListSizeof<MyTypeList>());
  free(buf);
  free(buf2);
}
