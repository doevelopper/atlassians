#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_READERMOCK_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_READERMOCK_HPP

#include <com/github/doevelopper/atlassians/atlas/io/Reader.hpp>

namespace com::github::doevelopper::night::owl::io::test
{
    class ReaderMock
    {
      LOG4CXX_DECLARE_STATIC_LOGGER
    public:
      ReaderMock() noexcept;
      ReaderMock(const ReaderMock&) noexcept = default;
      ReaderMock(ReaderMock&&) noexcept =  default;
      ReaderMock& operator=(const ReaderMock&) noexcept = default;
      ReaderMock& operator=(ReaderMock&&) noexcept = default;
      virtual ~ReaderMock() noexcept;
    protected:
    private:
    };
}
#endif