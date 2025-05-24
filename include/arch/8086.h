#ifndef __8086_H__
#define __8086_H__

#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

//Immediate to Register 1 0 1 1 w reg data data if w e 1
#define MOV_Immediate2Regs8086(IsWide, Regs, Data) \
    IsWide ? (0x0b << 20) | (IsWide << 19) | (Regs << 18) | (((Data >> 8) & 0xff) << 8) | Data & 0xff : (0x0b << 12) | (IsWide << 11) | (Regs << 10) | Data

#define MOV_Mem2AX(IsWide, Low, High) \
    (0xa0 << 16) | (IsWide << 16) | (Low << 8) | High

#define PUSH_Regs8086(Regs) (0x0A << 3) | Regs
#define POP_Regs8086(Regs) (0x0B << 3) | Regs

#endif