/* LEDR allumee uniquement quand SW2 maintenu appuye
 * Option 2 : changement de polarite de l'interruption
 * tp5_2.c
 */

#include "frdm_mcxn947.h"

volatile uint8_t P0_SW2 = 0;

void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);  // effacer le flag
        P0_SW2 = 1;               // signaler l'evenement (appui ou relachement)
    }
}

int main() {
    init_leds();
    init_switch();
    setup_vtab_RAM();

    // depart : interruption sur front descendant (appui)
    init_interrupt_sw2(isr_P0);

    // etat courant : 0 = attend appui, 1 = attend relachement
    uint8_t attend_relachement = 0;

    while (1) {
        if (!P0_SW2)
            asm(" WFI");

        if (P0_SW2) {
            P0_SW2 = 0;

            if (!attend_relachement) {
                // evenement = appui -> allumer LEDR
                set_ledr();
                // changer polarite : prochaine interruption sur front montant (relachement)
                set_irq_sw2_rising();
                attend_relachement = 1;
            } else {
                // evenement = relachement -> eteindre LEDR
                clr_ledr();
                // changer polarite : prochaine interruption sur front descendant (appui)
                set_irq_sw2_falling();
                attend_relachement = 0;
            }
        }
    }
    return 0;
}
