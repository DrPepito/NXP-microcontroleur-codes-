/* LEDR par SW2 si LEDB eteinte, LEDB par SW3 si LEDR eteinte
 * Exclusion mutuelle — sans variable d'etat LED, sans lire PDIR des LEDs
 * Les interruptions inutiles sont desactivees
 * tp5_6.c
 */

#include "frdm_mcxn947.h"

volatile uint8_t P0_SW2 = 0;
volatile uint8_t P0_SW3 = 0;

void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);
        P0_SW2 = 1;
    }
    if (rd_bit(*P0_ISFR0, SW3)) {
        set_bit(*P0_ISFR0, SW3);
        P0_SW3 = 1;
    }
}

int main() {
    init_leds();
    init_switch();
    setup_vtab_RAM();

    // les deux interruptions actives au depart
    init_interrupt_sw2(isr_P0);
    init_interrupt_sw3(isr_P0);

    while (1) {
        if (!P0_SW2 && !P0_SW3)
            asm(" WFI");

        if (P0_SW2) {
            P0_SW2 = 0;

            // changer LEDR uniquement si LEDB eteinte
            // rd_ledb() lit PDOR (pas PDIR) : pas de lecture d'etat bouton ni variable LED
            if (!rd_ledb()) {
                tog_ledr();

                // si LEDR vient de s'allumer : desactiver SW3 (inutile car LEDR allumee)
                // si LEDR vient de s'eteindre : reactiver SW3
                if (rd_ledr())
                    disable_irq_sw3();
                else
                    enable_irq_sw3();
            }
        }

        if (P0_SW3) {
            P0_SW3 = 0;

            // changer LEDB uniquement si LEDR eteinte
            if (!rd_ledr()) {
                tog_ledb();

                // si LEDB vient de s'allumer : desactiver SW2
                // si LEDB vient de s'eteindre : reactiver SW2
                if (rd_ledb())
                    disable_irq_sw2();
                else
                    enable_irq_sw2();
            }
        }
    }
    return 0;
}
