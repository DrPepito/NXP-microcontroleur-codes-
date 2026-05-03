/* Allumage LEDR - Interruptions
 * Appui SW2 -> inverser LEDR
 * tp5_1.c
 */

#include "frdm_mcxn947.h"

volatile uint8_t P0_SW2 = 0;  // variable partagee ISR <-> main

void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);  // effacer le flag
        P0_SW2 = 1;               // signaler l'evenement
    }
}

int main() {
    init_leds();
    init_switch();
    setup_vtab_RAM();
    init_interrupt_sw2(isr_P0);

    while (1) {
        if (!P0_SW2)
            asm(" WFI");  // dormir jusqu'a la prochaine interruption

        if (P0_SW2) {
            P0_SW2 = 0;
            tog_ledr();   // inverser LEDR
        }
    }
    return 0;
}
