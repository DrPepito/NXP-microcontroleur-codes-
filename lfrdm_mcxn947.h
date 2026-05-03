#ifndef FRDM_MCXN947_H
#define FRDM_MCXN947_H

#include <stdint.h>

			//////////////REGISTRES — adresses de base/////////////////////

#define SYSCON0     0x40000000
#define P0_BASE     0x40096000
#define P1_BASE     0x40098000
#define PORT0_BASE  0x40116000
#define PORT1_BASE  0x40117000

			//////////////MACROS bit/////////////////////

#define clr_bit(X,Y) (X &= ~((uintmax_t)1 << (Y)))  // met le bit Y de X a 0
#define set_bit(X,Y) (X |=  ((uintmax_t)1 << (Y)))  // met le bit Y de X a 1
#define tog_bit(X,Y) (X ^=  ((uintmax_t)1 << (Y)))  // inverse le bit Y de X
#define rd_bit(X,Y)  ((X >> (Y)) & 1)               // lit le bit Y de X (0 ou 1)

			//////////////NUMEROS DE BROCHES/////////////////////

#define LEDR 10  // P0_10
#define LEDG 27  // P0_27
#define LEDB 2   // P1_2
#define SW2  23  // P0_23
#define SW3  6   // P0_6

			//////////////DECLARATIONS POINTEURS REGISTRES/////////////////////

extern volatile uint32_t *const AHBCLKCTRL0;

// GPIO0
extern volatile uint32_t *const P0_PDOR;
extern volatile uint32_t *const P0_PDIR;
extern volatile uint32_t *const P0_PDDR;

// GPIO1
extern volatile uint32_t *const P1_PDOR;
extern volatile uint32_t *const P1_PDIR;
extern volatile uint32_t *const P1_PDDR;

// PORT0
extern volatile uint32_t *const PORT0_PCR10;
extern volatile uint32_t *const PORT0_PCR23;
extern volatile uint32_t *const PORT0_PCR27;
extern volatile uint32_t *const PORT0_PCR6;

// PORT1
extern volatile uint32_t *const PORT1_PCR2;

// Registres interruption — TP4
extern volatile uint32_t *const SCB_VTOR;    // Cortex-M33 : pointe vers la table des vecteurs
extern volatile uint32_t *const NVIC_ISER0;  // NVIC : active les lignes d'interruption
extern volatile uint32_t *const P0_ICR23;    // GPIO0 : type declenchement P0_23
extern volatile uint32_t *const P0_ICR6;     // GPIO0 : type declenchement P0_6 (SW3)
extern volatile uint32_t *const P0_ISFR0;    // GPIO0 : flags d'interruption

// Numero IRQ GPIO0 dans le NVIC
#define P0_INT 17

// Type pointeur vers fonction ISR
typedef void (*pointer_to_ISR)(void);

// Table des vecteurs en RAM — partagee entre librairie et application
#define VTAB_SIZE 172
extern pointer_to_ISR vtab[VTAB_SIZE];

			//////////////DECLARATIONS DES FONCTIONS/////////////////////

// Delai logiciel
void delay(uint32_t ms);

// Initialisation GPIO / PORT
void init_leds(void);    // LEDs RGB
void init_switch(void);  // Boutons SW2, SW3

// Commande LEDs (logique active-bas)
void set_ledr(void);  // allumer LED rouge
void set_ledg(void);  // allumer LED verte
void set_ledb(void);  // allumer LED bleue
void clr_ledr(void);  // eteindre LED rouge
void clr_ledg(void);  // eteindre LED verte
void clr_ledb(void);  // eteindre LED bleue

// Lecture boutons
int rd_sw2(void);  // retourne 1 si appuye, 0 si relache
int rd_sw3(void);  // retourne 1 si appuye, 0 si relache

// Interruptions — TP4
void setup_vtab_RAM(void);             // copie table Flash -> RAM, configure VTOR
void init_interrupt_sw2(void *isr_fn); // configure front descendant SW2 + enregistre ISR
void init_interrupt_sw3(void *isr_fn); // configure front descendant SW3 + enregistre ISR

#endif // FRDM_MCXN947_H
