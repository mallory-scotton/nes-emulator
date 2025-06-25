///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Utils.hpp"
#include <algorithm>
#include <atomic>
#include <type_traits>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class RingBuffer
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Assertions
    ///////////////////////////////////////////////////////////////////////////
    static_assert(
        std::is_trivially_destructible<T>::value,
        "Expecting a simple type in the ring buffer"
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    // Members
    ///////////////////////////////////////////////////////////////////////////
    const size_t m_size;            //<! Capacity of the ring buffer
    std::atomic<size_t> m_write;    //<! Write index in the ring buffer
    std::atomic<size_t> m_read;     //<! Read index in the ring buffer
    std::vector<T> m_buffer;        //<! Underlying storage for the ring buffer

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for the RingBuffer class
    ///
    /// \param capacity The maximum number of elements the ring buffer can hold
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit RingBuffer(size_t capacity)
        : m_size(capacity)
        , m_write(0)
        , m_read(0)
        , m_buffer(capacity)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copy constructor for the RingBuffer class
    ///
    ///////////////////////////////////////////////////////////////////////////
    RingBuffer(const RingBuffer&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor for the RingBuffer class
    ///
    ///////////////////////////////////////////////////////////////////////////
    RingBuffer& operator=(const RingBuffer&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the current size of the ring buffer
    ///
    /// \param index The current index to check
    /// \param size The size of the ring buffer
    ///
    /// \return The current size of the ring buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    static size_t GetNextIndex(size_t index, size_t size)
    {
        size_t result = index + 1;

        while (result >= size)
        {
            result -= size;
        }
        return (result);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pushes a value into the ring buffer
    ///
    /// \param value The value to be pushed into the ring buffer
    ///
    /// \return True if the value was successfully pushed, false if the buffer
    /// is full
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Push(const T& value)
    {
        const size_t index = m_write.load(std::memory_order_relaxed);
        const size_t next = GetNextIndex(index, m_size);

        if (next == m_read.load(std::memory_order_acquire))
        {
            // Buffer is full, cannot push
            return (false);
        }

        // Store the value in the buffer and update the write index
        m_buffer[index] = value;
        m_write.store(next, std::memory_order_release);
        return (true);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pops a value from the ring buffer
    ///
    /// \param output The output parameter to store the popped value
    /// \param count The number of elements to pop from the buffer
    ///
    /// \return True if the value was successfully popped, false if the buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t Pop(T* output, size_t count)
    {
        const size_t write = m_write.load(std::memory_order_acquire);
        const size_t read = m_read.load(std::memory_order_relaxed);

        // Calculate the number of available elements in the buffer
        size_t available = 0;
        if (write >= read)
        {
            available = write - read;
        }
        else
        {
            available = write + m_size - read;
        }

        if (available == 0)
        {
            // Buffer is empty, cannot pop
            return (0);
        }

        // Limit the count to the number of available elements
        count = std::min(count, available);

        // Calculate the number of elements to read
        size_t newRead = read + count;

        if (read + count >= m_size)
        {
            // Copy data in two sections
            const size_t count0 = m_size - read;
            const size_t count1 = count - count0;

            std::copy(
                m_buffer.begin() + read,
                m_buffer.end(),
                output
            );
            std::copy(
                m_buffer.begin(),
                m_buffer.begin() + count1,
                output + count0
            );

            newRead -= m_size;
        }
        else
        {
            // Copy data in a single section
            std::copy(
                m_buffer.begin() + read,
                m_buffer.begin() + (newRead + count),
                output
            );

            if (newRead >= m_size)
            {
                newRead = 0;
            }
        }

        // Update the read index
        m_read.store(newRead, std::memory_order_release);
        return (count);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resets the ring buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset()
    {
        m_write.store(0, std::memory_order_relaxed);
        m_read.store(0, std::memory_order_relaxed);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the ring buffer is empty
    ///
    /// \return True if the ring buffer is empty, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsEmpty(void)
    {
        return (GetSize() == 0);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the current size of the ring buffer
    ///
    /// \return The current size of the ring buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t GetSize(void)
    {
        const size_t write = m_write.load(std::memory_order_acquire);
        const size_t read = m_read.load(std::memory_order_relaxed);

        // Calculate the number of available elements in the buffer
        size_t available = 0;
        if (write >= read)
        {
            available = write - read;
        }
        else
        {
            available = write + m_size - read;
        }

        return (available);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Returns the capacity of the ring buffer
    ///
    /// \return The capacity of the ring buffer
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t GetCapacity(void) const
    {
        return (m_size);
    }
};

} // !namespace NES::Audio
