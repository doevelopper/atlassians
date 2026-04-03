#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_SERIALIZABLEMOCK_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_IO_SERIALIZABLEMOCK_HPP

#include <com/github/doevelopper/night/owl/io/ReaderMock.hpp>
#include <com/github/doevelopper/night/owl/io/WriterMock.hpp>

namespace com::github::doevelopper::night::owl::io::test
{
    class SerializableMock
    {
      LOG4CXX_DECLARE_STATIC_LOGGER
    public:
      SerializableMock() noexcept;
      SerializableMock(const SerializableMock&) noexcept = default;
      SerializableMock(SerializableMock&&) noexcept =  default;
      SerializableMock& operator=(const SerializableMock&) noexcept = default;
      SerializableMock& operator=(SerializableMock&&) noexcept = default;
      virtual ~SerializableMock() noexcept;
    protected:
    private:
    };
}
#endif