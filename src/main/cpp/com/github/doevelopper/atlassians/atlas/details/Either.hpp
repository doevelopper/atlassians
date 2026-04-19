#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_DETAILS_EITHER_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_DETAILS_EITHER_HPP

#include <algorithm>
#include <type_traits>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::details
{
template<typename Left, typename Right>
struct Either final
{
  public:
    using left_type = Left;
    using right_type = Right;

    Either () = delete;

    explicit Either (left_type left, std::true_type isLeft = std::true_type()) noexcept(std::is_nothrow_move_constructible<Left>::value)
      : m_storage(std::move(left), isLeft)
      , m_isLeft(isLeft)
    {}

    explicit Either (right_type right, std::false_type isLeft = std::false_type()) noexcept(std::is_nothrow_move_constructible<Right>::value)
      : m_storage(std::move(right), isLeft)
      , m_isLeft(isLeft)
    {}

    Either (const Either &other) noexcept(std::is_nothrow_copy_constructible<Left>::value && std::is_nothrow_copy_constructible<Right>::value)
      : m_isLeft(other.m_isLeft)
    {
      assignFrom(other.m_storage);
    }

    Either &operator= (const Either &other) noexcept(std::is_nothrow_copy_constructible<Left>::value && std::is_nothrow_copy_constructible<Right>::value && std::is_nothrow_destructible<Left>::value && std::is_nothrow_destructible<Right>::value)
    {
      destroy();

      m_isLeft = other.m_isLeft;
      assignFrom(other.m_storage);

      return *this;
    }

    Either (Either &&other) noexcept(std::is_nothrow_move_constructible<Left>::value && std::is_nothrow_move_constructible<Right>::value)
      : m_isLeft(other.m_isLeft)
    {
      assignFrom(std::move(other.m_storage));
    }

    Either &operator= (Either &&other) noexcept(std::is_nothrow_move_constructible<Left>::value && std::is_nothrow_move_constructible<Right>::value && std::is_nothrow_destructible<Left>::value && std::is_nothrow_destructible<Right>::value)
    {
      destroy();

      m_isLeft = other.m_isLeft;
      assignFrom(std::move(other.m_storage));

      return *this;
    }

    ~Either () noexcept(std::is_nothrow_destructible<Left>::value && std::is_nothrow_destructible<Right>::value)
    {
      destroy();
    }

    bool hasLeft () const noexcept
    {
      return m_isLeft;
    }

    Left &left () noexcept
    {
      return m_storage._left;
    }

    const Left &left () const noexcept
    {
      return m_storage._left;
    }

    bool hasRight () const noexcept
    {
      return !m_isLeft;
    }

    Right &right () noexcept
    {
      return m_storage._right;
    }

    const Right &right () const noexcept
    {
      return m_storage._right;
    }

    template<typename IfLeft, typename IfRight>
    auto match (IfLeft &&ifLeft, IfRight &&ifRight) const
    {
      if (m_isLeft) {
        return std::forward<IfLeft>(ifLeft)(left());
      } else {
        return std::forward<IfRight>(ifRight)(right());
      }
    }

  private:
    union Storage {
      left_type _left;
      right_type _right;

      Storage () noexcept
      {}

      Storage (left_type left, std::true_type isLeft) noexcept(std::is_nothrow_move_constructible<Left>::value)
        : _left(std::move(left))
      {}

      Storage (right_type right, std::false_type isLeft) noexcept(std::is_nothrow_move_constructible<Right>::value)
        : _right(std::move(right))
      {}

      ~Storage ()
      {}
    } m_storage;

    bool m_isLeft;

    void destroy () noexcept(std::is_nothrow_destructible<Left>::value && std::is_nothrow_destructible<Right>::value)
    {
      if (m_isLeft) {
        m_storage._left.~Left();
      } else {
        m_storage._right.~Right();
      }
    }

    void assignFrom (const Storage &otherStorage) noexcept(std::is_nothrow_copy_constructible<Left>::value && std::is_nothrow_copy_constructible<Right>::value)
    {
      if (m_isLeft) {
        m_storage._left = otherStorage._left;
      } else {
        m_storage._right = otherStorage._right;
      }
    }

    void assignFrom (Storage &&otherStorage) noexcept(std::is_nothrow_move_constructible<Left>::value && std::is_nothrow_move_constructible<Right>::value)
    {
      if (m_isLeft) {
        m_storage._left = std::move(otherStorage._left);
      } else {
        m_storage._right = std::move(otherStorage._right);
      }
    }
};

template<typename Left, typename Right>
auto makeLeft (Left value)
{
  return Either<Left, Right>(std::forward<Left>(value), std::true_type());
}

template<typename Left, typename Right>
auto makeRight (Right value)
{
  return Either<Left, Right>(std::forward<Right>(value), std::false_type());
}

template<typename Left, typename Right>
bool operator== (const Either<Left, Right> &lhs, const Either<Left, Right> &rhs)
{
  if (lhs.hasLeft()) {
    if (rhs.hasLeft()) {
      return lhs.left() == rhs.left();
    } else {
      return false;
    }
  } else {
    if (rhs.hasRight()) {
      return lhs.right() == rhs.right();
    } else {
      return false;
    }
  }
}
}
#endif
