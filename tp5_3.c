/* Allumage LEDR par SW2, extinction par SW3
 * Une seule ISR pour GPIO0, deux variables d'echange
 * tp5_3.c
 */

#include "frdm_mcxn947.h"

volatile uint8_t P0_SW2 = 0;  // evenement appui SW2
volatile uint8_t P0_SW3 = 0;  // evenement appui SW3

// une seule ISR pour GPIO0 — traite SW2 (bit23) ET SW3 (bit6)
void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);  // effacer flag SW2
        P0_SW2 = 1;
    }
    if (rd_bit(*P0_ISFR0, SW3)) {
        set_bit(*P0_ISFR0, SW3);  // effacer flag SW3
        P0_SW3 = 1;
    }
}

int main() {
    init_leds();
    init_switch();
    setup_vtab_RAM();

    // les deux boutons partagent la meme IRQ GPIO0
    // init_interrupt_sw2 enregistre l'ISR et active l'IRQ
    init_interrupt_sw2(isr_P0);
    // init_interrupt_sw3 configure juste le declenchement sur SW3
    // (l'IRQ GPIO0 et l'ISR sont deja configures par init_interrupt_sw2)
    init_interrupt_sw3(isr_P0);

    while (1) {
        if (!P0_SW2 && !P0_SW3)
            asm(" WFI");

        if (P0_SW2) {
            P0_SW2 = 0;
            set_ledr();   // allumer LEDR
        }

        if (P0_SW3) {
            P0_SW3 = 0;
            clr_ledr();   // eteindre LEDR
        }
    }
    return 0;
}
