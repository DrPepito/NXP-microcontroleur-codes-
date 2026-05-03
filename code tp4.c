#include "frdm_mcxn947.h"

			//////////////DÉFINITIONS DES POINTEURS REGISTRES/////////////////////

// Horloge
volatile uint32_t *const AHBCLKCTRL0 = (uint32_t*)(SYSCON0 + 0x200);

// GPIO0
volatile uint32_t *const P0_PDOR = (uint32_t*)(P0_BASE + 0x40);  // Port Data Output
volatile uint32_t *const P0_PDIR = (uint32_t*)(P0_BASE + 0x50);  // Port Data Input
volatile uint32_t *const P0_PDDR = (uint32_t*)(P0_BASE + 0x54);  // Port Data Direction

// GPIO1
volatile uint32_t *const P1_PDOR = (uint32_t*)(P1_BASE + 0x40);  // Port Data Output
volatile uint32_t *const P1_PDIR = (uint32_t*)(P1_BASE + 0x50);  // Port Data Input
volatile uint32_t *const P1_PDDR = (uint32_t*)(P1_BASE + 0x54);  // Port Data Direction

// PORT0 — Pin Control Registers
volatile uint32_t *const PORT0_PCR10 = (uint32_t*)(PORT0_BASE + 0x80 + 4*10);
volatile uint32_t *const PORT0_PCR23 = (uint32_t*)(PORT0_BASE + 0x80 + 4*23);
volatile uint32_t *const PORT0_PCR27 = (uint32_t*)(PORT0_BASE + 0x80 + 4*27);
volatile uint32_t *const PORT0_PCR6  = (uint32_t*)(PORT0_BASE + 0x80 + 4*6);

// PORT1 — Pin Control Registers
volatile uint32_t *const PORT1_PCR2  = (uint32_t*)(PORT1_BASE + 0x80 + 4*2);

			//////////////DÉFINITIONS DES FONCTIONS/////////////////////

// Délai logiciel
void delay(uint32_t ms) {
    for (int i = 0; i < ms; i++) {
        asm(" NOP");
    }
}

// Initialisation des LEDs
void init_leds(void) {
    // activation horloges GPIO1 et PORT1 (GPIO0/PORT0 activés dans init_switch)
    *AHBCLKCTRL0 |= 1 << 20;  // horloge GPIO1
    *AHBCLKCTRL0 |= 1 << 14;  // horloge PORT1

    // LED rouge — P0_10
    set_bit(*P0_PDOR, LEDR);   // eteindre avant de passer en sortie
    set_bit(*P0_PDDR, LEDR);   // ligne en sortie
    *PORT0_PCR10 = 0x1000;     // mux GPIO

    // LED verte — P0_27
    set_bit(*P0_PDOR, LEDG);   // eteindre
    set_bit(*P0_PDDR, LEDG);   // ligne en sortie
    *PORT0_PCR27 = 0x1000;     // mux GPIO

    // LED bleue — P1_2
    set_bit(*P1_PDOR, LEDB);   // eteindre
    set_bit(*P1_PDDR, LEDB);   // ligne en sortie
    *PORT1_PCR2  = 0x1000;     // mux GPIO
}

// Initialisation des boutons
void init_switch(void) {
    // activation horloges GPIO0 et PORT0
    *AHBCLKCTRL0 |= 1 << 19;  // horloge GPIO0
    *AHBCLKCTRL0 |= 1 << 13;  // horloge PORT0

    // SW2 — P0_23
    clr_bit(*P0_PDDR, SW2);    // ligne en entree
    *PORT0_PCR23 = 0x1000;     // mux GPIO

    // SW3 — P0_6
    clr_bit(*P0_PDDR, SW3);    // ligne en entree
    *PORT0_PCR6  = 0x1000;     // mux GPIO
}

// Allumage LEDs (logique active-bas : 0 = allumé)
void set_ledr(void) { clr_bit(*P0_PDOR, LEDR); }  // allumer rouge
void set_ledg(void) { clr_bit(*P0_PDOR, LEDG); }  // allumer verte
void set_ledb(void) { clr_bit(*P1_PDOR, LEDB); }  // allumer bleue

// Extinction LEDs (logique active-bas : 1 = éteint)
void clr_ledr(void) { set_bit(*P0_PDOR, LEDR); }  // eteindre rouge
void clr_ledg(void) { set_bit(*P0_PDOR, LEDG); }  // eteindre verte
void clr_ledb(void) { set_bit(*P1_PDOR, LEDB); }  // eteindre bleue

// Lecture boutons (retourne 1 si appuyé, 0 si relâché)
// logique active-bas : 0 sur PDIR = appuyé
int rd_sw2(void) { return !rd_bit(*P0_PDIR, SW2); }
int rd_sw3(void) { return !rd_bit(*P0_PDIR, SW3); }
