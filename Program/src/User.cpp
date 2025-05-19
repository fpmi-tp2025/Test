#include "User.h"
#include <memory>
#include <iostream>

std::vector<std::string> User::decodeAvaliableCars(const void* blobPtr, int blobSize)
{
    std::vector<std::string> cars;
    
    // Check that the blob is valid and contains at least an int64_t for count.
    if (!blobPtr || blobSize < static_cast<int>(sizeof(int64_t))) {
        std::cerr << "Invalid or incomplete blob data." << std::endl;
        return cars;
    }
    
    // Use a char pointer for pointer arithmetic.
    const char* buffer = static_cast<const char*>(blobPtr);
    size_t offset = 0;
    
    // Read the number of texts.
    int64_t count = 0;
    std::memcpy(&count, buffer + offset, sizeof(int64_t));
    offset += sizeof(int64_t);
    
    // Loop over each text.
    for (int64_t i = 0; i < count; ++i) {
        // Ensure there's enough data for length.
        if (offset + sizeof(int64_t) > static_cast<size_t>(blobSize)) {
            std::cerr << "Incomplete length information for car index " << i << std::endl;
            break;
        }
        
        int64_t len = 0;
        std::memcpy(&len, buffer + offset, sizeof(int64_t));
        offset += sizeof(int64_t);
        
        // Validate that the blob contains the expected text bytes.
        if (offset + len > static_cast<size_t>(blobSize)) {
            std::cerr << "Incomplete data for car index " << i << std::endl;
            break;
        }
        
        // Create a string from the blob and add it to our vector.
        cars.emplace_back(buffer + offset, static_cast<size_t>(len));
        offset += len;
    }
    
    return cars;
}

std::unique_ptr<char*> User::encodeAvaliableCars(const std::vector<std::string> &avaliableCars)
{
    // Data to store: an array of texts representing available cars.
    const int numCars = avaliableCars.size();

    // First, calculate the total size required.
    // We'll need space for:
    // - The count (int64_t)
    // - For each string: its length stored as int64_t and the text bytes themselves.
    size_t totalSize = sizeof(int64_t); // For the count
    for (int i = 0; i < numCars; ++i) {
        totalSize += sizeof(int64_t);      // For the length of the string
        totalSize += avaliableCars[i].size(); // For the content of the string
    }

    // Allocate a dynamic array (buffer) to hold the serialized data.
    char* buffer = new char[totalSize];

    // Serialization: write the count first.
    int64_t count = numCars;
    std::memcpy(buffer, &count, sizeof(int64_t));

    // Keep track of the current offset while filling the buffer.
    size_t offset = sizeof(int64_t);

    // Next, for each string, write its length and then its content.
    for (int i = 0; i < numCars; ++i) {
        int64_t len = static_cast<int64_t>(avaliableCars[i].size());
        std::memcpy(buffer + offset, &len, sizeof(int64_t));
        offset += sizeof(int64_t);
        std::memcpy(buffer + offset, avaliableCars[i].data(), avaliableCars[i].size());
        offset += avaliableCars[i].size();
    }

    return std::make_unique<char*>(buffer);
}
