/* Clignotement leds - Demarrage/arret par appui sur SW2
 * SW2 par interruptions — version separee materiel/algorithme
 * tp4_2.c
 */

#include "frdm_mcxn947.h"

// Variable partagee entre ISR et main
// volatile : interdit au compilateur de mettre en cache (indispensable a -O1)
volatile uint8_t P0_SW2 = 0;

// ISR — specifique a l'application, reste dans tp4_2.c (pas dans la librairie)
void isr_P0(void) {
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);  // effacer le flag (ecrire 1 pour remettre a 0)
        P0_SW2 = 1;               // signaler l'appui au main
    }
}

int main() {
    // initialisation du materiel via la librairie
    init_switch();  // SW2, SW3 — active aussi horloge GPIO0/PORT0
    init_leds();    // LEDR, LEDG, LEDB

    // configuration de la table des vecteurs en RAM
    __asm(" CPSID I");             // desactiver les interruptions pendant la reconfiguration
    setup_vtab_RAM();              // copier Flash -> RAM, faire pointer VTOR sur RAM
    init_interrupt_sw2(isr_P0);   // configurer front descendant SW2 + enregistrer isr_P0
    __asm(" CPSIE I");             // reactiver les interruptions

    uint8_t state = 0;

    while (P0_SW2 == 0) {};  // attendre le premier appui sur SW2

    while (1) {
        // si arrete et pas de nouveau flag : dormir jusqu'a la prochaine interruption
        if (!state && !P0_SW2)
            asm(" WFI");

        // traiter le flag d'appui
        if (P0_SW2) {
            P0_SW2 = 0;       // acquitter
            state = !state;   // basculer marche/arret
        }

        // clignotement rouge -> verte -> bleue
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
