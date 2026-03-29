#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_WRITERMOCK_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_WRITERMOCK_HPP

#include <com/github/doevelopper/night/owl/io/Writer.hpp>

namespace com::github::doevelopper::night::owl::io::test
{
  class WriterMock
  {
    LOG4CXX_DECLARE_STATIC_LOGGER
  public:
    WriterMock() noexcept;
    WriterMock(const WriterMock&) noexcept = default;
    WriterMock(WriterMock&&) noexcept =  default;
    WriterMock& operator=(const WriterMock&) noexcept = default;
    WriterMock& operator=(WriterMock&&) noexcept = default;
    virtual ~WriterMock() noexcept;
  protected:
  private:
  };
}
#endif