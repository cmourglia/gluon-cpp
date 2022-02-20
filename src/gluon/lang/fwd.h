#pragma once

#include <memory>

namespace gluon {

template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

}  // namespace gluon

namespace gluon::lang {}  // namespace gluon::lang