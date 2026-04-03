
#include <com/github/doevelopper/atlassians/atlas/time/AdelsonVelskyLandisTree.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

template <typename Derived>
log4cxx::LoggerPtr AdelsonVelskyLandisTree<Derived>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlas.utils.AdelsonVelskyLandisTree"));

template <typename Derived>
AdelsonVelskyLandisTree<Derived>::AdelsonVelskyLandisTree() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Derived>
AdelsonVelskyLandisTree<Derived>::~AdelsonVelskyLandisTree() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
