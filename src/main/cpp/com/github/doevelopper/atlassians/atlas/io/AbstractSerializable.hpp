
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_ABSTRACT_SERIALIZABLE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_IO_ABSTRACT_SERIALIZABLE_HPP

#include <cstdint>
#include <com/github/doevelopper/atlassians/atlas/io/Reader.hpp>
#include <com/github/doevelopper/atlassians/atlas/io/Writer.hpp>

namespace com::github::doevelopper::atlassians::atlas::io
{
    #define GG_ULONGLONG(x)   x##ULL
    static inline std::uint16_t ByteSwap16(std::uint16_t x)
    {
        return static_cast<std::uint16_t>(((x & 0xFF) << 8) | ((x & 0xFF00) >> 8));
    }

    static inline std::uint32_t ByteSwap32(std::uint32_t x)
    {
        return (((x & 0xFF) << 24UL) |
                ((x & 0xFF00) << 8UL) |
                ((x & 0xFF0000) >> 8UL) |
                ((x & 0xFF000000) >> 24UL));
    }

    static inline std::uint64_t ByteSwap64(std::uint64_t x)
    {
        return (((x & GG_ULONGLONG(0xFF)) << 56UL) |
                ((x & GG_ULONGLONG(0xFF00)) << 40UL) |
                ((x & GG_ULONGLONG(0xFF0000)) << 24UL) |
                ((x & GG_ULONGLONG(0xFF000000)) << 8UL) |
                ((x & GG_ULONGLONG(0xFF00000000)) >> 8UL) |
                ((x & GG_ULONGLONG(0xFF0000000000)) >> 24UL) |
                ((x & GG_ULONGLONG(0xFF000000000000)) >> 40UL) |
                ((x & GG_ULONGLONG(0xFF00000000000000)) >> 56UL));
    }

    #define bswap_16(x) ByteSwap16(x)
    #define bswap_32(x) ByteSwap32(x)
    #define bswap_64(x) ByteSwap64(x)

    class SDLC_API_EXPORT AbstractSerializable
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        AbstractSerializable();
        virtual ~AbstractSerializable();

        Writer &operator>>(Writer &stream) const
        {
            serialize(stream);
            return (stream);
        }

        Reader &operator<<(Reader &stream)
        {
            deSerialize(stream);
            return (stream);
        }

    protected:

        Q_DISABLE_COPY_MOVE(AbstractSerializable)
        virtual void serialize(Writer &writer) const = 0;
        virtual void deSerialize(Reader &reader) = 0;

    private:
    };

}  // namespace cfs::osal

#endif
