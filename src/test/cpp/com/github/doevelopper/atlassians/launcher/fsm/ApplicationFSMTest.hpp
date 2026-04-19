/*******************************************************************
 * @file   ApplicationFSMTest.hpp
 * @brief  Unit tests for the Application Lifecycle FSM
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSMTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSMTEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/launcher/fsm/ApplicationFSM.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::fsm::test
{
    /**
     * @brief Test fixture for ApplicationFSM tests
     */
    class ApplicationFSMTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
        
    protected:
        ApplicationFSM* m_targetUnderTest;
        FSMConfiguration m_config;
        
    public:
        /**
         * @brief Constructor
         */
        ApplicationFSMTest();
        
        /**
         * @brief Destructor
         */
        ~ApplicationFSMTest() override;
        
        /**
         * @brief Setup before each test
         */
        void SetUp() override;
        
        /**
         * @brief Teardown after each test
         */
        void TearDown() override;
        
        // ============================================
        // Helper Methods
        // ============================================
        
        /**
         * @brief Configure FSM with test settings
         */
        auto configureForTesting() -> void;
        
        /**
         * @brief Execute a complete successful startup sequence
         */
        auto executeSuccessfulStartup() -> bool;
        
        /**
         * @brief Execute startup with failure at specific phase
         */
        auto executeStartupWithFailure(const std::string& failurePhase) -> bool;
        
        /**
         * @brief Wait for state transition with timeout
         */
        auto waitForState(const std::string& expectedState, 
                         std::chrono::milliseconds timeout = std::chrono::milliseconds{1000}) -> bool;
    };

} // namespace com::github::doevelopper::atlassians::launcher::fsm::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSMTEST_HPP
