#include <cassert>

#include "unique_ptr.hpp"

struct MyCoolStruct {
  MyCoolStruct(int x, int y) : x(x), y(y) {}
  int x;
  int y;
};

void testMoveConstruct() {
  UniquePtr<MyCoolStruct> smart_ptr_old = MakeShared<MyCoolStruct>(1, 2);
  UniquePtr<MyCoolStruct> smart_ptr = std::move(smart_ptr_old);
  assert(smart_ptr_old.Get() == nullptr);
  assert(smart_ptr->x == 1);
  assert(smart_ptr->y == 2);
}

void testMoveAssign() {
  UniquePtr<MyCoolStruct> smart_ptr_old = MakeShared<MyCoolStruct>(1, 2);
  UniquePtr<MyCoolStruct> smart_ptr = MakeShared<MyCoolStruct>(3, 4);
  smart_ptr = std::move(smart_ptr_old);
  assert(smart_ptr_old.Get() == nullptr);
  assert(smart_ptr->x == 1);
  assert(smart_ptr->y == 2);
}

void testReset() {
  UniquePtr<MyCoolStruct> smart_ptr = MakeShared<MyCoolStruct>(1, 2);
  assert(smart_ptr->x == 1);
  assert(smart_ptr->y == 2);
  smart_ptr.Reset(new MyCoolStruct{4, 5});
  assert(smart_ptr->x == 4);
  assert(smart_ptr->y == 5);
}

int main() {
  testReset();
  testMoveConstruct();
  testMoveAssign();
}
