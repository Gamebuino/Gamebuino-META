/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Sorunome
*/

#include "Bootloader.h"

#include "../../Gamebuino-Meta.h"
namespace Gamebuino_Meta {
extern Gamebuino* gbptr;
}

// create our custom NMI handler
void NMI_Handler() {
	Gamebuino_Meta::gbptr->bootloader.error(1);
}

#if HARDFAULT_DEBUG_HANDLER

#ifdef __cplusplus
extern "C" {
#endif

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress ) {
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];

	/* When the following line is hit, the variables contain the register values. */
	Gamebuino_Meta::gbptr->tft.setCursor(0, 0);
	Gamebuino_Meta::gbptr->tft.setColor(Color::white, Color::black);
	Gamebuino_Meta::gbptr->tft.println("Hard Fault");
	Gamebuino_Meta::gbptr->tft.print("r0: 0x");
	Gamebuino_Meta::gbptr->tft.println(r0, HEX);
	Gamebuino_Meta::gbptr->tft.print("r1: 0x");
	Gamebuino_Meta::gbptr->tft.println(r1, HEX);
	Gamebuino_Meta::gbptr->tft.print("r2: 0x");
	Gamebuino_Meta::gbptr->tft.println(r2, HEX);
	Gamebuino_Meta::gbptr->tft.print("r3: 0x");
	Gamebuino_Meta::gbptr->tft.println(r3, HEX);
	Gamebuino_Meta::gbptr->tft.print("r12: 0x");
	Gamebuino_Meta::gbptr->tft.println(r12, HEX);
	Gamebuino_Meta::gbptr->tft.print("lr: 0x");
	Gamebuino_Meta::gbptr->tft.println(lr, HEX);
	Gamebuino_Meta::gbptr->tft.print("pc: 0x");
	Gamebuino_Meta::gbptr->tft.println(pc, HEX);
	Gamebuino_Meta::gbptr->tft.print("psr: 0x");
	Gamebuino_Meta::gbptr->tft.println(psr, HEX);
	__asm("BKPT #0\n") ; // Break into the debugger
	while(1);
}


void HardFault_Handler( void ) __attribute__( ( naked ) );

void HardFault_Handler(void) {
	__asm( ".syntax unified\n"
		"MOVS R0, #4 \n"
		"MOV R1, LR \n"
		"TST R0, R1 \n"
		"BEQ _MSP \n"
		"MRS R0, PSP \n"
		"B prvGetRegistersFromStack \n"
		"_MSP: \n"
		"MRS R0, MSP \n"
		"B prvGetRegistersFromStack \n"
		".syntax divided\n"
	) ;
}

#ifdef __cplusplus
}
#endif

#else // HARDFAULT_DEBUG_HANDLER
void HardFault_Handler() {
	Gamebuino_Meta::gbptr->bootloader.error(2);
}
#endif

namespace Gamebuino_Meta {

uint32_t Bootloader::version() {
	return *(uint32_t*)0x3FFC;
}

void Bootloader::game(const char* filename) {
	__disable_irq(); // fix for bootloader 1.0.0
	((void(*)(const char*))(*((uint32_t*)0x3FF8)))(filename);
}

void Bootloader::game(char* filename) {
	game((const char*)filename);
}

void Bootloader::loader() {
	((void(*)(void))(*((uint32_t*)0x3FF4)))();
}

void Bootloader::enter() {
	((void(*)(void))(*((uint32_t*)0x3FEC)))();
}

void Bootloader::error(uint16_t e) {
	if (version() <= 0x10001) {
		loader();
	} else {
		((void(*)(uint16_t))(*((uint32_t*)0x3FE8)))(e);
	}
}

void Bootloader::lock() {
	if (version() >= 0x010200) {
		((void(*)(void))(*((uint32_t*)0x3FE4)))();
	}
}

void Bootloader::unlock() {
	if (version() >= 0x010200) {
		((void(*)(void))(*((uint32_t*)0x3FE0)))();
	}
}

void Bootloader::flash_delete(uint32_t addr) {
	if (version() >= 0x010200) {
		((void(*)(uint32_t))(*((uint32_t*)0x3FDC)))(addr);
	}
}

void Bootloader::flash_write(uint32_t size, uint32_t *src_addr, uint32_t *dst_addr) {
	if (version() >= 0x010200) {
		((void(*)(uint32_t, uint32_t*, uint32_t*))(*((uint32_t*)0x3FD8)))(size, src_addr, dst_addr);
	}
}


}; //namespace Gamebuino_Meta
