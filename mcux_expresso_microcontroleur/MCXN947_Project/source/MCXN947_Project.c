/*
 * Systemes electroniques a base de microcontroleurs
 * TP 2 - tp2_1.c
 * Allumage led rouge quand SW2 appuye
 * 12/03/2026
 */

#include <stdint.h>

#define SYSCON0 0x40000000



volatile uint32_t* const AHBCLKCTRL0 = (uint32_t*)( SYSCON0 + 0x200 ); //Clock enable of various modules


#define P0_BASE 0x40096000
volatile uint32_t* const P0_PDOR = (uint32_t *)(P0_BASE+0x40);
volatile uint32_t* const P0_PDIR = (uint32_t*) (P0_BASE + 0x50);
volatile uint32_t* const P0_PDDR = (uint32_t *)(P0_BASE+0x54);


#define PORT0_BASE 0x40116000
volatile uint32_t *const PORT0_PCR06 = (uint32_t*) (PORT0_BASE + 0x98);
volatile uint32_t *const PORT0_PCR10 = (uint32_t*) (PORT0_BASE + 0xA8);
volatile uint32_t *const PORT0_PCR23 = (uint32_t*) (PORT0_BASE + 0x80 +4*23);
volatile uint32_t *const PORT0_PCR27 = (uint32_t*) (PORT0_BASE + 0x80 +4*27);






// leds
#define LEDR 10 //Indice ligne LEDR
const uint32_t LEDR_mask = 1 << LEDR; //Masque LEDR


#define LEDG 27
const uint32_t LEDG_mask =1<< LEDG; //masque led verte



// boutons :
#define SW2 23
const uint32_t SW2_mask = 1 << SW2;

#define SW3 24
const uint32_t SW3_mask =1<< SW3;



// but allumer led rouge puis led verte en focntion de si SW2 ou sw3 est appuyé :





// MAIN CODE


int main() {
	*AHBCLKCTRL0 |= 1 << 19; // Activation horloge sur GPIO0
	*AHBCLKCTRL0 |= 1 << 13; // Activation horloge sur PORT0

	*PORT0_PCR10 = 0b0001000000000000; // Configuration PORT ligne 10

	*PORT0_PCR23 = 0x1000; // COnfiguration PORT ligne 23

	*P0_PDOR |= LEDR_mask ;  // Led rouge : eteindre

	*P0_PDDR |= LEDR_mask ;  //Ligne en sortie

	*P0_PDDR &= ~SW2_mask; // SW2, ligne en entree (ce qui est le defaut mais il faut etre explicite)
	while (1) {
		if (*P0_PDIR & SW2_mask) //Lecture et test de la valeur de SW2
			*P0_PDOR |= LEDR_mask ;  //Eteindre
		else
			*P0_PDOR &= ~LEDR_mask ;  //Allumer
	}
	return 0;
}
