#include "Bootloader.h"

#include "../../Gamebuino-Meta.h"
extern Gamebuino gb;


// create our custom NMI handler
void NMI_Handler() {
	gb.bootloader.error(1);
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
	gb.tft.setCursors(0, 0);
	gb.tft.setColor(Color::white, Color::black);
	gb.tft.println("Hard Fault");
	gb.tft.print("r0: 0x");
	gb.tft.println(r0, HEX);
	gb.tft.print("r1: 0x");
	gb.tft.println(r1, HEX);
	gb.tft.print("r2: 0x");
	gb.tft.println(r2, HEX);
	gb.tft.print("r3: 0x");
	gb.tft.println(r3, HEX);
	gb.tft.print("r12: 0x");
	gb.tft.println(r12, HEX);
	gb.tft.print("lr: 0x");
	gb.tft.println(lr, HEX);
	gb.tft.print("pc: 0x");
	gb.tft.println(pc, HEX);
	gb.tft.print("psr: 0x");
	gb.tft.println(psr, HEX);
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
	gb.bootloader.error(2);
}
#endif

namespace Gamebuino_Meta {

uint32_t Bootloader::version() {
	return *(uint32_t*)0x3FFC;
}

void Bootloader::game(const char* filename) {
	noInterrupts(); // fix for bootloader 1.0.0
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

}; //namespace Gamebuino_Meta
