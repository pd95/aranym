/* 2001 MJ */

/*
 *  cpu_emulation.h - Definitions for Basilisk II CPU emulation module (UAE 0.8.10 version)
 *
 *  Basilisk II (C) 1997-2001 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

/*
 *  Memory system
 */

#include "sysdeps.h"
#include "memory.h"

// RAM and ROM pointers (allocated and set by main_*.cpp)
// This part will be moved to another include, only mains use it.
extern uint32 RAMBase;		// RAM base (Atari address space), does not include Low Mem when != 0
extern uint8 *RAMBaseHost;	// RAM base (host address space)
extern uint32 RAMSize;		// Size of RAM
extern uint32 ROMBase;		// ROM base (Atari address space)
extern uint8 *ROMBaseHost;	// ROM base (host address space)
extern uint32 ROMSize;		// Size of ROM
extern uint32 RealROMSize;	// Real size of ROM
extern uint32 FastRAMBase;	// Fast-RAM base (Atari address space)
extern uint8 *FastRAMBaseHost;	// Fast-RAM base (host address space)
extern uint32 VideoRAMBase;	// VideoRAM base (Atari address space)
extern uint8 *VideoRAMBaseHost;	// VideoRAM base (host address space)


// Atari memory access functions
// Direct access to CPU address space
// For HW operations
// Read/WriteAtariIntXX
//
static inline uint32 ReadAtariInt32(uint32 addr) {return get_long_direct(addr);}
static inline uint32 ReadAtariInt16(uint32 addr) {return get_word_direct(addr);}
static inline uint32 ReadAtariInt8(uint32 addr) {return get_byte_direct(addr);}
static inline void WriteAtariInt32(uint32 addr, uint32 l) {put_long_direct(addr, l);}
static inline void WriteAtariInt16(uint32 addr, uint32 w) {put_word_direct(addr, w);}
static inline void WriteAtariInt8(uint32 addr, uint32 b) {put_byte_direct(addr, b);}

// Direct access to allocated memory
// Ignores HW checks, so that be carefull
// Read/WriteHWMemIntXX
// 
static inline uint32 ReadHWMemInt32(uint32 addr) {return do_get_mem_long((uae_u32 *)get_real_address_direct(addr));}
static inline uint32 ReadHWMemInt16(uint32 addr) {return do_get_mem_word((uae_u16 *)get_real_address_direct(addr));}
static inline uint32 ReadHWMemInt8(uint32 addr) {return do_get_mem_byte((uae_u8 *)get_real_address_direct(addr));}
static inline void WriteHWMemInt32(uint32 addr, uint32 l) {do_put_mem_long((uae_u32 *)get_real_address_direct(addr), l);}
static inline void WriteHWMemInt16(uint32 addr, uint32 w) {do_put_mem_word((uae_u16 *)get_real_address_direct(addr), w);}
static inline void WriteHWMemInt8(uint32 addr, uint32 b) {do_put_mem_byte((uae_u8 *)get_real_address_direct(addr), b);}

// Indirect access to CPU address space
// Uses MMU if available
// For SW operations
// Only data space
// Read/WriteIntXX
// 
static inline uint32 ReadInt32(uint32 addr) {return get_long(addr, true);}
static inline uint32 ReadInt16(uint32 addr) {return get_word(addr, true);}
static inline uint32 ReadInt8(uint32 addr) {return get_byte(addr, true);}
static inline void WriteInt32(uint32 addr, uint32 l) {put_long(addr, l);}
static inline void WriteInt16(uint32 addr, uint32 w) {put_word(addr, w);}
static inline void WriteInt8(uint32 addr, uint32 b) {put_byte(addr, b);}


// These functions will be removed
static inline uint8 *Atari2HostAddr(uint32 addr) {return get_real_address_direct(addr);}
//static inline uint32 Host2AtariAddr(uint8 *addr) {return get_virtual_address(addr);}


// These functions will be removed
static inline void *Atari_memset(uint32 addr, int c, size_t n) {return memset(Atari2HostAddr(addr), c, n);}
static inline void *Atari2Host_memcpy(void *dest, uint32 src, size_t n) {return memcpy(dest, Atari2HostAddr(src), n);}
static inline void *Host2Atari_memcpy(uint32 dest, const void *src, size_t n) {return memcpy(Atari2HostAddr(dest), src, n);}
static inline void *Atari2Atari_memcpy(uint32 dest, uint32 src, size_t n) {return memcpy(Atari2HostAddr(dest), Atari2HostAddr(src), n);}

/*
 *  680x0 emulation
 */

// Initialization
extern bool InitMEM();
extern bool Init680x0(void);
extern void Exit680x0(void);

// 680x0 emulation functions
struct M68kRegisters;
extern void Start680x0(void);	// Reset and start 680x0

// These functions will be removed
extern "C" void Execute68k(uint32 addr, M68kRegisters *r);	// Execute 68k code from EMUL_OP routine
extern "C" void Execute68kTrap(uint16 trap, M68kRegisters *r);	// Execute Atari 68k trap from EMUL_OP routine

// Interrupt functions
// extern void TriggerInterrupt(void);	// Trigger interrupt level 1 (InterruptFlag must be set first)
// These functions will be removed
extern void TriggerVBL(void);		// Trigger interrupt level 4
extern void TriggerMFP(bool);		// Trigger interrupt level 6
extern void TriggerNMI(void);		// Trigger interrupt level 7


// This function will be removed
static uaecptr showPC(void) { return regs.pcp; }	// for debugging only

#endif
