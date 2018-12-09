#include <fstream>
#include <iostream>
#include <sstream>

#include "compression_reader.hpp"
#include "multistage_compression_reader.hpp"
#include "reader.hpp"
#include "typelist.hpp"

using MyTypeList = type_list::TypeList<int, float, char, short>;

const char kSimpleFilename[] = "simple.txt";
const char kCompressorFilename[] = "compressed.txt";
const char kMultistageCompressorFilename[] = "multistage_compressed.txt";

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
  CompressedString() = default;
  CompressedString(const std::string &str) : str_(str) {};
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

template <class T>
void UniversalDecompressorFunction(void *raw_buf, char *out) {
  std::string *str = static_cast<std::string*>(raw_buf);
  T *ptr = reinterpret_cast<T *>(out);
  *ptr = UniversalDecompressor<T>::decompress(*str);
}

void CompressedStringFunction(void *raw_buf, char *out) {
  std::string *str = static_cast<std::string*>(raw_buf);
  std::string *ptr = reinterpret_cast<std::string*>(out);
  *ptr = CompressedString(*str).decompress();
}

using MyMultistageCompressedTypeList = type_list::TypeList<int, int, std::string, std::string>;
using MyMultistageCompressorTypeList =
    type_list::TypeList<type_list::EmptyType, type_list::EmptyType, CompressedString, CompressedString>;
using MyMultistageDecompressorList =
    type_list::DecompressorList<nullptr, UniversalDecompressorFunction<int>, nullptr, CompressedStringFunction>;

void test_multicompression_reader() {
  std::cout << "### STARTING MULTICOMPRESSION READER TEST ###" << std::endl;
  std::fstream file(kMultistageCompressorFilename, std::fstream::in);
  void *buf =
      type_list::TypeListMultistageCompressionReader<MyMultistageCompressedTypeList,
                                           MyMultistageCompressorTypeList,
                                           MyMultistageDecompressorList>::Read(file);
  print_buffer(buf, type_list::TypeListSizeof<MyMultistageCompressedTypeList>());
  free(buf);
  std::cout << "### STOPPING MULTICOMPRESSION READER TEST ###" << std::endl;
}

int main() {
  test_multicompression_reader();
}
