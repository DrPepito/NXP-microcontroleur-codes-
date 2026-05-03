/* Allumage/extinction LEDR apres NB_APP appuis successifs sur SW2
 * tp5_4.c
 */

#include "frdm_mcxn947.h"

#define NB_APP 3  // nombre d'appuis necessaires pour basculer la LEDR

volatile uint8_t P0_SW2 = 0;

void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);
        P0_SW2 = 1;
    }
}

int main() {
    init_leds();
    init_switch();
    setup_vtab_RAM();
    init_interrupt_sw2(isr_P0);

    uint8_t compteur = 0;  // compte les appuis

    while (1) {
        if (!P0_SW2)
            asm(" WFI");

        if (P0_SW2) {
            P0_SW2 = 0;
            compteur++;

            if (compteur >= NB_APP) {
                compteur = 0;  // reinitialiser le compteur
                tog_ledr();    // basculer LEDR apres NB_APP appuis
            }
        }
    }
    return 0;
}
