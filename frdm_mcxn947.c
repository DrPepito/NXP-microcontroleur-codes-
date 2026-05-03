#include "frdm_mcxn947.h"

			//////////////DEFINITIONS POINTEURS REGISTRES/////////////////////

volatile uint32_t *const AHBCLKCTRL0 = (uint32_t*)(SYSCON0 + 0x200);

volatile uint32_t *const P0_PDOR = (uint32_t*)(P0_BASE + 0x40);
volatile uint32_t *const P0_PDIR = (uint32_t*)(P0_BASE + 0x50);
volatile uint32_t *const P0_PDDR = (uint32_t*)(P0_BASE + 0x54);

volatile uint32_t *const P1_PDOR = (uint32_t*)(P1_BASE + 0x40);
volatile uint32_t *const P1_PDIR = (uint32_t*)(P1_BASE + 0x50);
volatile uint32_t *const P1_PDDR = (uint32_t*)(P1_BASE + 0x54);

volatile uint32_t *const PORT0_PCR10 = (uint32_t*)(PORT0_BASE + 0x80 + 4*10);
volatile uint32_t *const PORT0_PCR23 = (uint32_t*)(PORT0_BASE + 0x80 + 4*23);
volatile uint32_t *const PORT0_PCR27 = (uint32_t*)(PORT0_BASE + 0x80 + 4*27);
volatile uint32_t *const PORT0_PCR6  = (uint32_t*)(PORT0_BASE + 0x80 + 4*6);

volatile uint32_t *const PORT1_PCR2  = (uint32_t*)(PORT1_BASE + 0x80 + 4*2);

volatile uint32_t *const SCB_VTOR   = (uint32_t*) 0xE000ED08;
volatile uint32_t *const NVIC_ISER0 = (uint32_t*) 0xE000E100;
volatile uint32_t *const NVIC_ICER0 = (uint32_t*) 0xE000E180;  // Interrupt Clear Enable
volatile uint32_t *const P0_ICR23   = (uint32_t*)(P0_BASE + 0x80 + 4*23);
volatile uint32_t *const P0_ICR6    = (uint32_t*)(P0_BASE + 0x80 + 4*6);
volatile uint32_t *const P0_ISFR0   = (uint32_t*)(P0_BASE + 0x120);

pointer_to_ISR vtab[VTAB_SIZE];

			//////////////DEFINITIONS DES FONCTIONS/////////////////////

void delay(uint32_t ms) {
    for (int i = 0; i < ms; i++)
        asm(" NOP");
}

void init_leds(void) {
    *AHBCLKCTRL0 |= 1 << 20;  // horloge GPIO1
    *AHBCLKCTRL0 |= 1 << 14;  // horloge PORT1

    set_bit(*P0_PDOR, LEDR);
    set_bit(*P0_PDDR, LEDR);
    *PORT0_PCR10 = 0x1000;

    set_bit(*P0_PDOR, LEDG);
    set_bit(*P0_PDDR, LEDG);
    *PORT0_PCR27 = 0x1000;

    set_bit(*P1_PDOR, LEDB);
    set_bit(*P1_PDDR, LEDB);
    *PORT1_PCR2  = 0x1000;
}

void init_switch(void) {
    *AHBCLKCTRL0 |= 1 << 19;  // horloge GPIO0
    *AHBCLKCTRL0 |= 1 << 13;  // horloge PORT0

    clr_bit(*P0_PDDR, SW2);
    *PORT0_PCR23 = 0x1000;

    clr_bit(*P0_PDDR, SW3);
    *PORT0_PCR6  = 0x1000;
}

// LEDs — logique active-bas (0=allume, 1=eteint)
void set_ledr(void) { clr_bit(*P0_PDOR, LEDR); }
void set_ledg(void) { clr_bit(*P0_PDOR, LEDG); }
void set_ledb(void) { clr_bit(*P1_PDOR, LEDB); }

void clr_ledr(void) { set_bit(*P0_PDOR, LEDR); }
void clr_ledg(void) { set_bit(*P0_PDOR, LEDG); }
void clr_ledb(void) { set_bit(*P1_PDOR, LEDB); }

// TP5 ex1 — inverser l'etat d'une LED
void tog_ledr(void) { tog_bit(*P0_PDOR, LEDR); }
void tog_ledg(void) { tog_bit(*P0_PDOR, LEDG); }
void tog_ledb(void) { tog_bit(*P1_PDOR, LEDB); }

// TP5 ex6 — lire l'etat d'une LED depuis PDOR (logique inversee)
// retourne 1 si allumee (bit PDOR=0), 0 si eteinte (bit PDOR=1)
int rd_ledr(void) { return !rd_bit(*P0_PDOR, LEDR); }
int rd_ledb(void) { return !rd_bit(*P1_PDOR, LEDB); }

// Boutons — retourne 1 si appuye (logique active-bas)
int rd_sw2(void) { return !rd_bit(*P0_PDIR, SW2); }
int rd_sw3(void) { return !rd_bit(*P0_PDIR, SW3); }

// ── Interruptions ────────────────────────────────────────────────────────

void setup_vtab_RAM(void) {
    uintptr_t *v = (uintptr_t*) *SCB_VTOR;
    for (unsigned int i = 0; i < VTAB_SIZE; i++)
        vtab[i] = (pointer_to_ISR) v[i];
    *SCB_VTOR = (uint32_t) vtab;
}

// configure ICR pour front descendant : IRQC bits 19:16 = 1010
static void cfg_falling(volatile uint32_t *ICR) {
    set_bit(*ICR, 19);
    clr_bit(*ICR, 18);
    set_bit(*ICR, 17);
    clr_bit(*ICR, 16);
}

// configure ICR pour front montant : IRQC bits 19:16 = 1001
static void cfg_rising(volatile uint32_t *ICR) {
    set_bit(*ICR, 19);
    clr_bit(*ICR, 18);
    clr_bit(*ICR, 17);
    set_bit(*ICR, 16);
}

// configure ICR pour fronts montant ET descendant : IRQC bits 19:16 = 1011
static void cfg_both(volatile uint32_t *ICR) {
    set_bit(*ICR, 19);
    clr_bit(*ICR, 18);
    set_bit(*ICR, 17);
    set_bit(*ICR, 16);
}

void init_interrupt_sw2(void *isr_fn) {
    cfg_falling(P0_ICR23);
    vtab[16 + P0_INT] = (pointer_to_ISR) isr_fn;
    set_bit(*NVIC_ISER0, P0_INT);
}

void init_interrupt_sw3(void *isr_fn) {
    cfg_falling(P0_ICR6);
    vtab[16 + P0_INT] = (pointer_to_ISR) isr_fn;
    set_bit(*NVIC_ISER0, P0_INT);
}

// TP5 ex2 — changer la polarite de l'interruption SW2
void set_irq_sw2_falling(void) { cfg_falling(P0_ICR23); }
void set_irq_sw2_rising(void)  { cfg_rising(P0_ICR23);  }
void set_irq_sw2_both(void)    { cfg_both(P0_ICR23);    }

// TP5 ex6 — activer/desactiver IRQ dans le NVIC
void enable_irq_sw2(void)  { set_bit(*NVIC_ISER0, P0_INT); }
void enable_irq_sw3(void)  { set_bit(*NVIC_ISER0, P0_INT); }
void disable_irq_sw2(void) { set_bit(*NVIC_ICER0, P0_INT); }
void disable_irq_sw3(void) { set_bit(*NVIC_ICER0, P0_INT); }
