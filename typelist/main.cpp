#include <iostream>

#include "typelist.hpp"

typedef type_list::TypeList<unsigned *, char *, int *, double *> some_list;
typedef type_list::Remove<some_list, 0>::Result list_remove;
typedef type_list::Insert<some_list, 4, float *>::Result list_insert;
typedef type_list::Replace<some_list, 1, float *>::Result list_replace;

static_assert(some_list::Length() == 4);
static_assert(list_remove::Length() == 3);
static_assert(list_replace::Length() == 4);

int main() {
  type_list::Get<list_replace, 1>::Result a = (float *)nullptr;
  type_list::Get<list_remove, 0>::Result b = (char *)nullptr;
  type_list::Get<list_insert, 4>::Result c = (float *)nullptr;
}
