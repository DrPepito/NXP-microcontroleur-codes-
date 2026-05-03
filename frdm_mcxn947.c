#include "frdm_mcxn947.h"

			//////////////DEFINITIONS POINTEURS REGISTRES/////////////////////

volatile uint32_t *const AHBCLKCTRL0 = (uint32_t*)(SYSCON0 + 0x200);

// GPIO0
volatile uint32_t *const P0_PDOR = (uint32_t*)(P0_BASE + 0x40);
volatile uint32_t *const P0_PDIR = (uint32_t*)(P0_BASE + 0x50);
volatile uint32_t *const P0_PDDR = (uint32_t*)(P0_BASE + 0x54);

// GPIO1
volatile uint32_t *const P1_PDOR = (uint32_t*)(P1_BASE + 0x40);
volatile uint32_t *const P1_PDIR = (uint32_t*)(P1_BASE + 0x50);
volatile uint32_t *const P1_PDDR = (uint32_t*)(P1_BASE + 0x54);

// PORT0
volatile uint32_t *const PORT0_PCR10 = (uint32_t*)(PORT0_BASE + 0x80 + 4*10);
volatile uint32_t *const PORT0_PCR23 = (uint32_t*)(PORT0_BASE + 0x80 + 4*23);
volatile uint32_t *const PORT0_PCR27 = (uint32_t*)(PORT0_BASE + 0x80 + 4*27);
volatile uint32_t *const PORT0_PCR6  = (uint32_t*)(PORT0_BASE + 0x80 + 4*6);

// PORT1
volatile uint32_t *const PORT1_PCR2  = (uint32_t*)(PORT1_BASE + 0x80 + 4*2);

// Registres interruption — TP4
volatile uint32_t *const SCB_VTOR   = (uint32_t*) 0xE000ED08;
volatile uint32_t *const NVIC_ISER0 = (uint32_t*) 0xE000E100;
volatile uint32_t *const P0_ICR23   = (uint32_t*)(P0_BASE + 0x80 + 4*23);
volatile uint32_t *const P0_ICR6    = (uint32_t*)(P0_BASE + 0x80 + 4*6);
volatile uint32_t *const P0_ISFR0   = (uint32_t*)(P0_BASE + 0x120);

// Table des vecteurs en RAM
pointer_to_ISR vtab[VTAB_SIZE];

			//////////////DEFINITIONS DES FONCTIONS/////////////////////

void delay(uint32_t ms) {
    for (int i = 0; i < ms; i++) {
        asm(" NOP");  // NOP : empeche le compilateur de supprimer la boucle a -O1
    }
}

void init_leds(void) {
    *AHBCLKCTRL0 |= 1 << 20;  // horloge GPIO1
    *AHBCLKCTRL0 |= 1 << 14;  // horloge PORT1

    set_bit(*P0_PDOR, LEDR);   // eteindre avant de passer en sortie
    set_bit(*P0_PDDR, LEDR);   // sortie
    *PORT0_PCR10 = 0x1000;     // mux GPIO

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

    clr_bit(*P0_PDDR, SW2);    // entree
    *PORT0_PCR23 = 0x1000;     // mux GPIO

    clr_bit(*P0_PDDR, SW3);    // entree
    *PORT0_PCR6  = 0x1000;     // mux GPIO
}

// LEDs — logique active-bas : 0 = allume, 1 = eteint
void set_ledr(void) { clr_bit(*P0_PDOR, LEDR); }  // allumer
void set_ledg(void) { clr_bit(*P0_PDOR, LEDG); }
void set_ledb(void) { clr_bit(*P1_PDOR, LEDB); }
void clr_ledr(void) { set_bit(*P0_PDOR, LEDR); }  // eteindre
void clr_ledg(void) { set_bit(*P0_PDOR, LEDG); }
void clr_ledb(void) { set_bit(*P1_PDOR, LEDB); }

// Boutons — retourne 1 si appuye (logique active-bas inversee)
int rd_sw2(void) { return !rd_bit(*P0_PDIR, SW2); }
int rd_sw3(void) { return !rd_bit(*P0_PDIR, SW3); }

// ── setup_vtab_RAM ────────────────────────────────────────────────────────
// Copie la table des vecteurs de la Flash vers le tableau vtab en RAM
// puis fait pointer SCB_VTOR vers vtab
// A appeler avec les interruptions desactivees (CPSID I)
void setup_vtab_RAM(void) {
    uintptr_t *v = (uintptr_t*) *SCB_VTOR;  // adresse table actuelle (Flash)

    // recopie Flash -> RAM
    for (unsigned int i = 0; i < VTAB_SIZE; i++)
        vtab[i] = (pointer_to_ISR) v[i];

    *SCB_VTOR = (uint32_t) vtab;  // VTOR pointe maintenant sur la RAM
}

// ── config_front_descendant ───────────────────────────────────────────────
// Configure les bits IRQC d'un registre ICR pour un front descendant
// IRQC bits 19:16 = 1010
static void config_front_descendant(volatile uint32_t *ICR) {
    set_bit(*ICR, 19);
    clr_bit(*ICR, 18);
    set_bit(*ICR, 17);
    clr_bit(*ICR, 16);
}

// ── init_interrupt_sw2 ────────────────────────────────────────────────────
// Configure l'interruption sur front descendant pour SW2 (P0_23)
// isr_fn : adresse de la fonction ISR de l'application
void init_interrupt_sw2(void *isr_fn) {
    config_front_descendant(P0_ICR23);
    vtab[16 + P0_INT] = (pointer_to_ISR) isr_fn;  // enregistrer ISR dans la table RAM
    set_bit(*NVIC_ISER0, P0_INT);                  // activer IRQ GPIO0 dans le NVIC
}

// ── init_interrupt_sw3 ────────────────────────────────────────────────────
// Configure l'interruption sur front descendant pour SW3 (P0_6)
// meme IRQ que SW2 (GPIO0), ISR doit tester P0_ISFR0
void init_interrupt_sw3(void *isr_fn) {
    config_front_descendant(P0_ICR6);
    vtab[16 + P0_INT] = (pointer_to_ISR) isr_fn;
    set_bit(*NVIC_ISER0, P0_INT);
}
