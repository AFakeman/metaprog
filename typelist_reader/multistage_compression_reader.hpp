#pragma once

#include <istream>
#include <list>

#include "typelist.hpp"

namespace type_list {

typedef void (*Decompressor)(void *, char *);

struct EmptyDecompressorList {
public:
  constexpr static Decompressor Head = nullptr;
  using Tail = EmptyType;
};

template <Decompressor F, Decompressor... Rest> struct DecompressorList {
  constexpr static Decompressor Head = F;
  using Tail = DecompressorList<Rest...>;
};

template <Decompressor F> struct DecompressorList<F> {
  constexpr static Decompressor Head = F;
  using Tail = EmptyDecompressorList;
};

template <class Type, class CompressedType, Decompressor F>
class TypeListMultistageCompressionReaderHelper {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    CompressedType comp;
    in >> comp;
    auto decomp = comp.decompress();
    F(&decomp, out);
  }
};

template <class Type, class CompressedType>
class TypeListMultistageCompressionReaderHelper<Type, CompressedType,
                                                nullptr> {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    CompressedType comp;
    in >> comp;
    Type *ptr = reinterpret_cast<Type *>(out);
    (*ptr) = comp.decompress();
  }
};

template <class Type, Decompressor F>
class TypeListMultistageCompressionReaderHelper<Type, EmptyType, F> {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    std::string value;
    in >> value;
    F(&value, out);
  }
};

template <class Type>
class TypeListMultistageCompressionReaderHelper<Type, EmptyType, nullptr> {
public:
  static void ReadToBuffer(std::istream &in, char *out) {
    Type *ptr = reinterpret_cast<Type *>(out);
    in >> (*ptr);
  }
};

template <class TL, class CompressedTL, class Decompressors>
class TypeListMultistageCompressionReader {
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

template <class TL, class CompressedTL, class Decompressors>
void TypeListMultistageCompressionReader<
    TL, CompressedTL, Decompressors>::ReadToBuffer(std::istream &in,
                                                   char *out) {
  TypeListMultistageCompressionReaderHelper<
      typename TL::CurrentType, typename CompressedTL::CurrentType,
      Decompressors::Head>::ReadToBuffer(in, out);
  TypeListMultistageCompressionReader<typename TL::Tail,
                                      typename CompressedTL::Tail,
                                      typename Decompressors::Tail>::
      ReadToBuffer(in, out + sizeof(typename TL::CurrentType));
}

template <>
void TypeListMultistageCompressionReader<
    EmptyTypeList, EmptyTypeList, EmptyDecompressorList>::ReadToBuffer(std::istream &in,
                                                               char *out) {}
} // namespace type_list
