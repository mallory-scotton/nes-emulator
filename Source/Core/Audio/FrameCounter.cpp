///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Audio/FrameCounter.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace NES::Audio
///////////////////////////////////////////////////////////////////////////////
namespace NES::Audio
{

///////////////////////////////////////////////////////////////////////////////
FrameCounter::FrameCounter(std::vector<Slot> slots, IRQHandler& irq)
    : slots(slots)
    , mode(Mode::Seq4Step)
    , counter(0)
    , interruptInhibit(false)
    , irq(irq)
    , frameInterrupt(false)
{}

///////////////////////////////////////////////////////////////////////////////
void FrameCounter::ClearFrameInterrupt(void)
{
    if (frameInterrupt)
    {
        frameInterrupt = false;
        irq.Release();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FrameCounter::Clock(void)
{
    counter++;

    switch (counter)
    {
    case Q1:
    case Q3:
        for (FrameClockable& slot : slots)
        {
            slot.QuarterFrameClock();
        }
        break;
    case Q2:
        for (FrameClockable& slot : slots)
        {
            slot.QuarterFrameClock();
            slot.HalfFrameClock();
        }
        break;
    case Q4:
        if (mode != Mode::Seq4Step)
        {
            break;
        }
        for (FrameClockable& slot : slots)
        {
            slot.QuarterFrameClock();
            slot.HalfFrameClock();
        }
        if (!interruptInhibit)
        {
            irq.Pull();
            frameInterrupt = true;
        }
        break;
    case Q5:
        if (mode != Mode::Seq5Step)
        {
            break;
        }
        for (FrameClockable& slot : slots)
        {
            slot.QuarterFrameClock();
            slot.HalfFrameClock();
        }
        break;
    }

    if ((mode == Mode::Seq4Step && counter == seq4step) || counter == seq5step)
    {
        counter = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FrameCounter::Reset(Mode mode, bool irqInhibit)
{
    this->mode = mode;
    interruptInhibit = irqInhibit;

    if (interruptInhibit)
    {
        ClearFrameInterrupt();
    }
    if (mode == Mode::Seq5Step)
    {
        for (FrameClockable& slot : slots)
        {
            slot.QuarterFrameClock();
            slot.HalfFrameClock();
        }
    }
}

} // !namespace NES::Audio
