#include <fstream>
#include <iostream>
#include <sstream>

#include "compression_reader.hpp"
#include "reader.hpp"
#include "typelist.hpp"

using MyTypeList = type_list::TypeList<int, float, char, short>;

const char kSimpleFilename[] = "simple.txt";
const char kCompressorFilename[] = "compressed.txt";

void print_buffer(void *raw_buf, size_t len) {
  unsigned char *buf = static_cast<unsigned char *>(raw_buf);
  std::cout << std::hex;
  for (size_t i = 0; i < len; ++i) {
    std::cout << static_cast<int>(buf[i]) << " " << std::endl;
  }
  std::cout << std::endl << std::dec;
}

void test_simple_reader() {
  std::cout << "### STARTING SIMPLE READER TEST ###" << std::endl;
  std::fstream file(kSimpleFilename, std::fstream::in);
  void *buf = type_list::TypeListReader<MyTypeList>::Read(file);
  void *buf2 = type_list::TypeListReader<MyTypeList>::Read(file);
  print_buffer(buf, type_list::TypeListSizeof<MyTypeList>());
  print_buffer(buf2, type_list::TypeListSizeof<MyTypeList>());
  free(buf);
  free(buf2);
  std::cout << "### STOPPING SIMPLE READER TEST ###" << std::endl;
}

class CompressedString {
public:
  std::string decompress() { return "*" + str_ + "*"; }
  friend std::istream &operator>>(std::istream &in, CompressedString &rhs) {
    return in >> rhs.str_;
  }

private:
  std::string str_;
};

template <class T> class UniversalDecompressor {
public:
  static T decompress(const std::string &str) {
    std::stringstream stream(str);
    T val;
    stream >> val;
    return val;
  }
};

using MyCompressedTypeList = type_list::TypeList<std::string, int>;
using MyCompressorTypeList =
    type_list::TypeList<CompressedString, type_list::EmptyType>;
using MyDecompressorTypeList =
    type_list::TypeList<type_list::NoDecompressor, UniversalDecompressor<int>>;

void test_compression_reader() {
  std::cout << "### STARTING COMPRESSION READER TEST ###" << std::endl;
  std::fstream file(kCompressorFilename, std::fstream::in);
  void *buf =
      type_list::TypeListCompressionReader<MyCompressedTypeList,
                                           MyCompressorTypeList,
                                           MyDecompressorTypeList>::Read(file);
  print_buffer(buf, type_list::TypeListSizeof<MyCompressedTypeList>());
  free(buf);
  std::cout << "### STOPPING COMPRESSION READER TEST ###" << std::endl;
}

int main() {
  test_compression_reader();
}
