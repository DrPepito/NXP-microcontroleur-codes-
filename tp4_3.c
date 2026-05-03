/* Clignotement leds - Demarrage/arret par appui sur SW3
 * SW3 par interruptions
 * tp4_3.c — pour les plus rapides
 */

#include "frdm_mcxn947.h"

// Variable partagee entre ISR et main
volatile uint8_t P0_SW3 = 0;

// ISR — SW3 est sur le meme GPIO0 que SW2, on teste le bit 6 dans ISFR0
void isr_P0_sw3(void) {
    if (rd_bit(*P0_ISFR0, SW3)) {
        set_bit(*P0_ISFR0, SW3);  // effacer le flag
        P0_SW3 = 1;               // signaler l'appui
    }
}

int main() {
    init_switch();
    init_leds();

    __asm(" CPSID I");
    setup_vtab_RAM();
    init_interrupt_sw3(isr_P0_sw3);  // SW3 cette fois
    __asm(" CPSIE I");

    uint8_t state = 0;

    while (P0_SW3 == 0) {};  // attendre le premier appui sur SW3

    while (1) {
        if (!state && !P0_SW3)
            asm(" WFI");

        if (P0_SW3) {
            P0_SW3 = 0;
            state = !state;
        }

        if (state) {
            set_ledr();
            delay(1000000);
            clr_ledr();

            set_ledg();
            delay(1000000);
            clr_ledg();

            set_ledb();
            delay(1000000);
            clr_ledb();
        }
    }
    return 0;
}
