//We will practice some fundamental concepts of C++ 17 with respect to quantitative finance.

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <random>
#include <exception>
#include <algorithm>

// Market data for different financial instruments
class MarketData {
public:
    virtual ~MarketData() = default;
    virtual void process() = 0;
};

class BondMarketData : public MarketData {
    double bondPrice;
public:
    BondMarketData(double price) : bondPrice(price) {}
    void process() override {
        std::cout << "Processing Bond Market Data: Price = " << bondPrice << std::endl;
        // Simulate some processing logic
    }
};

class InterestRateMarketData : public MarketData {
    double interestRate;
public:
    InterestRateMarketData(double rate) : interestRate(rate) {}
    void process() override {
        std::cout << "Processing Interest Rate Market Data: Rate = " << interestRate << std::endl;
        // Simulate some processing logic
    }
};

// A data processor that uses templates and smart pointers
template<typename T>
class DataProcessor {
    std::vector<std::unique_ptr<T>> marketDataList;
public:
    void addData(std::unique_ptr<T> data) {
        marketDataList.push_back(std::move(data));
    }

    void processAll() {
        for (auto& data : marketDataList) {
            try {
                data->process();
            } catch (const std::exception& e) {
                std::cerr << "Exception while processing market data: " << e.what() << std::endl;
            }
        }
    }
};

// Simulate incoming market data and process it in a thread-safe manner
void simulateMarketData(DataProcessor<MarketData>& processor, std::mutex& mtx) {
    // Step 1: Set up a random number generator to simulate market data values.
    std::random_device rd; // Non-deterministic random number generator
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 100.0); // Distribution between 1.0 and 100.0

    // Step 2: Create market data objects with random values.
    auto bondData = std::make_unique<BondMarketData>(dis(gen)); // Create a unique_ptr to BondMarketData
    auto interestRateData = std::make_unique<InterestRateMarketData>(dis(gen)); // Create a unique_ptr to InterestRateMarketData

    // Step 3: Safely add the created market data to the processor.
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex to prevent data race
        processor.addData(std::move(bondData)); // Move the bond data into the processor's list
        processor.addData(std::move(interestRateData)); // Move the interest rate data into the processor's list
    } // The lock_guard goes out of scope here, releasing the mutex automatically.
}


int main() {
    DataProcessor<MarketData> processor;
    std::mutex mtx;

    // Launch multiple threads to simulate incoming market data
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(simulateMarketData, std::ref(processor), std::ref(mtx));
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Process all market data
    processor.processAll();

    return 0;
}
