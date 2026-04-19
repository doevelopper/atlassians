#ifndef E3D6C5CF_4AC0_4833_A723_2AE9F3FF105D
#define E3D6C5CF_4AC0_4833_A723_2AE9F3FF105D
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_BMCA_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_BMCA_HPP

#include <optional>
#include <vector>

#include <com/github/doevelopper/atlassians/atlas/time/ptp/PtpTypes.hpp>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
class IBmcaStrategy
{
public:
    virtual ~IBmcaStrategy() = default;
    virtual std::optional<ClockIdentity> selectBestMaster(const std::vector<ClockIdentity> &candidates) const = 0;
};

class DefaultBmcaStrategy final : public IBmcaStrategy
{
public:
    std::optional<ClockIdentity> selectBestMaster(const std::vector<ClockIdentity> &candidates) const override;
};
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_BMCA_HPP


#endif /* E3D6C5CF_4AC0_4833_A723_2AE9F3FF105D */
