/* MJ 2001 */

#include "sysdeps.h"
#include "hardware.h"
#include "cpu_emulation.h"
#include "memory.h"
#include "icio.h"
#include "mfp.h"
#include "acia.h"
#include "acsifdc.h"
#include "rtc.h"
#include "blitter.h"
#include "ide.h"
#include "videl.h"
#include "yamaha.h"
#include "dsp.h"
#include "mmu.h"
#include "exceptions.h"
#include "uae_cpu/newcpu.h"	// for regs.pc

BASE_IO fake_io;
MMU mmu;
MFP mfp;
IKBD ikbd;
MIDI midi;
ACSIFDC fdc;
RTC rtc;
IDE ide;
DSP dsp;
BLITTER blitter;
VIDEL videl;
YAMAHA yamaha;

#define BUS_ERROR	longjmp(excep_env, 2)

void update_screen() { videl.update_screen(); }
int getFloppyStats() { return yamaha.getFloppyStat(); }

bool dP = false;

void HWInit (void) {
	// ide.init();
	rtc.init();
}

static const int HW_IDE 	= 0xf00000;
static const int HW_ROM	= 0xfa0000;
static const int HW_MMU 	= 0xff8000;
static const int HW_VIDEO 	= 0xff8200;
static const int HW_FDC	= 0xff8600;
static const int HW_SCSI	= 0xff8700;
static const int HW_YAMAHA	= 0xff8800;
static const int HW_SOUND	= 0xff8900;
static const int HW_DSP	= 0xff8930;
static const int HW_RTC	= 0xff8960;
static const int HW_BLITT	= 0xff89a0;
static const int HW_SCC	= 0xff8c00;
static const int HW_SCU	= 0xff8e00;
static const int HW_PADDLE	= 0xff9200;
static const int HW_VIDEL	= 0xff9800;
static const int HW_DSPH	= 0xffa200;
static const int HW_STMFP	= 0xfffa00;
static const int HW_FPU	= 0xfffa40;
static const int HW_TTMFP	= 0xfffa80;
static const int HW_IKBD	= 0xfffc00;
static const int HW_MIDI	= 0xfffc04;

struct HARDWARE {
	char name[32];
	int	begin;
	int len;
	ICio *handle;
};

HARDWARE ICs[] = {
	{"IDE", 0xf00000, 0x3a, &ide},
	{"Cartridge", 0xfa0000, 0x20000, &fake_io},
	{"Memory Management", 0xff8000, 8, &mmu},
	{"VIDEL", 0xff8200, 0xc4, &videl},
	{"DMA/FDC", 0xff8600, 0x10, &fdc},
	{"DMA/SCSI", 0xff8700, 0x16, &fake_io},
	{"SCSI", 0xff8780, 0x10, &fake_io},
	{"Yamaha", 0xff8800, 4, &yamaha},
	{"Sound", 0xff8900, 0x22, &fake_io},
	// {"MicroWire", 0xff8922, 0x4},
	{"DMA/DSP", 0xff8930, 0x14, &fake_io},
	{"TT RTC", 0xff8960, 4, &rtc},
	{"BLiTTER", 0xff8A00, 0x3e, &blitter},
	// {"DMA/SCC", 0xff8C00, 0x16},
	{"SCC", 0xff8C80, 0x16, &fake_io},
	// {"VME", 0xff8e00, 0x0c},
	{"Paddle", 0xff9200, 0x24, &fake_io},
	{"VIDEL Pallete", 0xff9800, 0x400, &fake_io},
	{"DSP", 0xffa200, 8, &dsp},
	{"STMFP", 0xfffa00, 0x30, &mfp},
	// {"STFPC", 0xfffa40, 8},
	{"IKBD", 0xfffc00, 4, &ikbd},
	{"MIDI", 0xfffc04, 4, &midi}
	// {"RTC", 0xfffc20, 0x20}
};

/*static*/char* debug_print_IO(uaecptr addr) {
	int len = sizeof(ICs) / sizeof(ICs[0]);
	for(int i=0; i<len; i++)
		if (addr >= ICs[i].begin && addr < (ICs[i].begin + ICs[i].len))
			return ICs[i].name;
	return "Unknown";
}

uaecptr showPC() {
	return regs.pcp;
}

uae_u32 handleRead(uaecptr addr) {
	int len = sizeof(ICs) / sizeof(ICs[0]);
	for(int i=0; i<len; i++) {
		if (addr >= ICs[i].begin && addr < (ICs[i].begin + ICs[i].len)) {
			ICio *ptr = ICs[i].handle;
			return ptr->handleRead(addr);
		}
	}
	fprintf(stderr, "HWget_b %x <- %s at %08x\n", addr, debug_print_IO(addr), showPC());
	BUS_ERROR;
}

void handleWrite(uaecptr addr, uae_u8 value) {
	int len = sizeof(ICs) / sizeof(ICs[0]);
	for(int i=0; i<len; i++) {
		if (addr >= ICs[i].begin && addr < (ICs[i].begin + ICs[i].len)) {
			ICio *ptr = ICs[i].handle;
			ptr->handleWrite(addr, value);
			return;
		}
	}
	fprintf(stderr, "HWput_b %x = %d ($%x) <- %s at %08x\n", addr, value, value, debug_print_IO(addr), showPC());
	BUS_ERROR;
}

void MakeMFPIRQ(int no) {
	mfp.IRQ(no);
}

void ikbd_send(int code) {
	ikbd.ikbd_send(code);
}

uae_u32 HWget_l (uaecptr addr) {
//	uae_u32 * const m = (uae_u32 *)do_get_real_address(addr);
//	return do_get_mem_long(m);
	if (dP)
		fprintf(stderr, "HWget_l %x <- %s at %08x\n", addr, debug_print_IO(addr), showPC());
	// return (handleRead(addr) << 24) | (handleRead(addr+1) << 16) | (handleRead(addr+2) << 8) | handleRead(addr+3);
/*
	if (addr >= 0xf00000 && addr < 0xf0003a)
		return ide.read_handler(&ide, addr, 4);
*/
	if (addr == HW_IDE) {
		uae_u16 x = HWget_w(addr);
		uae_u16 y = HWget_w(addr);
		return (x << 16)| y;
	}
	else
		return (HWget_w(addr) << 16) | HWget_w(addr+2);
}

uae_u32 HWget_w (uaecptr addr) {
//	uae_u16 * const m = (uae_u16 *)do_get_real_address(addr);
//	return do_get_mem_word(m);
	if (dP)
		fprintf(stderr, "HWget_w %x <- %s at %08x\n", addr, debug_print_IO(addr), showPC());
	if (addr == HW_IDE)
		return ide.handleReadW(addr);
	else
		return (handleRead(addr) << 8) | handleRead(addr+1);
}

uae_u32 HWget_b (uaecptr addr) {
//	uae_u8 * const m = (uae_u8 *)do_get_real_address(addr);
//	return do_get_mem_byte(m);
	if (dP)
		fprintf(stderr, "HWget_b %x <- %s at %08x\n", addr, debug_print_IO(addr), showPC());
	return handleRead(addr);
}

void HWput_l (uaecptr addr, uae_u32 l) {
//	uae_u32 * const m = (uae_u32 *)do_get_real_address(addr);
//	do_put_mem_long(m, l);
	if (dP)
		fprintf(stderr, "HWput_l %x,%d ($%08x) -> %s at %08x\n", addr, l, l, debug_print_IO(addr), showPC());
	if (addr == HW_IDE) {
		HWput_w(addr, l >> 16);
		HWput_w(addr, l & 0x0000ffff);
	}
	else {
		handleWrite(addr, l >> 24);
		handleWrite(addr+1, l >> 16);
		handleWrite(addr+2, l >> 8);
		handleWrite(addr+3, l);
	}
}

void HWput_w (uaecptr addr, uae_u32 w) {
//	uae_u16 * const m = (uae_u16 *)do_get_real_address(addr);
//	do_put_mem_word(m, w);
	if (dP)
		fprintf(stderr, "HWput_w %x,%d ($%04x) -> %s at %08x\n", addr, w, w, debug_print_IO(addr), showPC());
	if (addr == HW_IDE)
		ide.handleWriteW(addr, w);
	else {
		handleWrite(addr, w >> 8);
		handleWrite(addr+1, w);
	}
}

void HWput_b (uaecptr addr, uae_u32 b) {
//	uae_u8 * const m = (uae_u8 *)do_get_real_address(addr);
//	do_put_mem_byte(m, b);
	unsigned int bb = b & 0x000000ff;
	if (dP)
		fprintf(stderr, "HWput_b %x,%u ($%02x) -> %s at %08x\n", addr, bb, bb, debug_print_IO(addr), showPC());
	handleWrite(addr, b);
}
