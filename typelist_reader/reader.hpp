#pragma once

#include <istream>

#include "typelist.hpp"

namespace type_list {
template <class TL> constexpr size_t TypeListSizeof() {
  return sizeof(typename TL::CurrentType) + TypeListSizeof<typename TL::Tail>();
}

template <> constexpr size_t TypeListSizeof<EmptyTypeList>() { return 0; }

template <class TL> class TypeListReader {
public:
  static void *Read(std::istream &in) {
    char *buffer = AllocateBuffer();
    ReadToBuffer(in, buffer);
    return buffer;
  }
  static void ReadToBuffer(std::istream &in, char *out);

private:
  static char *AllocateBuffer() { return static_cast<char*>(std::malloc(TypeListSizeof<TL>())); }
};

template <class TL>
void TypeListReader<TL>::ReadToBuffer(std::istream &in, char *out) {
  typename TL::CurrentType *var = reinterpret_cast<typename TL::CurrentType*>(out);
  in >> (*var);
  TypeListReader<typename TL::Tail>::ReadToBuffer(in, out + sizeof(*var));
}

template <>
void TypeListReader<EmptyTypeList>::ReadToBuffer(std::istream &in, char *out) {}
} // namespace type_list
