#include <iostream>
#include <algorithm>
#include <memory>
#include <array>
#include <set>

enum class Type {
  Proxy,
  Mediator,
  Observer,
};

class InterfaceB {
public:
  virtual void foo() = 0;
  virtual void baz() = 0;
};

class InterfaceBObserver {
public:
    virtual void notified() = 0;
};

class InterfaceBObservable {
public:
    virtual void subscribe(InterfaceBObserver *) = 0;
    virtual void unsubscribe(InterfaceBObserver *) = 0;
};

class A : public InterfaceBObserver {
public:
  A(const std::shared_ptr<InterfaceB> &ptr) : obj_(ptr) {}
  A(const std::shared_ptr<InterfaceB> &ptr,
    const std::weak_ptr<InterfaceBObservable> &observable)
      : obj_(ptr), observable_(observable) {
     if (auto ptr = observable_.lock()) {
         ptr->subscribe(this);
     }
  }
  ~A() {
      if (auto ptr = observable_.lock()) {
          ptr->unsubscribe(this);
      }
  }
  void DoJob() {
      obj_->foo();
      obj_->baz();
  }
  virtual void notified() { std::cout << "notified" << std::endl; }
private:
  std::shared_ptr<InterfaceB> obj_;
  std::weak_ptr<InterfaceBObservable> observable_;
};

class B : public InterfaceB {
public:
  B(const std::string &name) : name_(name) {}
  B(const std::string &name, const std::weak_ptr<InterfaceBObserver> &obs)
      : name_(name), observer_(obs) {}

  virtual void foo() {
      std::cout << "foo " << name_ << std::endl;
      if (auto ptr = observer_.lock()) {
          ptr->notified();
      }
  }

  virtual void baz() { std::cout << "baz " << name_ << std::endl; }

private:
  std::string name_;
  std::weak_ptr<InterfaceBObserver> observer_;
};

template <Type T> class C {};

template<>
class C<Type::Proxy> : public InterfaceB {
public:
    C(const std::shared_ptr<B> &ptr) : obj_(ptr) {}

    virtual void foo() {
        obj_->foo();
    }

    virtual void baz() {
        obj_->baz();
    }
private:
    std::shared_ptr<B> obj_;
};

template<>
class C<Type::Mediator> : public InterfaceB {
public:
  C(const std::shared_ptr<B> &ptr1, const std::shared_ptr<B> &ptr2)
      : obj_({ptr1, ptr2}) {}

  virtual void foo() { obj_[0]->foo(); }

  virtual void baz() { obj_[1]->baz(); }
private:
    std::array<std::shared_ptr<B>, 2> obj_;
};

template<>
class C<Type::Observer> : public InterfaceBObservable, public InterfaceBObserver {
public:
    virtual void subscribe(InterfaceBObserver *ptr) {
        subscribers_.insert(ptr);
    }

    virtual void unsubscribe(InterfaceBObserver *ptr) {
        subscribers_.erase(ptr);
    }

    void notified() {
        for (auto ptr : subscribers_) {
            ptr->notified();
        }
    }
private:
    std::set<InterfaceBObserver*> subscribers_;
};

void test_proxy() {
    auto b_ptr = std::make_shared<B>("name");
    auto c_ptr = std::make_shared<C<Type::Proxy>>(b_ptr);
    auto a_ptr = std::make_shared<A>(c_ptr);
    a_ptr->DoJob();
}

void test_mediator() {
    auto b_1_ptr = std::make_shared<B>("name1");
    auto b_2_ptr = std::make_shared<B>("name2");
    auto c_ptr = std::make_shared<C<Type::Mediator>>(b_1_ptr, b_2_ptr);
    auto a_ptr = std::make_shared<A>(c_ptr);
    a_ptr->DoJob();
}

void test_observer() {
    auto c_ptr = std::make_shared<C<Type::Observer>>();
    auto b_ptr = std::make_shared<B>("name", c_ptr);
    auto a_ptr = std::make_shared<A>(b_ptr, c_ptr);
    b_ptr->foo();
}

int main() {
    test_proxy();
    test_mediator();
    test_observer();
}
