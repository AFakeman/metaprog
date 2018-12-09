#pragma once

namespace type_list {
struct EmptyType {};

struct EmptyTypeList {
  typedef EmptyType CurrentType;
  static constexpr int Length() { return 0; }
};

template <class T, class... Rest> struct TypeList {
  typedef T CurrentType;
  typedef TypeList<Rest...> Tail;
  static constexpr int Length() { return Tail::Length() + 1; }
};

template <class T> struct TypeList<T> {
  typedef T CurrentType;
  typedef EmptyTypeList Tail;
  static constexpr int Length() { return Tail::Length() + 1; }
};

template <class T, class... Rest> struct Merge {};

template <class T, class... Rest> struct Merge<T, TypeList<Rest...>> {
  typedef TypeList<T, Rest...> Result;
};

template <class T> struct Merge<T, EmptyTypeList> {
  typedef TypeList<T> Result;
};

template <class TL, int pos> struct Get {
  static_assert(TL::Length() > pos && pos >= 0,
                "java.lang.IndexOutOfBoundsException");
  typedef typename Get<typename TL::Tail, pos - 1>::Result Result;
};

template <class TL> struct Get<TL, 0> {
  typedef typename TL::CurrentType Result;
};

template <class T, class TL> struct Add { typedef TypeList<T, TL> Result; };

template <class TL, int pos> struct Remove {
  static_assert(TL::Length() > pos && pos >= 0,
                "java.lang.IndexOutOfBoundsException");
  typedef Merge<typename TL::CurrentType,
                typename Remove<typename TL::Tail, pos - 1>::Result>
      Result;
};

template <class TL> struct Remove<TL, 0> { typedef typename TL::Tail Result; };

template <class TL, int pos, class T> struct Insert {
  static_assert(TL::Length() >= pos && pos >= 0,
                "java.lang.IndexOutOfBoundsException");
  typedef typename Merge<
      typename TL::CurrentType,
      typename Insert<typename TL::Tail, pos - 1, T>::Result>::Result Result;
};

template <class TL, class T> struct Insert<TL, 0, T> {
  typedef typename Merge<T, TL>::Result Result;
};

template <class T> struct Insert<EmptyTypeList, 0, T> {
  typedef TypeList<T> Result;
};

template <class TL, int pos, class T> struct Replace {
  static_assert(TL::Length() > pos, "java.lang.IndexOutOfBoundsException");
  typedef typename Merge<
      typename TL::CurrentType,
      typename Replace<typename TL::Tail, pos - 1, T>::Result>::Result Result;
};

template <class TL, class T> struct Replace<TL, 0, T> {
  typedef typename Merge<T, typename TL::Tail>::Result Result;
};
}; // namespace type_list
