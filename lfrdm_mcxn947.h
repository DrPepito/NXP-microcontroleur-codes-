#ifndef FRDM_MCXN947_H
#define FRDM_MCXN947_H

#include <stdint.h>

			//////////////REGISTRES — adresses/////////////////////

// Systeme de controle
#define SYSCON0     0x40000000

// GPIO Port 0
#define P0_BASE     0x40096000

// GPIO Port 1
#define P1_BASE     0x40098000

// Port Control Port 0
#define PORT0_BASE  0x40116000

// Port Control Port 1
#define PORT1_BASE  0x40117000

			//////////////MACROS bit/////////////////////

#define clr_bit(X,Y) (X &= ~((uintmax_t)1 << (Y)))  // met le bit Y de X à 0
#define set_bit(X,Y) (X |=  ((uintmax_t)1 << (Y)))  // met le bit Y de X à 1
#define tog_bit(X,Y) (X ^=  ((uintmax_t)1 << (Y)))  // inverse le bit Y de X
#define rd_bit(X,Y)  ((X >> (Y)) & 1)               // lit le bit Y de X (0 ou 1)

			//////////////NUMÉROS DE BROCHES/////////////////////

// LEDs
#define LEDR 10  // P0_10
#define LEDG 27  // P0_27
#define LEDB 2   // P1_2

// Boutons
#define SW2  23  // P0_23
#define SW3  6   // P0_6

			//////////////DÉCLARATIONS DES POINTEURS REGISTRES/////////////////////

// Horloge
extern volatile uint32_t *const AHBCLKCTRL0;

// GPIO0
extern volatile uint32_t *const P0_PDOR;
extern volatile uint32_t *const P0_PDIR;
extern volatile uint32_t *const P0_PDDR;

// GPIO1
extern volatile uint32_t *const P1_PDOR;
extern volatile uint32_t *const P1_PDIR;
extern volatile uint32_t *const P1_PDDR;

// PORT0 — Pin Control Registers
extern volatile uint32_t *const PORT0_PCR10;
extern volatile uint32_t *const PORT0_PCR23;
extern volatile uint32_t *const PORT0_PCR27;
extern volatile uint32_t *const PORT0_PCR6;

// PORT1 — Pin Control Registers
extern volatile uint32_t *const PORT1_PCR2;

			//////////////DÉCLARATIONS DES FONCTIONS/////////////////////

// Délai logiciel
void delay(uint32_t ms);

// Initialisation
void init_leds(void);    // initialisation GPIO/PORT pour les LEDs
void init_switch(void);  // initialisation GPIO/PORT pour les boutons

// Commande LEDs
void set_ledr(void);  // allumage LED rouge
void set_ledg(void);  // allumage LED verte
void set_ledb(void);  // allumage LED bleue
void clr_ledr(void);  // extinction LED rouge
void clr_ledg(void);  // extinction LED verte
void clr_ledb(void);  // extinction LED bleue

// Lecture boutons
int rd_sw2(void);  // lecture SW2 : retourne 1 si appuyé, 0 sinon
int rd_sw3(void);  // lecture SW3 : retourne 1 si appuyé, 0 sinon

#endif // FRDM_MCXN947_H
