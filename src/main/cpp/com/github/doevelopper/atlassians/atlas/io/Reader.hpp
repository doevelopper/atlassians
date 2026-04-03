
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_READER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_READER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::io
{

    class SDLC_API_EXPORT Reader
    {
    LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using Buffer = std::vector< std::uint16_t >;

        Reader();
        virtual ~Reader();
        virtual void gather(std::int8_t &value) = 0;
        virtual void gather(std::int16_t &value) = 0;
        virtual void gather(std::int32_t &value) = 0;
        virtual void gather(std::int64_t &value) = 0;
        virtual void gather(std::uint8_t &value) = 0;
        virtual void gather(std::uint16_t &value) = 0;
        virtual void gather(std::uint32_t &value) = 0;
        virtual void gather(std::uint64_t &value) = 0;
        virtual void gather(bool &value) = 0;
        virtual void gather(float &value) = 0;
        virtual void gather(double &value) = 0;
        virtual void gather(const char *value) = 0;
        virtual void gather(std::string &value) = 0;
        virtual void gather(char *value, std::size_t &size) = 0;
        virtual void gather(Buffer &value) = 0;
        virtual void gather(void *value, std::size_t &size) = 0;
        virtual void gather(void *&value, std::size_t &size) = 0;

        template < typename T >
        int read(std::istream &in, T *ptr)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            in.read(reinterpret_cast< char * >(ptr), sizeof(T));
            return (sizeof(T));
        }
        template < typename T >
        int readArray(std::istream &in, int values, T *ptr)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            in.read(reinterpret_cast< char * >(ptr), values * sizeof(T));
            return (values * sizeof(T));
        }
        template < typename T >
        T read(std::istream &in)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            T obj;
            read< T >(in, &obj);
            return (obj);
        }

    protected:

    private:

        Q_DISABLE_COPY_MOVE(Reader)

    };
}
#endif
