
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_WRITER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_WRITER_HPP

#include <cstdint>
#include <vector>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::io
{

    class SDLC_API_EXPORT Writer
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using Buffer = std::vector< std::uint16_t >;

        Writer();
        virtual ~Writer();

        virtual void commit(std::int8_t value) = 0;
        virtual void commit(std::int16_t value) = 0;
        virtual void commit(std::int32_t value) = 0;
        virtual void commit(std::int64_t value) = 0;
        virtual void commit(std::uint8_t value) = 0;
        virtual void commit(std::uint16_t value) = 0;
        virtual void commit(std::uint32_t value) = 0;
        virtual void commit(std::uint64_t value) = 0;
        virtual void commit(bool value) = 0;
        virtual void commit(float value) = 0;
        virtual void commit(double value) = 0;
        virtual void commit(const char *value) = 0;
        virtual void commit(const std::string &value) = 0;
        virtual void commit(const char *value, std::uint64_t size) = 0;
        virtual void commit(const Buffer &value) = 0;
        virtual void commit(const void *value, std::uint64_t size) = 0;
        virtual void commit(const void *&value, std::uint64_t size) = 0;

        template < typename T >
        int write(std::ostream &out, const T &val)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            out.write(reinterpret_cast< const char * >(&val), sizeof(T));
            return (sizeof(T));
        }

        template < typename T >
        int writeArray(std::ostream &out, int values, const T *val)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            out.write(reinterpret_cast< const char * >(val), values * sizeof(T));
            return (values * sizeof(T));
        }

    protected:

    private:

        Q_DISABLE_COPY_MOVE(Writer)
    };
}
#endif
