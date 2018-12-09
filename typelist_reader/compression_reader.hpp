#pragma once

#include <istream>

#include "typelist.hpp"

namespace type_list {

struct NoDecompressor {};

template <class Type, class CompressedType, class DecompressorType>
class TypeListCompressionReaderHelper {
public:
  static void ReadToBuffer(std::istream &in, char *out);
};

template <class Type, class DecompressorType>
class TypeListCompressionReaderHelper<Type, EmptyType, DecompressorType> {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    std::string value;
    in >> value;
    Type *ptr = reinterpret_cast<Type *>(out);
    (*ptr) = DecompressorType::decompress(value);
  }
};

template <class Type, class CompressedType>
class TypeListCompressionReaderHelper<Type, CompressedType, NoDecompressor> {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    CompressedType comp;
    in >> comp;
    Type *ptr = reinterpret_cast<Type *>(out);
    (*ptr) = comp.decompress();
  }
};

template <class TL, class CompressedTL, class DecompressorTL>
class TypeListCompressionReader {
public:
  static void *Read(std::istream &in) {
    char *buffer = AllocateBuffer();
    ReadToBuffer(in, buffer);
    return buffer;
  }
  static void ReadToBuffer(std::istream &in, char *out);

private:
  static char *AllocateBuffer() {
    return static_cast<char *>(std::malloc(TypeListSizeof<TL>()));
  }
};

template <class TL, class CompressedTL, class DecompressorTL>
void TypeListCompressionReader<TL, CompressedTL, DecompressorTL>::ReadToBuffer(
    std::istream &in, char *out) {
  TypeListCompressionReaderHelper<typename TL::CurrentType,
                       typename CompressedTL::CurrentType,
                       typename DecompressorTL::CurrentType>::ReadToBuffer(in,
                                                                           out);
  TypeListCompressionReader<typename TL::Tail, typename CompressedTL::Tail,
                 typename DecompressorTL::Tail>::
      ReadToBuffer(in, out + sizeof(typename TL::CurrentType));
}

template <>
void TypeListCompressionReader<EmptyTypeList, EmptyTypeList, EmptyTypeList>::ReadToBuffer(
    std::istream &in, char *out) {}
} // namespace type_list
