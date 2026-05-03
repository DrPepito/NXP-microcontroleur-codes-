/* Allumage LEDR par SW2 uniquement si SW3 non appuye au moment de l'appui
 * tp5_5.c
 */

#include "frdm_mcxn947.h"

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

    while (1) {
        if (!P0_SW2)
            asm(" WFI");

        if (P0_SW2) {
            P0_SW2 = 0;

            // condition : SW3 non appuye au moment de l'evenement SW2
            if (!rd_sw3())
                tog_ledr();  // basculer LEDR uniquement si SW3 relache
        }
    }
    return 0;
}
