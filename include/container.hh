#ifndef IVANP_CONTAINER_HH
#define IVANP_CONTAINER_HH

#include <iterator>

#include "detect.hh"

namespace ivanp {

template <typename T>
auto reserve(size_t n) {
  std::vector<T> _v;
  _v.reserve(n);
  return _v;
}

template <typename C>
using detect_size = decltype(std::declval<C>().size());

template <typename C>
constexpr auto size(const C& c) -> decltype(c.size()) { return c.size(); }

template <typename C>
inline std::enable_if_t<
  !ivanp::is_detected<detect_size,C>::value, size_t >
size(const C& c) { return std::distance( std::begin(c), std::end(c) ); }

template <class T, size_t N>
constexpr size_t size(const T(&)[N]) noexcept { return N; }

namespace detail { namespace comprehend_container {

template <typename C, typename F>
using return_type = std::decay_t<decltype(
  std::declval<F>()( *std::begin(std::declval<C>()) ) )>;

} } // end detail

} // end namespace ivanp

template <typename C, typename F>
inline auto operator|(C&& c, F&& f)
-> std::enable_if_t<
  std::is_void<ivanp::detail::comprehend_container::return_type<C&,F&&>>::value,
  C&& >
{
  for (auto&& x : std::forward<C>(c)) std::forward<F>(f)(x);
  return std::forward<C>(c);
}

template <typename C, typename F>
inline auto operator|(C&& c, F&& f)
-> std::enable_if_t<
  !std::is_void<ivanp::detail::comprehend_container::return_type<C&,F&&>>::value,
  std::vector<ivanp::detail::comprehend_container::return_type<C&,F&&>> >
{
  std::vector<ivanp::detail::comprehend_container::return_type<C&,F&&>> vec;
  vec.reserve(ivanp::size(c));

  for (auto&& x : std::forward<C>(c))
    vec.emplace_back( std::forward<F>(f)(x) );

  return vec;
}

template <typename T>
inline std::vector<T>& operator+=(std::vector<T>& v, const std::vector<T>& u) {
  v.reserve(v.size()+u.size());
  v.insert(v.end(), u.begin(), u.end());
  return v;
}
template <typename T>
inline std::vector<T>& operator+=(std::vector<T>& v, std::vector<T>&& u) {
  v.reserve(v.size()+u.size());
  v.insert(v.end(),
    std::make_move_iterator(u.begin()),
    std::make_move_iterator(u.end())
  );
  return v;
}

#endif
