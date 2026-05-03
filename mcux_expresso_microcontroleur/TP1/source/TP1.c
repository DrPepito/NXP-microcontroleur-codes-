// Clignotement led rouge : bit 10 du port GPIO 0
/* TP1_1.c
 * Y. Blanchard -  17/12/2024
 */
#include <stdint.h>

//adresse de base des registres de controle du microcontroleur
#define SYSCON0 0x40000000
//Registre permettant l'activation des horloges de certains modules
volatile uint32_t *const AHBCLKCTRL0 = (uint32_t *) ( SYSCON0 + 0x200);

//Adresse de base du GPIO port P0
#define P0_BASE 0x40096000
//Port Data Output Port 0
volatile uint32_t *const P0_PDOR = (uint32_t*) (P0_BASE + 0x40);
//Port Data Direction Port 0
volatile uint32_t *const P0_PDDR = (uint32_t*) (P0_BASE + 0x54);

//Adresse Port Control Port 0
#define PORT0_BASE 0x40116000
//Pin Control bit 10 du port 0
volatile uint32_t *const PORT0_PCR10 = (uint32_t*) (PORT0_BASE + 0xA8);

#define LEDR 10 //Indice ligne LEDR
const uint32_t LEDR_mask = 1 << LEDR; //Masque LEDR

int main() {
	*AHBCLKCTRL0 |= 1 << 13; // Activation horloge sur PORT0
	*AHBCLKCTRL0 |= 1 << 19; // Activation horloge sur GPIO0

	//Initialisation du PORT0, ligne 10
	*PORT0_PCR10 = 0b000000000000000;
	// initialisation valeur de sortie Ã  1 - extinction led
	*P0_PDOR |= LEDR_mask;
	//Mise en sortie de la ligne
	*P0_PDDR |= LEDR_mask;

	while (1) {
		//Ecriture 0 : allumage de la led
		*P0_PDOR &= ~LEDR_mask;
		for (int i = 0; i < 1600000; i++) {
		}
		//Ecriture 1 : extinction led
		*P0_PDOR |= LEDR_mask;
		for (int i = 0; i < 1600000; i++) {
		}
	}
	return 0;
}
