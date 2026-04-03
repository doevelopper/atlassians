# Example Usage of PtpTimestamp in a PTP Implementation

Let me provide several practical examples showing how the `PtpTimestamp` class would be used in a complete Precision Time Protocol implementation. These examples demonstrate real-world applications of the class across different aspects of the protocol.

## Example 1: Basic Timestamp Creation and Manipulation

```cpp
#include "PtpTimestamp.h"
#include <iostream>
#include <chrono>

void timestampBasics() {
    // Create a timestamp for the current time
    PtpTimestamp now = PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
    
    // Create a timestamp with explicit values
    PtpTimestamp specific(1618426800, 500000000); // April 14, 2021, 12:00:00.5
    
    // Calculate time difference
    TimeInterval diff = specific - now;
    
    std::cout << "Current PTP time: " << now.toString() << std::endl;
    std::cout << "Specific time: " << specific.toString() << std::endl;
    std::cout << "Time difference: " << diff.toString() << std::endl;
    
    // Create a timestamp for 10 seconds in the future
    TimeInterval tenSeconds(10, 0);
    PtpTimestamp future = now + tenSeconds;
    
    std::cout << "Time 10 seconds from now: " << future.toString() << std::endl;
    
    // Compare timestamps
    if (future > now) {
        std::cout << "Future timestamp is indeed later than current time." << std::endl;
    }
}
```

## Example 2: PTP Sync Message Processing

This example shows how `PtpTimestamp` would be used in a slave clock when processing a Sync message from a master clock:

```cpp
void processSyncMessage(const SyncMessage& syncMsg, const PtpTimestamp& receiveTimestamp) {
    // Extract origin timestamp from the Sync message
    PtpTimestamp originTimestamp = syncMsg.getOriginTimestamp();
    
    // Print timestamps for debugging
    std::cout << "Master clock sent Sync message at: " << originTimestamp.toString() << std::endl;
    std::cout << "Slave clock received Sync message at: " << receiveTimestamp.toString() << std::endl;
    
    // Calculate propagation delay and clock offset (simplified - real implementation would use Delay_Req/Resp too)
    TimeInterval propagationDelay = calculatePropagationDelay();
    
    // Master-to-slave delay = receive time - send time - propagation delay
    TimeInterval masterToSlaveDelay = receiveTimestamp - originTimestamp - propagationDelay;
    
    std::cout << "Estimated master-to-slave delay: " << masterToSlaveDelay.toString() << std::endl;
    
    // Update local clock offset (simplified)
    updateClockOffset(masterToSlaveDelay);
}

TimeInterval calculatePropagationDelay() {
    // In a real implementation, this would use Delay_Req/Delay_Resp messages
    // to measure round-trip time and estimate one-way delay
    
    // For this example, just return a dummy value
    return TimeInterval(0, 200000); // 200 microseconds
}

void updateClockOffset(const TimeInterval& offset) {
    std::cout << "Adjusting local clock by: " << offset.toString() << std::endl;
    
    // In a real implementation, this would adjust the system clock
    // or hardware clock using platform-specific APIs
}
```

## Example 3: Two-Step Clock Synchronization Process

A more complete example showing the full two-step synchronization process using `PtpTimestamp`:

```cpp
class PtpSlave {
public:
    void synchronize() {
        // Step 1: Process Sync message
        SyncMessage syncMsg = receiveSync();
        PtpTimestamp t2 = getPreciseReceiveTime(); // Receive timestamp
        
        // Step 2: Process Follow_Up message (for two-step clocks)
        FollowUpMessage followUpMsg = receiveFollowUp();
        PtpTimestamp t1 = followUpMsg.getPreciseOriginTimestamp();
        
        // Step 3: Send Delay_Request
        PtpTimestamp t3;
        sendDelayRequest(t3); // t3 is set to precise send time
        
        // Step 4: Process Delay_Response
        DelayResponseMessage delayRespMsg = receiveDelayResponse();
        PtpTimestamp t4 = delayRespMsg.getReceiveTimestamp();
        
        // Calculate clock offset and path delay
        calculateAndApplyCorrection(t1, t2, t3, t4);
    }

private:
    SyncMessage receiveSync() {
        // Simulate receiving a Sync message from the network
        SyncMessage msg;
        // ... populate message fields
        return msg;
    }
    
    PtpTimestamp getPreciseReceiveTime() {
        // In real implementation, this might use hardware timestamping
        return PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
    }
    
    FollowUpMessage receiveFollowUp() {
        // Simulate receiving a Follow_Up message
        FollowUpMessage msg;
        // ... populate message fields including precise origin timestamp
        return msg;
    }
    
    void sendDelayRequest(PtpTimestamp& sendTime) {
        // Create Delay_Request message
        DelayRequestMessage msg;
        
        // Send message and capture precise send time
        // ... network send code
        
        sendTime = PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
    }
    
    DelayResponseMessage receiveDelayResponse() {
        // Simulate receiving a Delay_Response message
        DelayResponseMessage msg;
        // ... populate message fields
        return msg;
    }
    
    void calculateAndApplyCorrection(
        const PtpTimestamp& t1, // Master sync send time
        const PtpTimestamp& t2, // Slave sync receive time
        const PtpTimestamp& t3, // Slave delay request send time
        const PtpTimestamp& t4  // Master delay request receive time
    ) {
        // Calculate mean path delay: [(t2-t1) + (t4-t3)]/2
        TimeInterval masterToSlave = t2 - t1;
        TimeInterval slaveToMaster = t4 - t3;
        
        // Mean path delay
        TimeInterval meanPathDelay = (masterToSlave + slaveToMaster) / 2;
        
        // Calculate offset from master: [(t2-t1) - (t4-t3)]/2
        TimeInterval offset = masterToSlave - slaveToMaster;
        offset = offset / 2;
        
        std::cout << "Mean path delay: " << meanPathDelay.toString() << std::endl;
        std::cout << "Clock offset: " << offset.toString() << std::endl;
        
        // Apply correction to local clock
        applyClockCorrection(offset);
    }
    
    void applyClockCorrection(const TimeInterval& offset) {
        // In a real implementation, adjust system or hardware clock
        std::cout << "Adjusting local clock by: " << offset.toString() << std::endl;
    }
};
```

## Example 4: PTP Message Serialization with Timestamps

This example shows how `PtpTimestamp` is used in PTP message serialization:

```cpp
class SyncMessage {
public:
    SyncMessage() = default;
    
    explicit SyncMessage(const PtpTimestamp& originTimestamp) 
        : m_originTimestamp(originTimestamp) {}
    
    PtpTimestamp getOriginTimestamp() const {
        return m_originTimestamp;
    }
    
    void setOriginTimestamp(const PtpTimestamp& timestamp) {
        m_originTimestamp = timestamp;
    }
    
    std::vector<std::byte> serialize() const {
        // PTP common header (34 bytes)
        std::vector<std::byte> data(34);
        
        // Set message type (0x0 for Sync)
        data[0] = std::byte{0x00};
        
        // ... other header fields ...
        
        // Serialize origin timestamp at the appropriate offset (bytes 34-43)
        std::vector<std::byte> timestampBytes = m_originTimestamp.serialize();
        data.insert(data.end(), timestampBytes.begin(), timestampBytes.end());
        
        return data;
    }
    
    static SyncMessage deserialize(const std::vector<std::byte>& data) {
        SyncMessage message;
        
        // Extract timestamp bytes (10 bytes starting at offset 34)
        std::vector<std::byte> timestampBytes(data.begin() + 34, data.begin() + 44);
        
        // Deserialize the timestamp
        PtpTimestamp timestamp = PtpTimestamp::deserialize(timestampBytes);
        message.setOriginTimestamp(timestamp);
        
        return message;
    }
    
private:
    PtpTimestamp m_originTimestamp;
    // Other message fields...
};

// Usage example
void sendSyncMessage(INetworkAdapter& network) {
    // Create a Sync message with current time
    PtpTimestamp now = PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
    SyncMessage syncMsg(now);
    
    // Serialize and send the message
    std::vector<std::byte> serializedData = syncMsg.serialize();
    network.sendMessage(serializedData);
    
    std::cout << "Sent Sync message with timestamp: " << now.toString() << std::endl;
}
```

## Example 5: Best Master Clock Algorithm (BMCA)

This example shows how timestamps are used in the BMCA implementation:

```cpp
class BestMasterClockAlgorithm {
public:
    void processAnnounceMessage(const AnnounceMessage& message, const PortIdentity& receivingPort) {
        // Extract the timestamp from the announce message
        PtpTimestamp messageTimestamp = message.getOriginTimestamp();
        
        // Record when we received this message
        PtpTimestamp receiveTime = PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
        
        // Calculate message age
        TimeInterval messageAge = receiveTime - messageTimestamp;
        
        // Check if message is too old (timeout)
        if (messageAge > TimeInterval(2, 0)) { // 2 second timeout
            std::cout << "Discarding stale Announce message, age: " << messageAge.toString() << std::endl;
            return;
        }
        
        // Extract clock quality information
        ClockQuality remoteClock = message.getGrandmasterClockQuality();
        
        // Compare with our current best master
        if (isBetterMaster(remoteClock, m_currentBestMaster)) {
            std::cout << "New best master found from port: " << receivingPort.toString() << std::endl;
            m_currentBestMaster = remoteClock;
            m_currentBestMasterPort = receivingPort;
            
            // Update state machine
            updateStateMachine();
        }
    }
    
private:
    bool isBetterMaster(const ClockQuality& candidate, const ClockQuality& current) {
        // Compare clock qualities according to IEEE 1588 algorithm
        // This is a simplified version
        
        // First compare priority1
        if (candidate.priority1 < current.priority1) return true;
        if (candidate.priority1 > current.priority1) return false;
        
        // Then compare clock class
        if (candidate.clockClass < current.clockClass) return true;
        if (candidate.clockClass > current.clockClass) return false;
        
        // And so on with other parameters...
        
        return false;
    }
    
    void updateStateMachine() {
        // Transition to appropriate state based on BMCA result
    }
    
    ClockQuality m_currentBestMaster;
    PortIdentity m_currentBestMasterPort;
};
```

## Example 6: Integration with Hardware Timestamping (Linux PTP Implementation)

This example shows how `PtpTimestamp` might be used with hardware timestamping on Linux:

```cpp
#include <linux/ptp_clock.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

class HardwareTimestampingAdapter {
public:
    HardwareTimestampingAdapter(const std::string& ptpDevicePath = "/dev/ptp0") {
        // Open PTP hardware clock device
        m_clockFd = open(ptpDevicePath.c_str(), O_RDWR);
        if (m_clockFd < 0) {
            std::cerr << "Failed to open PTP hardware clock: " << ptpDevicePath << std::endl;
            throw std::runtime_error("Hardware timestamping initialization failed");
        }
        
        // Get clock capabilities
        struct ptp_clock_caps caps;
        if (ioctl(m_clockFd, PTP_CLOCK_GETCAPS, &caps) < 0) {
            std::cerr << "Failed to get PTP clock capabilities" << std::endl;
            close(m_clockFd);
            throw std::runtime_error("Hardware timestamping capability check failed");
        }
        
        std::cout << "PTP Hardware Clock capabilities:" << std::endl;
        std::cout << "  Max adjustments per second: " << caps.max_adj << std::endl;
        std::cout << "  Number of programmable pins: " << caps.n_pins << std::endl;
        std::cout << "  Supports external timestamp: " << (caps.extts ? "Yes" : "No") << std::endl;
        std::cout << "  Supports periodic output: " << (caps.pps ? "Yes" : "No") << std::endl;
    }
    
    ~HardwareTimestampingAdapter() {
        if (m_clockFd >= 0) {
            close(m_clockFd);
        }
    }
    
    PtpTimestamp getCurrentTime() const {
        struct ptp_clock_time ptpTime;
        
        // Get current hardware timestamp
        if (ioctl(m_clockFd, PTP_CLOCK_GETTIME, &ptpTime) < 0) {
            std::cerr << "Failed to get PTP hardware time" << std::endl;
            throw std::runtime_error("Hardware time read failed");
        }
        
        // Convert to PtpTimestamp
        return PtpTimestamp(ptpTime.sec, ptpTime.nsec);
    }
    
    void adjustClock(const TimeInterval& offset) {
        struct ptp_clock_time ptpTime;
        ptpTime.sec = offset.getSeconds();
        ptpTime.nsec = offset.getNanoseconds();
        
        // Set time offset
        if (ioctl(m_clockFd, PTP_CLOCK_SETTIME, &ptpTime) < 0) {
            std::cerr << "Failed to adjust PTP hardware time" << std::endl;
            throw std::runtime_error("Hardware time adjustment failed");
        }
        
        std::cout << "Adjusted hardware clock by: " << offset.toString() << std::endl;
    }
    
    void adjustFrequency(int ppb) {
        // Set frequency adjustment in parts-per-billion
        if (ioctl(m_clockFd, PTP_CLOCK_ADJFREQ, &ppb) < 0) {
            std::cerr << "Failed to adjust PTP hardware frequency" << std::endl;
            throw std::runtime_error("Hardware frequency adjustment failed");
        }
        
        std::cout << "Adjusted hardware clock frequency by: " << ppb << " ppb" << std::endl;
    }
    
private:
    int m_clockFd = -1;
};

// Usage example
void hardwareTimestampingExample() {
    try {
        HardwareTimestampingAdapter hwClock;
        
        // Get current hardware time
        PtpTimestamp hwTime = hwClock.getCurrentTime();
        std::cout << "Hardware clock time: " << hwTime.toString() << std::endl;
        
        // Get system time
        PtpTimestamp sysTime = PtpTimestamp::fromSystemTime(std::chrono::system_clock::now());
        std::cout << "System clock time: " << sysTime.toString() << std::endl;
        
        // Calculate offset
        TimeInterval offset = sysTime - hwTime;
        std::cout << "Offset between system and hardware clock: " << offset.toString() << std::endl;
        
        // Adjust hardware clock if needed
        if (std::abs(offset.getSeconds()) > 0 || std::abs(offset.getNanoseconds()) > 1000000) {
            std::cout << "Adjusting hardware clock to match system time..." << std::endl;
            hwClock.adjustClock(offset);
        }
        
        // Adjust frequency slightly (5 ppb)
        hwClock.adjustFrequency(5);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
```

## Example 7: Complete Integration of PtpTimestamp in PTP Engine

This example shows how `PtpTimestamp` is integrated within the core PTP engine class:

```cpp
class PtpEngine {
public:
    PtpEngine(const PtpConfig& config) 
        : m_config(config),
          m_running(false) {
        
        // Initialize network adapter
        m_network = createNetworkAdapter(config.networkType);
        
        // Initialize clock
        m_clock = createPtpClock(config.clockType);
        
        // Initialize state machine
        m_stateMachine = std::make_unique<PtpStateMachine>(*m_clock, *m_network);
    }
    
    void start() {
        m_running = true;
        
        // Start message processing thread
        m_processingThread = std::thread(&PtpEngine::processMessages, this);
        
        std::cout << "PTP Engine started at: " 
                  << PtpTimestamp::fromSystemTime(std::chrono::system_clock::now()).toString() 
                  << std::endl;
    }
    
    void stop() {
        m_running = false;
        
        if (m_processingThread.joinable()) {
            m_processingThread.join();
        }
        
        std::cout << "PTP Engine stopped at: " 
                  << PtpTimestamp::fromSystemTime(std::chrono::system_clock::now()).toString() 
                  << std::endl;
    }
    
    PtpTimestamp getCurrentTime() const {
        return m_clock->getCurrentTime();
    }
    
    PtpState getCurrentState() const {
        return m_stateMachine->getCurrentState();
    }
    
private:
    void processMessages() {
        while (m_running) {
            try {
                // Try to receive a message with timeout
                auto messageOpt = m_network->receiveMessage(std::chrono::milliseconds(100));
                
                if (messageOpt) {
                    // Get receive timestamp (ideally from hardware)
                    PtpTimestamp receiveTime = m_network->getLastReceiveTimestamp();
                    
                    // Process the message through state machine
                    m_stateMachine->processMessage(*messageOpt.value(), receiveTime);
                }
                
                // Periodic tasks
                m_stateMachine->runPeriodicTasks();
                
            } catch (const std::exception& e) {
                std::cerr << "Error in message processing: " << e.what() << std::endl;
            }
        }
    }
    
    std::unique_ptr<INetworkAdapter> createNetworkAdapter(NetworkType type) {
        // Factory method for network adapter creation
        switch (type) {
            case NetworkType::Ethernet:
                return std::make_unique<EthernetAdapter>(m_config.networkInterface);
            case NetworkType::UdpIpv4:
                return std::make_unique<UdpIpv4Adapter>(m_config.networkInterface);
            case NetworkType::UdpIpv6:
                return std::make_unique<UdpIpv6Adapter>(m_config.networkInterface);
            default:
                throw std::invalid_argument("Unsupported network type");
        }
    }
    
    std::unique_ptr<IPtpClock> createPtpClock(ClockType type) {
        // Factory method for clock creation
        switch (type) {
            case ClockType::Ordinary:
                return std::make_unique<OrdinaryClock>(m_config.clockConfig);
            case ClockType::Boundary:
                return std::make_unique<BoundaryClock>(m_config.clockConfig);
            case ClockType::Transparent:
                return std::make_unique<TransparentClock>(
                    m_config.transparentClockType,
                    m_config.clockConfig
                );
            default:
                throw std::invalid_argument("Unsupported clock type");
        }
    }
    
    PtpConfig m_config;
    std::unique_ptr<INetworkAdapter> m_network;
    std::unique_ptr<IPtpClock> m_clock;
    std::unique_ptr<PtpStateMachine> m_stateMachine;
    
    std::atomic<bool> m_running;
    std::thread m_processingThread;
};

// Example of starting a PTP instance
void runPtpInstance() {
    // Create configuration
    PtpConfig config;
    config.clockType = ClockType::Ordinary;
    config.networkType = NetworkType::UdpIpv4;
    config.networkInterface = "eth0";
    config.clockConfig.priority1 = 128;
    config.clockConfig.priority2 = 128;
    
    // Create and start PTP engine
    PtpEngine engine(config);
    engine.start();
    
    // Get current PTP time
    PtpTimestamp ptpTime = engine.getCurrentTime();
    std::cout << "Current PTP time: " << ptpTime.toString() << std::endl;
    
    // Run for 60 seconds
    std::this_thread::sleep_for(std::chrono::seconds(60));
    
    // Check final state and time
    std::cout << "Final PTP state: " << toString(engine.getCurrentState()) << std::endl;
    std::cout << "Final PTP time: " << engine.getCurrentTime().toString() << std::endl;
    
    // Stop the engine
    engine.stop();
}
```

## Example 8: Using PtpTimestamp in a Test Suite

This example shows how `PtpTimestamp` would be used in a test suite for PTP functionality:

```cpp
#include <gtest/gtest.h>

class PtpTimestampTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup common test infrastructure
    }
    
    void TearDown() override {
        // Clean up
    }
};

TEST_F(PtpTimestampTest, Creation) {
    // Test default constructor
    PtpTimestamp defaultTs;
    EXPECT_EQ(defaultTs.getSeconds(), 0);
    EXPECT_EQ(defaultTs.getNanoseconds(), 0);
    
    // Test explicit constructor
    PtpTimestamp explicitTs(123, 456789000);
    EXPECT_EQ(explicitTs.getSeconds(), 123);
    EXPECT_EQ(explicitTs.getNanoseconds(), 456789000);
    
    // Test system time conversion roundtrip
    auto now = std::chrono::system_clock::now();
    PtpTimestamp fromSystemTime = PtpTimestamp::fromSystemTime(now);
    auto roundTrip = fromSystemTime.toSystemTime();
    
    // Allow 1 microsecond rounding difference due to potential precision loss
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(
        roundTrip - now).count();
    EXPECT_LE(std::abs(diff), 1);
}

TEST_F(PtpTimestampTest, Arithmetic) {
    PtpTimestamp ts1(100, 500000000);
    PtpTimestamp ts2(103, 700000000);
    
    // Test subtraction
    TimeInterval diff = ts2 - ts1;
    EXPECT_EQ(diff.getSeconds(), 3);
    EXPECT_EQ(diff.getNanoseconds(), 200000000);
    
    // Test addition
    TimeInterval interval(2, 800000000);
    PtpTimestamp ts3 = ts1 + interval;
    EXPECT_EQ(ts3.getSeconds(), 103);
    EXPECT_EQ(ts3.getNanoseconds(), 300000000);
    
    // Test normalization
    PtpTimestamp ts4(10, 1500000000);  // Over 1 billion nanoseconds
    EXPECT_EQ(ts4.getSeconds(), 11);
    EXPECT_EQ(ts4.getNanoseconds(), 500000000);
}

TEST_F(PtpTimestampTest, Comparison) {
    PtpTimestamp ts1(100, 500000000);
    PtpTimestamp ts2(100, 500000000);
    PtpTimestamp ts3(100, 600000000);
    PtpTimestamp ts4(101, 200000000);
    
    // Test equality
    EXPECT_TRUE(ts1 == ts2);
    EXPECT_FALSE(ts1 == ts3);
    
    // Test inequality
    EXPECT_TRUE(ts1 != ts3);
    EXPECT_FALSE(ts1 != ts2);
    
    // Test less than
    EXPECT_TRUE(ts1 < ts3);
    EXPECT_TRUE(ts3 < ts4);
    EXPECT_FALSE(ts4 < ts1);
    
    // Test greater than
    EXPECT_TRUE(ts4 > ts3);
    EXPECT_TRUE(ts3 > ts1);
    EXPECT_FALSE(ts1 > ts4);
}

TEST_F(PtpTimestampTest, Serialization) {
    PtpTimestamp original(0x123456789ABC, 0xDEF01234);
    
    // Serialize
    std::vector<std::byte> serialized = original.serialize();
    EXPECT_EQ(serialized.size(), 10);  // 6 bytes seconds + 4 bytes nanoseconds
    
    // Verify bytes (assuming big-endian network format)
    EXPECT_EQ(static_cast<uint8_t>(serialized[0]), 0x12);
    EXPECT_EQ(static_cast<uint8_t>(serialized[1]), 0x34);
    EXPECT_EQ(static_cast<uint8_t>(serialized[2]), 0x56);
    EXPECT_EQ(static_cast<uint8_t>(serialized[3]), 0x78);
    EXPECT_EQ(static_cast<uint8_t>(serialized[4]), 0x9A);
    EXPECT_EQ(static_cast<uint8_t>(serialized[5]), 0xBC);
    EXPECT_EQ(static_cast<uint8_t>(serialized[6]), 0xDE);
    EXPECT_EQ(static_cast<uint8_t>(serialized[7]), 0xF0);
    EXPECT_EQ(static_cast<uint8_t>(serialized[8]), 0x12);
    EXPECT_EQ(static_cast<uint8_t>(serialized[9]), 0x34);
    
    // Deserialize
    PtpTimestamp deserialized = PtpTimestamp::deserialize(serialized);
    
    // Verify roundtrip
    EXPECT_EQ(deserialized.getSeconds(), original.getSeconds());
    EXPECT_EQ(deserialized.getNanoseconds(), original.getNanoseconds());
}
```

These examples demonstrate the versatility and critical importance of the `PtpTimestamp` class in a PTP implementation. From basic manipulation to complex protocol handling, the timestamp class serves as a fundamental building block that enables precise time synchronization across networked devices. 

The examples cover the full spectrum of `PtpTimestamp` usage: creation, arithmetic, comparison, serialization, hardware integration, and application within the PTP protocol state machine and message handling system. This comprehensive set of examples should provide a clear understanding of how to effectively use the `PtpTimestamp` class in a real-world PTP implementation following modern C++17 practices.