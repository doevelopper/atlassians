
#ifndef COM_GITHUB_DOEVELOPPER_ATLASSIANS_ATLAS_UTILS_NOOP_HPP
#define COM_GITHUB_DOEVELOPPER_ATLASSIANS_ATLAS_UTILS_NOOP_HPP
#include <type_traits>
namespace com::github::doevelopper::atlassians::atlas::utils {
class NoOp {
  //   Q_DISABLE_COPY_MOVE(NoOp)

 public:
  NoOp() noexcept;
  ~NoOp() noexcept;

  template <class... Args>
  constexpr void operator()(Args&&... args) const {}

  constexpr explicit operator bool() const { return (false); }

  constexpr bool operator!() const { return (true); }

  /** No-op performed on a lock() operation. */
  void lock() {
    // clang-format off

            /* *INDENT-OFF* */
            [] () { } ();
            //              |  |   |  |
            //              |  |   |  The lambda is invoked.
            //              |  |   The lambda does not do anything.
            //              |  The lambda does not take any arguments.
            //              The lambda does not capture anything.
            /* *INDENT-ON* */

    // clang-format on
  }
  // clang-format off
        /** No-op performed on an unlock() operatior. */
        /* *INDENT-OFF* */
        void unlock()
        {
            [] () { } ();
        }

        /* *INDENT-ON* */

  // clang-format on

 protected:
 private:
};
}  // namespace com::github::doevelopper::atlassians::atlas::utils

#endif  // COM_GITHUB_DOEVELOPPER_ATLASSIANS_ATLAS_UTILS_NOOP_HPP
