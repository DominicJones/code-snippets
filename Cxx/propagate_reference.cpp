#include <iostream>

#include <utility>
#include <type_traits>
#include <functional>


template<typename T, typename Enable_t = void>
struct PropagateReference_const;

template<typename T>
struct PropagateReference_const<T, typename std::enable_if<!std::is_reference<T>::value>::type>
{
  using type = std::decay_t<T> const;
};

template<typename T>
struct PropagateReference_const<T, typename std::enable_if<std::is_reference<T>::value>::type>
{
  using type = std::reference_wrapper<std::decay_t<T> const>;
};

template<typename T> using PropagateReference_const_t = typename PropagateReference_const<T>::type;


template<class T>
struct Wrapper
{
  using TM = PropagateReference_const_t<T>;
  Wrapper(T &&v) : _v(static_cast<T &&>(v)) {}
  T const &member() { return _v; }
  TM const _v;
};


template<class T>
Wrapper<T> makeWrapper(T &&v)
{
  return Wrapper<T>(static_cast<T &&>(v));
}


struct Term { float value; };


int main()
{
  Term const v{4};
  auto wr = makeWrapper(v);
  auto wc = makeWrapper(Term{8});

  std::cout << wr.member().value << std::endl;
  std::cout << wc.member().value << std::endl;

  using WR = decltype(wr);
  using WC = decltype(wc);

  static_assert(std::is_same<WR::TM, std::reference_wrapper<Term const> >::value);
  static_assert(std::is_same<WC::TM, Term const>::value);
}
