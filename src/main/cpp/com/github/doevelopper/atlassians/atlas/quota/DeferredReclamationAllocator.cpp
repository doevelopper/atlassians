
#include <com/github/doevelopper/atlassians/atlas/quota/DeferredReclamationAllocator.hpp>

using namespace com::github::doevelopper::atlassians::atlas::quota;

template <typename Allocator>
log4cxx::LoggerPtr DeferredReclamationAllocator<Allocator>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.quota.DeferredReclamationAllocator"));

