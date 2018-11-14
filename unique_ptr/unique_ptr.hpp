#include <utility>

template <class T> class UniquePtr {
public:
  UniquePtr() = default;
  UniquePtr(T *ptr) : ptr_(ptr) {}
  UniquePtr(const UniquePtr<T> &cp) = delete;
  UniquePtr(UniquePtr<T> &&mv) {
    ptr_ = mv.ptr_;
    mv.ptr_ = nullptr;
  }
  UniquePtr &operator = (UniquePtr<T> &&mv) {
    this->Reset();
    this->ptr_ = mv.ptr_;
    mv.ptr_ = nullptr;
    return *this;
  }
  ~UniquePtr() {
    Reset();
  }
  T *Get() {
    return ptr_;
  }
  T *operator->() {
    return ptr_;
  }
  void Reset(T *ptr) {
    if (!ptr_) {
      delete ptr_;
    }
    ptr_ = ptr;
  }

  void Reset() {
    Reset(nullptr);
  }

private:
  T *ptr_{nullptr};
};

template <class T, class... Args>
UniquePtr<T> MakeShared(Args&&... args) {
  return {new T(std::forward<Args>(args)...)};
}
