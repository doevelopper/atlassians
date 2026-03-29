/**
 * @file DependencyResolver.cpp
 * @brief Plugin dependency resolution implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <algorithm>
#include <regex>
#include <sstream>
#include <stack>

#include <com/github/doevelopper/atlassians/plugin/resolver/DependencyResolver.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

namespace com::github::doevelopper::atlassians::plugin::resolver
{
    log4cxx::LoggerPtr DependencyResolver::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.resolver.DependencyResolver"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    DependencyResolver::DependencyResolver()
        : DependencyResolver(Config{})
    {
    }

    DependencyResolver::DependencyResolver(const Config& config)
        : config_(config)
    {
        LOG4CXX_TRACE(logger, "DependencyResolver created");
    }

    DependencyResolver::~DependencyResolver() noexcept
    {
        LOG4CXX_TRACE(logger, "DependencyResolver destroyed");
    }

    // ============================================
    // IPluginDependencyResolver Implementation
    // ============================================

    auto DependencyResolver::resolve(const std::vector<const IPluginMetadata*>& plugins)
        -> ResolutionResult
    {
        LOG4CXX_DEBUG(logger, "Resolving dependencies for " << plugins.size() << " plugins");

        ResolutionResult result;
        result.success = true;

        // Build load order using topological sort
        result.loadOrder = topologicalSort(plugins);

        // Check for missing dependencies
        std::map<std::string, const IPluginMetadata*> pluginMap;
        for (const auto* plugin : plugins)
        {
            if (plugin)
            {
                pluginMap[std::string(plugin->getName())] = plugin;
            }
        }

        for (const auto* plugin : plugins)
        {
            if (!plugin) continue;

            for (const auto& dep : plugin->getDependencies())
            {
                if (pluginMap.find(dep.pluginName) == pluginMap.end())
                {
                    if (!dep.optional)
                    {
                        result.missingDependencies.push_back(dep.pluginName);
                        result.success = false;
                    }
                }
            }
        }

        // Check for circular dependencies
        auto cycles = detectCircularDependencies(plugins);
        if (!cycles.empty())
        {
            result.success = false;
            for (const auto& cycle : cycles)
            {
                std::string cycleStr;
                for (const auto& name : cycle)
                {
                    if (!cycleStr.empty()) cycleStr += " -> ";
                    cycleStr += name;
                }
                result.circularDependencies.push_back(cycleStr);
            }
        }

        LOG4CXX_INFO(logger, "Resolution complete. Success: " << (result.success ? "yes" : "no"));
        return result;
    }

    auto DependencyResolver::resolveFor(const IPluginMetadata& plugin,
                                        const std::vector<const IPluginMetadata*>& availablePlugins)
        -> ResolutionResult
    {
        LOG4CXX_DEBUG(logger, "Resolving for plugin: " << plugin.getName());

        ResolutionResult result;
        result.success = true;

        std::map<std::string, const IPluginMetadata*> pluginMap;
        for (const auto* p : availablePlugins)
        {
            if (p)
            {
                pluginMap[std::string(p->getName())] = p;
            }
        }

        result.loadOrder = getDependencyChain(plugin.getName(), availablePlugins);

        for (const auto& dep : plugin.getDependencies())
        {
            if (pluginMap.find(dep.pluginName) == pluginMap.end())
            {
                if (!dep.optional)
                {
                    result.missingDependencies.push_back(dep.pluginName);
                    result.success = false;
                }
            }
        }

        return result;
    }

    auto DependencyResolver::getDependencyChain(std::string_view pluginName,
                                                const std::vector<const IPluginMetadata*>& availablePlugins) const
        -> std::vector<std::string>
    {
        std::vector<std::string> chain;
        std::set<std::string> visited;

        std::map<std::string, const IPluginMetadata*> pluginMap;
        for (const auto* p : availablePlugins)
        {
            if (p)
            {
                pluginMap[std::string(p->getName())] = p;
            }
        }

        std::function<void(const std::string&)> visit = [&](const std::string& name) {
            if (visited.count(name) > 0) return;
            visited.insert(name);

            auto it = pluginMap.find(name);
            if (it != pluginMap.end() && it->second)
            {
                for (const auto& dep : it->second->getDependencies())
                {
                    visit(dep.pluginName);
                }
            }
            chain.push_back(name);
        };

        visit(std::string(pluginName));
        return chain;
    }

    auto DependencyResolver::getDependents(std::string_view pluginName,
                                           const std::vector<const IPluginMetadata*>& allPlugins) const
        -> std::vector<std::string>
    {
        std::vector<std::string> dependents;
        const std::string targetName(pluginName);

        for (const auto* plugin : allPlugins)
        {
            if (!plugin) continue;

            for (const auto& dep : plugin->getDependencies())
            {
                if (dep.pluginName == targetName)
                {
                    dependents.push_back(std::string(plugin->getName()));
                    break;
                }
            }
        }

        return dependents;
    }

    auto DependencyResolver::isVersionCompatible(std::string_view version,
                                                  std::string_view constraint) const
        -> bool
    {
        if (constraint.empty())
        {
            return true; // No constraint means any version is acceptable
        }

        auto [major, minor, patch, pre, build] = parseVersion(version);

        // Simple constraint parsing - supports >=, <=, =, >, <
        std::string constraintStr(constraint);

        // Handle range constraints like ">=1.0.0, <2.0.0"
        std::string::size_type pos = 0;
        while (pos < constraintStr.size())
        {
            // Find next constraint
            auto commaPos = constraintStr.find(',', pos);
            std::string singleConstraint = constraintStr.substr(pos,
                commaPos != std::string::npos ? commaPos - pos : std::string::npos);

            // Trim whitespace
            while (!singleConstraint.empty() && std::isspace(singleConstraint.front()))
                singleConstraint.erase(0, 1);
            while (!singleConstraint.empty() && std::isspace(singleConstraint.back()))
                singleConstraint.pop_back();

            if (!singleConstraint.empty())
            {
                std::string op;
                std::string verStr;

                if (singleConstraint.substr(0, 2) == ">=" || singleConstraint.substr(0, 2) == "<=")
                {
                    op = singleConstraint.substr(0, 2);
                    verStr = singleConstraint.substr(2);
                }
                else if (singleConstraint[0] == '>' || singleConstraint[0] == '<' || singleConstraint[0] == '=')
                {
                    op = singleConstraint.substr(0, 1);
                    verStr = singleConstraint.substr(1);
                }
                else
                {
                    op = "=";
                    verStr = singleConstraint;
                }

                // Trim version string
                while (!verStr.empty() && std::isspace(verStr.front()))
                    verStr.erase(0, 1);

                int cmp = compareVersions(version, verStr);

                if (op == "=" && cmp != 0) return false;
                if (op == ">" && cmp <= 0) return false;
                if (op == ">=" && cmp < 0) return false;
                if (op == "<" && cmp >= 0) return false;
                if (op == "<=" && cmp > 0) return false;
            }

            if (commaPos == std::string::npos) break;
            pos = commaPos + 1;
        }

        return true;
    }

    auto DependencyResolver::parseVersion(std::string_view version) const
        -> std::tuple<int, int, int, std::string, std::string>
    {
        int major = 0, minor = 0, patch = 0;
        std::string prerelease, build;

        std::string verStr(version);

        // Extract build metadata
        auto plusPos = verStr.find('+');
        if (plusPos != std::string::npos)
        {
            build = verStr.substr(plusPos + 1);
            verStr = verStr.substr(0, plusPos);
        }

        // Extract prerelease
        auto dashPos = verStr.find('-');
        if (dashPos != std::string::npos)
        {
            prerelease = verStr.substr(dashPos + 1);
            verStr = verStr.substr(0, dashPos);
        }

        // Parse major.minor.patch
        std::sscanf(verStr.c_str(), "%d.%d.%d", &major, &minor, &patch);

        return {major, minor, patch, prerelease, build};
    }

    auto DependencyResolver::compareVersions(std::string_view version1,
                                              std::string_view version2) const
        -> int
    {
        auto [major1, minor1, patch1, pre1, build1] = parseVersion(version1);
        auto [major2, minor2, patch2, pre2, build2] = parseVersion(version2);

        if (major1 != major2) return major1 < major2 ? -1 : 1;
        if (minor1 != minor2) return minor1 < minor2 ? -1 : 1;
        if (patch1 != patch2) return patch1 < patch2 ? -1 : 1;

        // Prerelease versions have lower precedence
        if (pre1.empty() && !pre2.empty()) return 1;
        if (!pre1.empty() && pre2.empty()) return -1;
        if (!pre1.empty() && !pre2.empty()) return pre1.compare(pre2);

        return 0;
    }

    auto DependencyResolver::detectCircularDependencies(
        const std::vector<const IPluginMetadata*>& plugins) const
        -> std::vector<std::vector<std::string>>
    {
        std::vector<std::vector<std::string>> cycles;

        // Build adjacency list
        std::map<std::string, std::vector<std::string>> graph;
        for (const auto* plugin : plugins)
        {
            if (!plugin) continue;
            std::string name(plugin->getName());
            for (const auto& dep : plugin->getDependencies())
            {
                graph[name].push_back(dep.pluginName);
            }
        }

        // DFS to detect cycles
        std::map<std::string, NodeColor> colors;
        for (const auto& [name, deps] : graph)
        {
            colors[name] = NodeColor::White;
        }

        for (const auto& [name, deps] : graph)
        {
            if (colors[name] == NodeColor::White)
            {
                std::vector<std::string> path;
                detectCycleHelper(name, graph, colors, path, cycles);
            }
        }

        return cycles;
    }

    auto DependencyResolver::detectCycleHelper(const std::string& node,
                                               const std::map<std::string, std::vector<std::string>>& graph,
                                               std::map<std::string, NodeColor>& colors,
                                               std::vector<std::string>& path,
                                               std::vector<std::vector<std::string>>& cycles) const -> bool
    {
        colors[node] = NodeColor::Gray;
        path.push_back(node);

        auto it = graph.find(node);
        if (it != graph.end())
        {
            for (const auto& neighbor : it->second)
            {
                if (colors[neighbor] == NodeColor::Gray)
                {
                    // Found cycle - extract it from path
                    std::vector<std::string> cycle;
                    bool inCycle = false;
                    for (const auto& p : path)
                    {
                        if (p == neighbor) inCycle = true;
                        if (inCycle) cycle.push_back(p);
                    }
                    cycle.push_back(neighbor);
                    cycles.push_back(cycle);
                    return true;
                }
                else if (colors[neighbor] == NodeColor::White)
                {
                    if (detectCycleHelper(neighbor, graph, colors, path, cycles))
                    {
                        // Continue to find more cycles
                    }
                }
            }
        }

        colors[node] = NodeColor::Black;
        path.pop_back();
        return false;
    }

    auto DependencyResolver::wouldCreateCycle(std::string_view fromPlugin,
                                              std::string_view toPlugin,
                                              const std::vector<const IPluginMetadata*>& allPlugins) const
        -> bool
    {
        // Check if 'toPlugin' depends (transitively) on 'fromPlugin'
        std::set<std::string> visited;
        std::stack<std::string> toVisit;
        toVisit.push(std::string(toPlugin));

        std::map<std::string, const IPluginMetadata*> pluginMap;
        for (const auto* p : allPlugins)
        {
            if (p)
            {
                pluginMap[std::string(p->getName())] = p;
            }
        }

        while (!toVisit.empty())
        {
            std::string current = toVisit.top();
            toVisit.pop();

            if (current == fromPlugin) return true;

            if (visited.count(current) > 0) continue;
            visited.insert(current);

            auto it = pluginMap.find(current);
            if (it != pluginMap.end() && it->second)
            {
                for (const auto& dep : it->second->getDependencies())
                {
                    toVisit.push(dep.pluginName);
                }
            }
        }

        return false;
    }

    auto DependencyResolver::validateDependencies(
        const std::vector<const IPluginMetadata*>& plugins) const
        -> ResolutionResult
    {
        // Just delegate to resolve since it does validation
        return const_cast<DependencyResolver*>(this)->resolve(plugins);
    }

    auto DependencyResolver::getMissingOptionalDependencies(
        const IPluginMetadata& plugin,
        const std::vector<const IPluginMetadata*>& availablePlugins) const
        -> std::vector<std::string>
    {
        std::vector<std::string> missing;

        std::set<std::string> available;
        for (const auto* p : availablePlugins)
        {
            if (p)
            {
                available.insert(std::string(p->getName()));
            }
        }

        for (const auto& dep : plugin.getDependencies())
        {
            if (dep.optional && available.find(dep.pluginName) == available.end())
            {
                missing.push_back(dep.pluginName);
            }
        }

        return missing;
    }

    // ============================================
    // Additional Methods
    // ============================================

    auto DependencyResolver::clearCache() -> void
    {
        std::unique_lock lock(cacheMutex_);
        resolutionCache_.clear();
        LOG4CXX_DEBUG(logger, "Resolution cache cleared");
    }

    auto DependencyResolver::getConfig() const noexcept -> const Config&
    {
        return config_;
    }

    auto DependencyResolver::setConfig(const Config& config) -> void
    {
        config_ = config;
        clearCache();
        LOG4CXX_DEBUG(logger, "Configuration updated");
    }

    // ============================================
    // Internal Methods
    // ============================================

    auto DependencyResolver::topologicalSort(const std::vector<const IPluginMetadata*>& plugins) const
        -> std::vector<std::string>
    {
        std::vector<std::string> result;
        std::set<std::string> visited;
        std::set<std::string> inStack;

        std::map<std::string, const IPluginMetadata*> pluginMap;
        for (const auto* p : plugins)
        {
            if (p)
            {
                pluginMap[std::string(p->getName())] = p;
            }
        }

        std::function<void(const std::string&)> visit = [&](const std::string& name) {
            if (visited.count(name) > 0) return;
            if (inStack.count(name) > 0) return; // Cycle detected, skip

            inStack.insert(name);

            auto it = pluginMap.find(name);
            if (it != pluginMap.end() && it->second)
            {
                for (const auto& dep : it->second->getDependencies())
                {
                    visit(dep.pluginName);
                }
            }

            inStack.erase(name);
            visited.insert(name);
            result.push_back(name);
        };

        for (const auto* p : plugins)
        {
            if (p)
            {
                visit(std::string(p->getName()));
            }
        }

        return result;
    }

} // namespace com::github::doevelopper::atlassians::plugin::resolver
