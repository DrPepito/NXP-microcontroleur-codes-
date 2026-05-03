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

// Interruptions
extern volatile uint32_t *const SCB_VTOR;
extern volatile uint32_t *const NVIC_ISER0;
extern volatile uint32_t *const P0_ICR23;   // config declenchement P0_23 (SW2)
extern volatile uint32_t *const P0_ICR6;    // config declenchement P0_6  (SW3)
extern volatile uint32_t *const P0_ISFR0;   // flags interruption GPIO0

#define P0_INT 17

typedef void (*pointer_to_ISR)(void);
#define VTAB_SIZE 172
extern pointer_to_ISR vtab[VTAB_SIZE];

			//////////////DECLARATIONS DES FONCTIONS/////////////////////

void delay(uint32_t ms);

// Initialisation
void init_leds(void);
void init_switch(void);

// Commande LEDs — allumer
void set_ledr(void);
void set_ledg(void);
void set_ledb(void);

// Commande LEDs — eteindre
void clr_ledr(void);
void clr_ledg(void);
void clr_ledb(void);

// Commande LEDs — inverser (TP5)
void tog_ledr(void);
void tog_ledg(void);
void tog_ledb(void);

// Lecture broches LEDs (TP5 ex6 — lit PDOR pas PDIR)
int rd_ledr(void);  // retourne 1 si allumee, 0 si eteinte
int rd_ledb(void);  // retourne 1 si allumee, 0 si eteinte

// Lecture boutons
int rd_sw2(void);
int rd_sw3(void);

// Interruptions
void setup_vtab_RAM(void);
void init_interrupt_sw2(void *isr_fn);    // front descendant SW2
void init_interrupt_sw3(void *isr_fn);    // front descendant SW3
void set_irq_sw2_falling(void);           // reconfigure SW2 sur front descendant
void set_irq_sw2_rising(void);            // reconfigure SW2 sur front montant (TP5 ex2)
void enable_irq_sw2(void);               // active IRQ SW2 dans NVIC
void enable_irq_sw3(void);               // active IRQ SW3 dans NVIC
void disable_irq_sw2(void);              // desactive IRQ SW2 dans NVIC (TP5 ex6)
void disable_irq_sw3(void);              // desactive IRQ SW3 dans NVIC (TP5 ex6)

#endif // FRDM_MCXN947_H
