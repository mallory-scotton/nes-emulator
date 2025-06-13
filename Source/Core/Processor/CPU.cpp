///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Core/Processor/CPU.hpp"
#include "Core/Processor/OpCodes.hpp"
#include <iostream>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////
// Namespace NES
///////////////////////////////////////////////////////////////////////////////
namespace NES
{

///////////////////////////////////////////////////////////////////////////////
CPU::CPU(MainBus& bus)
    : m_pendingNMI(false)
    , m_bus(bus)
    , m_irqPulldowns(0)
{}

///////////////////////////////////////////////////////////////////////////////
void CPU::Step(void)
{
    m_cycles++;

    if (m_skipCycles-- > 1)
    {
        return;
    }

    m_skipCycles = 0;

    if (m_pendingNMI)
    {
        InterruptSequence(InterruptType::NMI);
        m_pendingNMI = false;
        return;
    }
    else if (IsPendingIRQ())
    {
        InterruptSequence(InterruptType::IRQ);
        return;
    }

    Byte opcode = m_bus.Read(r_pc++);
    int cycleLength = OperationCycles[opcode];

    if (cycleLength && (
        ExecuteImplied(opcode) ||
        ExecuteBranch(opcode) ||
        ExecuteType1(opcode) ||
        ExecuteType2(opcode) ||
        ExecuteType0(opcode)
    ))
    {
        m_skipCycles += cycleLength;
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPU::Reset(void)
{
    Reset(ReadAddress(RESET_VECTOR));
}

///////////////////////////////////////////////////////////////////////////////
void CPU::Reset(Address address)
{
    m_skipCycles = 0;
    m_cycles = 0;
    r_a = 0; r_x = 0; r_y = 0;
    f_i = true;
    f_c = false; f_d = false; f_n = false; f_v = false; f_z = false;
    r_pc = address;
    r_sp = 0xFD;
}

///////////////////////////////////////////////////////////////////////////////
Address CPU::GetPC(void)
{
    return (r_pc);
}

///////////////////////////////////////////////////////////////////////////////
void CPU::SkipOAMDMACycles(void)
{
    m_skipCycles += 513;
    m_skipCycles += (m_cycles & 1);
}

///////////////////////////////////////////////////////////////////////////////
void CPU::SkipDMCDMACycles(void)
{
    m_skipCycles += 3;
}

///////////////////////////////////////////////////////////////////////////////
void CPU::NMIInterrupt(void)
{
    m_pendingNMI = true;
}

///////////////////////////////////////////////////////////////////////////////
IRQHandler& CPU::CreateIRQHandler(void)
{
    int bit = 1 << m_irqHandlers.size();
    m_irqHandlers.emplace_back(IRQHandler(bit, *this));
    return (m_irqHandlers.back());
}

///////////////////////////////////////////////////////////////////////////////
void CPU::SetIRQPulldown(int bit, bool state)
{
    int mask = ~(1 << bit);
    m_irqPulldowns = (m_irqPulldowns & mask) | state;
}

///////////////////////////////////////////////////////////////////////////////
void CPU::InterruptSequence(InterruptType type)
{
    if (f_i && type != InterruptType::NMI && type != InterruptType::BRK)
    {
        return;
    }

    if (type == InterruptType::BRK)
    {
        r_pc++;
    }

    PushStack(r_pc >> 8);
    PushStack(r_pc);

    Byte flags =
        f_n << 7 | f_v << 6 | 1 << 5 | (type == InterruptType::BRK) << 4 |
        f_d << 3 | f_i << 2 | f_z << 1 | f_c;

    PushStack(flags);

    f_i = true;

    switch (type)
    {
    case InterruptType::IRQ:
    case InterruptType::BRK:
        r_pc = ReadAddress(IRQ_VECTOR);
        break;
    case InterruptType::NMI:
        r_pc = ReadAddress(NMI_VECTOR);
        break;
    }

    m_skipCycles += 7;
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::ExecuteImplied(Byte opcode)
{
    switch (static_cast<OperationImplied>(opcode))
    {
    case OperationImplied::NOP:
        break;
    case OperationImplied::BRK:
        InterruptSequence(InterruptType::BRK);
        break;
    case OperationImplied::JSR:
        PushStack(static_cast<Byte>((r_pc + 1) >> 8));
        PushStack(static_cast<Byte>((r_pc + 1)));
        r_pc = ReadAddress(r_pc);
        break;
    case OperationImplied::RTS:
        r_pc = PullStack();
        r_pc |= PullStack() << 8;
        r_pc++;
        break;
    case OperationImplied::RTI:
    {
        Byte flags = PullStack();
        f_n = flags & 0x80;
        f_v = flags & 0x40;
        f_d = flags & 0x8;
        f_i = flags & 0x4;
        f_z = flags & 0x2;
        f_c = flags & 0x1;
        r_pc = PullStack();
        r_pc |= PullStack() << 8;
        break;
    }
    case OperationImplied::JMP:
        r_pc = ReadAddress(r_pc);
        break;
    case OperationImplied::JMPI:
    {
        Address location = ReadAddress(r_pc);
        Address page = location & 0xFF00;
        r_pc =
            m_bus.Read(location) |
            m_bus.Read(page | ((location + 1) & 0xff)) << 8;
        break;
    }
    case OperationImplied::PHP:
    {
        Byte flags =
            f_n << 7 | f_v << 6 | 1 << 5 | 1 << 4 |
            f_d << 3 | f_i << 2 | f_z << 1 | f_c;
        PushStack(flags);
        break;
    }
    case OperationImplied::PLP:
    {
        Byte flags = PullStack();
        f_n = flags & 0x80;
        f_v = flags & 0x40;
        f_d = flags & 0x8;
        f_i = flags & 0x4;
        f_z = flags & 0x2;
        f_c = flags & 0x1;
        break;
    }
    case OperationImplied::PHA:
        PushStack(r_a);
        break;
    case OperationImplied::PLA:
        r_a = PullStack();
        SetZN(r_a);
        break;
    case OperationImplied::DEY:
        r_y--;
        SetZN(r_y);
        break;
    case OperationImplied::DEX:
        r_x--;
        SetZN(r_x);
        break;
    case OperationImplied::TAY:
        r_y = r_a;
        SetZN(r_y);
        break;
    case OperationImplied::INY:
        r_y++;
        SetZN(r_y);
        break;
    case OperationImplied::INX:
        r_x++;
        SetZN(r_x);
        break;
    case OperationImplied::CLC:
        f_c = false;
        break;
    case OperationImplied::SEC:
        f_c = true;
        break;
    case OperationImplied::CLI:
        f_i = false;
        break;
    case OperationImplied::SEI:
        f_i = true;
        break;
    case OperationImplied::CLD:
        f_d = false;
        break;
    case OperationImplied::SED:
        f_d = true;
        break;
    case OperationImplied::TYA:
        r_a = r_y;
        SetZN(r_a);
        break;
    case OperationImplied::CLV:
        f_v = false;
        break;
    case OperationImplied::TXA:
        r_a = r_x;
        SetZN(r_a);
        break;
    case OperationImplied::TXS:
        r_sp = r_x;
        break;
    case OperationImplied::TAX:
        r_x = r_a;
        SetZN(r_x);
        break;
    case OperationImplied::TSX:
        r_x = r_sp;
        SetZN(r_x);
        break;
    default:
        return (false);
    }
    return (true);
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::ExecuteBranch(Byte opcode)
{
    if ((opcode & BRANCH_INSTRUCTION_MASK) == BRANCH_INSTRUCTION_MASK_RESULT)
    {
        bool branch = opcode & BRANCH_CONDITION_MASK;

        switch (static_cast<BranchOnFlag>(opcode >> BRANCH_ON_FLAG_SHIFT))
        {
        case BranchOnFlag::NEGATIVE:
            branch = !(branch ^ f_n);
            break;
        case BranchOnFlag::OVERFLOW:
            branch = !(branch ^ f_v);
            break;
        case BranchOnFlag::CARRY:
            branch = !(branch ^ f_c);
            break;
        case BranchOnFlag::ZERO:
            branch = !(branch ^ f_z);
            break;
        default:
            return (false);
        };

        if (branch)
        {
            Int8 offset = m_bus.Read(r_pc++);
            m_skipCycles++;
            Address newPC = static_cast<Address>(r_pc + offset);
            SkipPageCrossCycle(r_pc, newPC);
            r_pc = newPC;
        }
        else
        {
            r_pc++;
        }

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::ExecuteType0(Byte opcode)
{
    if ((opcode & INSTRUCTION_MODE_MASK) == 0x0)
    {
        Address location = 0;
        AddrMode2 mode = static_cast<AddrMode2>(
            (opcode & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT);

        switch (mode)
        {
        case AddrMode2::IMMEDIATE:
            location = r_pc++;
            break;
        case AddrMode2::ZERO_PAGE:
            location = m_bus.Read(r_pc++);
            break;
        case AddrMode2::ABSOLUTE:
            location = ReadAddress(r_pc);
            r_pc += 2;
            break;
        case AddrMode2::INDEXED:
            location = (m_bus.Read(r_pc++) + r_x) & 0xFF;
            break;
        case AddrMode2::ABSOLUTE_INDEXED:
            location = ReadAddress(r_pc);
            r_pc += 2;
            SkipPageCrossCycle(location, location + r_x);
            location += r_x;
            break;
        default:
            return (false);
        }

        Uint16 operand = 0;
        Operation0 operation = static_cast<Operation0>(
            (opcode & OPERATION_MASK) >> OPERATION_SHIFT);

        switch (operation)
        {
        case Operation0::BIT:
            operand = m_bus.Read(location);
            f_z = !(r_a & operand);
            f_v = operand & 0x40;
            f_n = operand & 0x80;
            break;
        case Operation0::STY:
            m_bus.Write(location, r_y);
            break;
        case Operation0::LDY:
            r_y = m_bus.Read(location);
            SetZN(r_y);
            break;
        case Operation0::CPY:
        {
            Uint16 diff = r_y - m_bus.Read(location);
            f_c = !(diff & 0x100);
            SetZN(diff);
            break;
        }
        case Operation0::CPX:
        {
            Uint16 diff = r_x - m_bus.Read(location);
            f_c = !(diff & 0x100);
            SetZN(diff);
            break;
        }
        default:
            return (false);
        }

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::ExecuteType1(Byte opcode)
{
    if ((opcode & INSTRUCTION_MODE_MASK) == 0x1)
    {
        Address location = 0;
        Operation1 operation = static_cast<Operation1>(
            (opcode & OPERATION_MASK) >> OPERATION_SHIFT);
        AddrMode1 mode = static_cast<AddrMode1>(
            (opcode & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT);

        switch (mode)
        {
        case AddrMode1::INDEXED_INDIRECT_X:
        {
            Byte zpAddress = r_x + m_bus.Read(r_pc++);
            location = m_bus.Read(zpAddress & 0xff) |
                m_bus.Read((zpAddress + 1) & 0xff) << 8;
            break;
        }
        case AddrMode1::ZERO_PAGE:
            location = m_bus.Read(r_pc++);
            break;
        case AddrMode1::IMMEDIATE:
            location = r_pc++;
            break;
        case AddrMode1::ABSOLUTE:
            location = ReadAddress(r_pc);
            r_pc += 2;
            break;
        case AddrMode1::INDIRECT_Y:
        {
            Byte zpAddress = m_bus.Read(r_pc++);
            location = m_bus.Read(zpAddress & 0xff) |
                m_bus.Read((zpAddress + 1) & 0xff) << 8;
            if (operation != Operation1::STA)
            {
                SkipPageCrossCycle(location, location + r_y);
            }
            location += r_y;
            break;
        }
        case AddrMode1::INDEXED_X:
            location = (m_bus.Read(r_pc++) + r_x) & 0xFF;
            break;
        case AddrMode1::ABSOLUTE_Y:
            location = ReadAddress(r_pc);
            r_pc += 2;
            if (operation != Operation1::STA)
            {
                SkipPageCrossCycle(location, location + r_y);
            }
            location += r_y;
            break;
        case AddrMode1::ABSOLUTE_X:
            location = ReadAddress(r_pc);
            r_pc += 2;
            if (operation != Operation1::STA)
            {
                SkipPageCrossCycle(location, location + r_x);
            }
            location += r_x;
            break;
        default:
            return (false);
        }

        switch (operation)
        {
        case Operation1::ORA:
            r_a |= m_bus.Read(location);
            SetZN(r_a);
            break;
        case Operation1::AND:
            r_a &= m_bus.Read(location);
            SetZN(r_a);
            break;
        case Operation1::EOR:
            r_a ^= m_bus.Read(location);
            SetZN(r_a);
            break;
        case Operation1::ADC:
        {
            Byte operand = m_bus.Read(location);
            Uint16 sum = r_a + operand + f_c;
            f_c = sum & 0x100;
            f_v = (r_a ^ sum) & (operand ^ sum) & 0x80;
            r_a = static_cast<Byte>(sum);
            SetZN(r_a);
            break;
        }
        case Operation1::STA:
            m_bus.Write(location, r_a);
            break;
        case Operation1::LDA:
            r_a = m_bus.Read(location);
            SetZN(r_a);
            break;
        case Operation1::SBC:
        {
            Uint16 subtrahend = m_bus.Read(location);
            Uint16 diff = r_a - subtrahend - !f_c;
            f_c = !(diff & 0x100);
            f_v = (r_a ^ diff) & (~subtrahend ^ diff) & 0x80;
            r_a = diff;
            SetZN(diff);
            break;
        }
        case Operation1::CMP:
        {
            Uint16 diff = r_a - m_bus.Read(location);
            f_c = !(diff & 0x100);
            SetZN(diff);
            break;
        }
        default:
            return (false);
        }

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::ExecuteType2(Byte opcode)
{
    if ((opcode & INSTRUCTION_MODE_MASK) == 2)
    {
        Address location = 0;
        Operation2 operation = static_cast<Operation2>(
            (opcode & OPERATION_MASK) >> OPERATION_SHIFT);
        AddrMode2 mode = static_cast<AddrMode2>(
            (opcode & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT);

        switch (mode)
        {
        case AddrMode2::IMMEDIATE:
            location = r_pc++;
            break;
        case AddrMode2::ZERO_PAGE:
            location = m_bus.Read(r_pc++);
            break;
        case AddrMode2::ACCUMULATOR:
            break;
        case AddrMode2::ABSOLUTE:
            location = ReadAddress(r_pc);
            r_pc += 2;
            break;
        case AddrMode2::INDEXED:
        {
            location = m_bus.Read(r_pc++);
            Byte index;

            if (operation == Operation2::LDX || operation == Operation2::STX)
            {
                index = r_y;
            }
            else
            {
                index = r_x;
            }

            location = (location + index) & 0xFF;
            break;
        }
        case AddrMode2::ABSOLUTE_INDEXED:
        {
            location = ReadAddress(r_pc);
            r_pc += 2;
            Byte index;

            if (operation == Operation2::LDX || operation == Operation2::STX)
            {
                index = r_y;
            }
            else
            {
                index = r_x;
            }

            SkipPageCrossCycle(location, location + index);
            location += index;
            break;
        }
        default:
            return (false);
        }

        Uint16 operand = 0;

        switch (operation)
        {
        case Operation2::ASL:
        case Operation2::ROL:
        {
            if (mode == AddrMode2::ACCUMULATOR)
            {
                bool prev_c = f_c;
                f_c = r_a & 0x80;
                r_a <<= 1;
                r_a = r_a | (prev_c && (operation == Operation2::ROL));
                SetZN(r_a);
            }
            else
            {
                bool prev_c = f_c;
                operand = m_bus.Read(location);
                f_c = operand & 0x80;
                operand = operand << 1 | (prev_c &&
                    (operation == Operation2::ROL));
                SetZN(operand);
                m_bus.Write(location, operand);
            }
            break;
        }
        case Operation2::LSR:
        case Operation2::ROR:
        {
            if (mode == AddrMode2::ACCUMULATOR)
            {
                bool prev_c = f_c;
                f_c = r_a & 1;
                r_a >>= 1;
                r_a = r_a | (prev_c && (operation == Operation2::ROR)) << 7;
                SetZN(r_a);
            }
            else
            {
                bool prev_c = f_c;
                operand = m_bus.Read(location);
                f_c = operand & 1;
                operand = operand >> 1 | (prev_c &&
                    (operation == Operation2::ROR)) << 7;
                SetZN(operand);
                m_bus.Write(location, operand);
            }
            break;
        }
        case Operation2::STX:
            m_bus.Write(location, r_x);
            break;
        case Operation2::LDX:
            r_x = m_bus.Read(location);
            SetZN(r_x);
            break;
        case Operation2::DEC:
        {
            Byte tmp = m_bus.Read(location) - 1;
            SetZN(tmp);
            m_bus.Write(location, tmp);
            break;
        }
        case Operation2::INC:
        {
            Byte tmp = m_bus.Read(location) + 1;
            SetZN(tmp);
            m_bus.Write(location, tmp);
            break;
        }
        default:
            return (false);
        }

        return (true);
    }
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
Address CPU::ReadAddress(Address address)
{
    return (m_bus.Read(address) | (m_bus.Read(address + 1) << 8));
}

///////////////////////////////////////////////////////////////////////////////
void CPU::PushStack(Byte value)
{
    m_bus.Write(0x100 | r_sp, value);
    r_sp--;
}

///////////////////////////////////////////////////////////////////////////////
Byte CPU::PullStack(void)
{
    return (m_bus.Read(0x100 | ++r_sp));
}

///////////////////////////////////////////////////////////////////////////////
void CPU::SkipPageCrossCycle(Address a, Address b)
{
    if ((a & 0xFF00) != (b & 0xFF00))
    {
        m_skipCycles++;
    }
}

///////////////////////////////////////////////////////////////////////////////
void CPU::SetZN(Byte value)
{
    f_z = !value;
    f_n = value & 0x80;
}

///////////////////////////////////////////////////////////////////////////////
bool CPU::IsPendingIRQ(void) const
{
    return (!f_i && m_irqPulldowns != 0);
}

} // !namespace NES
