#pragma once

#include <coroutine>
#include <utility>

namespace lib {
  template<typename S, typename T>
  struct Dim : private std::pair<S, T> {
    Dim() : std::pair<S, T> { } { }
    Dim(S const& s, T const& t) : 
      std::pair<S, T> { s, t } { }
    Dim(std::pair<S, T> const& p) :
      std::pair<S, T> { p } { }
    S& x() noexcept { return this->first; }
    S x() const noexcept { return this->first; }
    S& w() noexcept { return this->first; }
    S w() const noexcept { return this->first; }
    T& y() noexcept { return this->second; }
    T y() const noexcept { return this->second; }
    T& h() noexcept { return this->second; }
    T h() const noexcept { return this->second; }
  };

  using Z2 = Dim<int, int>;
  
  template<typename T>
  class Generator {
    public:
    struct promise_type {
      using Handle = std::coroutine_handle<promise_type>;
      Generator get_return_object() {
        return Generator { Handle::from_promise(*this) };
      }

      std::suspend_always initial_suspend() { return { }; }
      std::suspend_always final_suspend() noexcept { 
        return { };
      }

      template<std::convertible_to<T> From>
      // C++20 concept
      std::suspend_always yield_value(From&& from) {
        t = std::forward<From>(from);
        // caching the result in promise
        return { };
      }
      
      void unhandled_exception() { }
      void return_void() { }
      T t;
    };

    explicit Generator(promise_type::Handle h) : 
      h { h }
    { }

    ~Generator() {
      if (h)
        h.destroy();
    }
    // Make move-only
    Generator(Generator const&) = delete;
    Generator& operator=(Generator const&) = delete;
    Generator(Generator&& gen) noexcept : 
      h { std::exchange(gen.h, { }) }
    { }

    Generator& operator=(Generator&& gen) noexcept {
      if (this == &gen)
        return *this;
      if (h)
        h.destroy();

      h = std::exchange(gen.h, { });
      return *this;
    }    

    T& next() {
      h.resume();
      return h.promise().t;
    }    

    explicit operator bool() {
      return !h.done();
    }

    private:
    promise_type::Handle h;
  };
}
