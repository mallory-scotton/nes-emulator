///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#define MINIAUDIO_IMPLEMENTATION
#include "Core/Audio/Player.hpp"
#include <chrono>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
Player::Player(int inputRate)
    : inputSampleRate(inputRate)
    , queue(4 * inputRate * (std::chrono::milliseconds(120).count() / 100))
    , m_data({queue, &m_resampler, {}, false, 1})
    , m_initialized(false)
{}

///////////////////////////////////////////////////////////////////////////////
Player::~Player()
{
    if (m_initialized)
    {
        ma_device_uninit(&m_device);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Player::Start(void)
{
    m_config = ma_device_config_init(ma_device_type_playback);
    m_config.playback.format = ma_format_f32;
    m_config.playback.channels = 1;
    m_config.sampleRate = outputSampleRate;
    m_config.dataCallback = DataCallback;
    m_config.pUserData = &m_data;
    m_config.periodSizeInMilliseconds = std::chrono::milliseconds(120).count();

    if (ma_device_init(nullptr, &m_config, &m_device) != MA_SUCCESS)
    {
        return (false);
    }

    if (ma_device_start(&m_device) != MA_SUCCESS)
    {
        ma_device_uninit(&m_device);
        return (false);
    }

    ma_resampler_config config = ma_resampler_config_init(
        ma_format_f32,
        m_config.playback.channels,
        inputSampleRate,
        outputSampleRate,
        ma_resample_algorithm_linear
    );

    if (ma_resampler_init(&config, nullptr, &m_resampler) != MA_SUCCESS)
    {
        ma_device_uninit(&m_device);
        return (false);
    }

    m_initialized = true;
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
void Player::Mute(void)
{
    m_data.mute = true;
}

///////////////////////////////////////////////////////////////////////////////
void Player::DataCallback(
    ma_device* device,
    void* output,
    const void*,
    Uint32 outputFrameCount
)
{
    if (device->pUserData == nullptr)
    {
        return;
    }

    CallbackData& data = *static_cast<CallbackData*>(device->pUserData);

    if (data.mute || data.remainingRounds-- > 0)
    {
        return;
    }

    ma_uint64 presamples = 0;
    ma_result result = ma_resampler_get_required_input_frame_count(
        data.resampler, outputFrameCount, &presamples
    );

    if (result != MA_SUCCESS)
    {
        presamples =
            outputFrameCount *
            data.resampler->sampleRateIn /
            data.resampler->sampleRateOut;
    }

    data.frames.resize(presamples);
    ma_uint64 available = data.buffer.Pop(data.frames.data(), presamples);

    if (available < presamples && available > 0)
    {
        for (auto i = available; i < presamples; i++)
        {
            data.frames[i] = data.frames[available];
        }
    }

    ma_uint64 count = outputFrameCount;

    ma_resampler_process_pcm_frames(
        data.resampler,
        reinterpret_cast<void*>(data.frames.data()),
        &presamples,
        output,
        &count
    );
}

} // !namespace NES::Audio
