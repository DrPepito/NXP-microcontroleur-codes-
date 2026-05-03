/* Clignotement leds - Demarrage/arret par appui sur SW2
 * SW2 par interruptions
 * tp4_1.c — version corrigée
 * Y. Blanchard / corrigé TP4
 */

#include "frdm_mcxn947.h"

// Table des vecteurs d'interruption en SRAM
typedef void (*pointer_to_ISR)(void);
pointer_to_ISR vtab[172];

volatile uint32_t *const SCB_VTOR  = (uint32_t*) 0xE000ED08;
volatile uint32_t *const NVIC_ISER0 = (uint32_t*) 0xE000E100;

#define P0_INT 17  // numero d'interruption port P0
volatile uint32_t *const P0_ICR23 = (uint32_t*)(P0_BASE + 0x80 + 4*23);
volatile uint32_t *const P0_ISFR0 = (uint32_t*)(P0_BASE + 0x120);

// CORRECTION 1 : P0_SW2 doit etre volatile
// sans volatile, avec -O1 le compilateur met la variable en cache registre
// et ne voit jamais la mise a jour faite par l'ISR -> boucle infinie
volatile uint8_t P0_SW2 = 0;  // variable partagee entre ISR et main

void isr_P0() {  // routine d'interruption
    if (rd_bit(*P0_ISFR0, SW2)) {
        set_bit(*P0_ISFR0, SW2);  // effacement flag interruption (ecrire 1 pour effacer)
        P0_SW2 = 1;               // signaler l'appui au main
    }
}

void delay(uint32_t ms) {
    for (int i = 0; i < ms; i++) {
        asm(" NOP");  // CORRECTION 2 : NOP necessaire sinon le compilateur supprime la boucle vide a -O1
    }
}

int main() {
    init_leds();
    init_switch();

    uintptr_t *v;
    v = (uintptr_t*) *SCB_VTOR;  // adresse de la table d'interruption actuelle (en Flash)

    // CORRECTION 3 : la ligne fautive etait :
    //   *P0v = (uintptr_t)isr_P0;
    // Elle ecrit dans la Flash (table en Flash = memoire morte) -> erreur materielle HardFault
    // Il faut d'abord recopier la table en RAM, puis modifier la RAM
    // La ligne fautive est donc simplement supprimee ici

    __asm(" CPSID I");  // desactiver les interruptions pendant la reconfiguration

    // recopie de la table initialisee par le firmware de Flash vers RAM
    for (unsigned int i = 0; i < sizeof(vtab)/sizeof(vtab[0]); i++)
        vtab[i] = (pointer_to_ISR) v[i];

    *SCB_VTOR = (uint32_t) vtab;  // faire pointer VTOR vers la table en RAM

    // enregistrement de l'ISR dans la table RAM (maintenant possible car RAM = modifiable)
    vtab[16 + P0_INT] = isr_P0;

    // configuration interruption sur front descendant sur P0_23
    // bits 19:16 du registre ICR23 : 1010 = front descendant
    set_bit(*P0_ICR23, 19);
    clr_bit(*P0_ICR23, 18);
    set_bit(*P0_ICR23, 17);
    clr_bit(*P0_ICR23, 16);

    set_bit(*NVIC_ISER0, P0_INT);  // activer la ligne d'interruption GPIO0 dans le NVIC

    __asm(" CPSIE I");  // activer les interruptions

    uint8_t state = 0;

    while (P0_SW2 == 0) {};  // attendre le premier appui sur SW2

    while (1) {
        if (!state && !P0_SW2)
            asm(" WFI");  // mise en veille jusqu'a la prochaine interruption

        if (P0_SW2) {
            P0_SW2 = 0;       // acquitter le flag
            state = !state;   // basculer marche/arret
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
