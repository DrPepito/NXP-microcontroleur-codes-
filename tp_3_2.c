/*
 * Systemes electroniques a base de microcontroleurs
 *
 * tp3_2.c — clignotement RGB démarré/arrêté par appui sur SW2
 * Toute la gestion matérielle passe par la librairie frdm_mcxn947
 */

#include "frdm_mcxn947.h"

int main() {

    // initialisation du matériel via la librairie
    init_switch();  // SW2, SW3 — doit être avant init_leds (active GPIO0/PORT0)
    init_leds();    // LEDR, LEDG, LEDB

    uint32_t sw2_old = 0;
    uint32_t sw2     = 0;
    uint8_t  state   = 0;  // 0 = arret, 1 = clignotement

    while (1) {

        sw2 = rd_sw2();  // 1 si appuyé, 0 si relâché

        // détection du front montant (transition relâché → appuyé)
        if (!sw2_old && sw2)
            state = !state;  // basculer marche/arret

        if (state) {
            // clignotement : rouge → verte → bleue
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

        sw2_old = sw2;
    }

    return 0;
}
