#include <stdio.h>
#include "LPC8xx.h"
#include "mrt.h"

#define LED_LOCATION 2
#define GND_LOCATION 3
#define ISP_LOCATION 1

#define LED_ON() (LPC_GPIO_PORT->SET0 = 1 << LED_LOCATION)
#define LED_OFF() (LPC_GPIO_PORT->CLR0 = 1 << LED_LOCATION)

#define ISPBTN() (!(LPC_GPIO_PORT->PIN0 & (1 << ISP_LOCATION)))

// deep power down
void deepPowerDown(void) {
//	if (LPC_PMU->PCON & 0b100)
//		return 1; // error
	LPC_PMU->PCON = 3;
	SCB->SCR |= 0b100;
	asm("wfi");
}
void deepPowerDownWithWakuUp(int msec, int usewakeuppin) {
	LPC_PMU->DPDCTRL = usewakeuppin ? 0b1100 : 0b1110;
	LPC_PMU->PCON = 3;
	SCB->SCR |= 0b100;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 9); // enable WKT
	LPC_WKT->CTRL = 0b111;
	LPC_WKT->COUNT = msec * 10;
	asm("wfi");
}

void init() {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);  // enable SWM
//	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 18);  // enabel UART
	
	// pin config
	LPC_SWM->PINASSIGN0 = 0xffff0004UL; // U0_TXD, U0_RXD
	LPC_SWM->PINENABLE0 = 0xffffffbfUL;
	
	LPC_SYSCON->SYSAHBCLKCTRL |=  (1 << 6); // enable GPIO
	LPC_SYSCON->PRESETCTRL &= ~(1 << 10); // GPIO reset
	LPC_SYSCON->PRESETCTRL |=  (1 << 10); //
	
	LPC_GPIO_PORT->DIR0 |= 1 << GND_LOCATION;
	LPC_GPIO_PORT->CLR0 = 1 << GND_LOCATION;
	
	LPC_GPIO_PORT->DIR0 |= 1 << LED_LOCATION;
	LPC_GPIO_PORT->CLR0 = 1 << LED_LOCATION;
	
	mrtInit(__SYSTEM_CLOCK / 1000); // multi-rate timer for 1ms
}
int main() {
	init();
	mrtDelay(100);
	deepPowerDownWithWakuUp(60 * 1000, 0); // wake up after 30sec, disable wake up pin
}
