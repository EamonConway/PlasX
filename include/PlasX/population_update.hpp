#ifndef PLASX_POPULATION_UPDATE_HPP
#define PLASX_POPULATION_UPDATE_HPP
namespace plasx{
template <class ForwardIterator, class UpdateFunction, class ReplaceFunction>
inline void population_update(ForwardIterator first, ForwardIterator last,
                       UpdateFunction updater, ReplaceFunction replacer) {
  for (; first != last; ++first) {
    if (updater(*first)) {
      *first = replacer();
    }
  }
}
}
#endif