#include <com/github/doevelopper/atlassians/atlas/time/ptp/Bmca.hpp>

#include <algorithm>
#include <tuple>

namespace com::github::doevelopper::atlassians::atlas::time::ptp
{
std::optional<ClockIdentity> DefaultBmcaStrategy::selectBestMaster(const std::vector<ClockIdentity> &candidates) const
{
    if (candidates.empty())
    {
        return std::nullopt;
    }

    const auto best = std::min_element(
        candidates.begin(),
        candidates.end(),
        [](const ClockIdentity &lhs, const ClockIdentity &rhs)
        {
            const auto lhsKey = std::tie(lhs.priority1, lhs.clockClass, lhs.clockAccuracy, lhs.offsetScaledLogVariance, lhs.priority2, lhs.clockId);
            const auto rhsKey = std::tie(rhs.priority1, rhs.clockClass, rhs.clockAccuracy, rhs.offsetScaledLogVariance, rhs.priority2, rhs.clockId);
            return lhsKey < rhsKey;
        });

    return *best;
}
} // namespace com::github::doevelopper::atlassians::atlas::time::ptp
