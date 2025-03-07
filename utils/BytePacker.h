//
// Created by Admin on 2025/1/7.
//

#ifndef UNTITLED13_BYTEPACKER_H
#define UNTITLED13_BYTEPACKER_H

#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
/*
 * 以下是 CBytePacker 和 CByteUnpacker 的简要设计，这两个类通常用于序列化和反序列化操作，
 * 即将复杂的数据结构转换为字节流（序列化），或者从字节流恢复成数据结构（反序列化）。
 * */
class CBytePacker
{
private:
    std::vector<uint8_t> buffer;

public:
    // Append integer to buffer
    void PackInt(int32_t value) {
        uint8_t* data = reinterpret_cast<uint8_t*>(&value);
        buffer.insert(buffer.end(), data, data + sizeof(int32_t));
    }

    // Append float to buffer
    void PackFloat(float value) {
        uint8_t* data = reinterpret_cast<uint8_t*>(&value);
        buffer.insert(buffer.end(), data, data + sizeof(float));
    }

    // Append string to buffer
    void PackString(const std::string& value) {
        PackInt(static_cast<int32_t>(value.size())); // Store string size
        buffer.insert(buffer.end(), value.begin(), value.end());
    }

    // Get packed data
    const std::vector<uint8_t>& GetBuffer() const {
        return buffer;
    }

    // Clear buffer
    void Clear() {
        buffer.clear();
    }
};

class CByteUnpacker
{
private:
    const std::vector<uint8_t>& buffer;
    size_t offset = 0; // Current read position

public:
    explicit CByteUnpacker(const std::vector<uint8_t>& data) : buffer(data) {}

    // Unpack integer from buffer
    int32_t UnpackInt() {
        if (offset + sizeof(int32_t) > buffer.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        int32_t value;
        std::memcpy(&value, &buffer[offset], sizeof(int32_t));
        offset += sizeof(int32_t);
        return value;
    }

    // Unpack float from buffer
    float UnpackFloat() {
        if (offset + sizeof(float) > buffer.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        float value;
        std::memcpy(&value, &buffer[offset], sizeof(float));
        offset += sizeof(float);
        return value;
    }

    // Unpack string from buffer
    std::string UnpackString() {
        int32_t size = UnpackInt(); // Read string size
        if (offset + size > buffer.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        std::string value(reinterpret_cast<const char*>(&buffer[offset]), size);
        offset += size;
        return value;
    }

    // Reset unpacker
    void Reset() {
        offset = 0;
    }
};

#endif //UNTITLED13_BYTEPACKER_H
