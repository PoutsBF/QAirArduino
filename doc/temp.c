/* 
 * Programme de gestion de l'alimentation secourue GSM HVAC PAX
 * Author: Stéphane Lepoutère
 * EUROTUNNEL 2016
 *
 * Created on 12 juillet 2016, 13:34
 * V1.0 - 21/09/2016 - version en prototype
 */

#include "system_def.h"
void system_io(void);           // fonctionnement du système

// Différents modes de fonctionnement
enum 
{
    GM_ETEINT,
    GM_PRECHARGE,
    GM_ATTENTE_BAT,
    GM_DEMARRAGE,
    GM_DEMARRAGE_SECOURU,
    GM_DEMARRAGE_SECOURU_RETOUR,
    GM_NORMAL,
    GM_HYSTOFF,
    GM_HYSTOFF_RETOUR,
    GM_M_E_VEILLE,
    GM_VEILLE,
    GM_FIN
};
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
                         Main application
 */

void main(void) 
{
    // initialize the device
    SYSTEM_Initialize();
      
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    adc_on();
    led_test();
    
    while (1) 
    {
        static uchar mode = GM_ETEINT;
		static uchar mode_anc = GM_FIN;	
		
		if(mode != mode_anc)		// Ne l'effectue qu'une fois au changement de mode
		{
			mode_anc = mode;
			switch (mode)
			{
				case GM_ETEINT:
				{
					cmdIgnition  = OFF;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_OFF;
					cmdShuntPrecharge = OFF;
					chargebat_desactive();
                                        led_etat(LED_D20V, LED_OFF);
                                        led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_PRECHARGE:
				{
					cmdIgnition  = OFF;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_OFF;
					cmdShuntPrecharge = OFF;
					chargebat_desactive();
                                        led_etat(LED_D20V, LED_FLASH_RAPIDE);
                                        led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_ATTENTE_BAT:
				{
					cmdIgnition  = OFF;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_OFF;
					cmdShuntPrecharge = ON;
					chargebat_active();
                    led_etat(LED_D20V, LED_OFF);
                    led_etat(LED_D12V, LED_FLASH_RAPIDE);
				} break;
				case GM_DEMARRAGE:
				{
					cmdIgnition  = ON;
					cmd20V5      = FPF_ON;
					cmd12V       = FPF_ON;
					cmdShuntPrecharge = ON;
					chargebat_active();
                    led_etat(LED_D20V, LED_CLIGN_LENT);
                    led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_DEMARRAGE_SECOURU:
				case GM_DEMARRAGE_SECOURU_RETOUR:
				{
					cmdIgnition  = ON;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_ON;
					cmdShuntPrecharge = OFF;
					chargebat_desactive();
                    led_etat(LED_D20V, LED_CLIGN_LENT);
                    led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_NORMAL:
				{
					cmdIgnition  = ON;
					cmd20V5      = FPF_ON;
					cmd12V       = FPF_ON;
					cmdShuntPrecharge = ON;
					chargebat_active();
                    led_etat(LED_D20V, LED_OFF);
                    led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_HYSTOFF:
				case GM_HYSTOFF_RETOUR:
				{
					cmdIgnition  = ON;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_ON;
					cmdShuntPrecharge = OFF;
					chargebat_desactive();
                    led_etat(LED_D20V, LED_FLASH_LENT);
                    led_etat(LED_D12V, LED_OFF);
				} break;
				case GM_M_E_VEILLE:
				{
					cmdIgnition  = OFF;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_ON;
					cmdShuntPrecharge = OFF;
					chargebat_desactive();
                    led_etat(LED_D20V, LED_OFF);
                    led_etat(LED_D12V, LED_CLIGN_RAPIDE);
				} break;
				case GM_VEILLE:
				{    
					cmdIgnition  = ON;
					cmd20V5      = FPF_OFF;
					cmd12V       = FPF_OFF;
					cmdShuntPrecharge = ON;
					chargebat_desactive();
					led_off();

					C1IF = 0;				// Efface la présence de changement sur l'entrée C1
					C1IE = 1;				// Active l'interruption pour réveiller le µC
					if(!(Etat20V5 == 1))	// Si pas de tension en entrée
						SLEEP();			// passe en veille
                    
					cmdIgnition  = OFF;
					cmdShuntPrecharge = OFF;
					CLRWDT();				// Au réveil, efface le watchdog
					C1IE = 0;				// et désactive l'interruption
                    chargebat_finVeille();  // Signale au chargeur un retour de veille
				} break;
			}
		}
        
        switch (mode)
        {
            case GM_ETEINT:
            {
                if(Etat20V5 == 1)
                {
                    mode = GM_PRECHARGE;
                    horloge1s_lance(TMR_1s_PRECHARGE, TIMER_1s_PRECHARGE);
                }
                else
                {
                    mode = GM_VEILLE;
                }
            } break;
            case GM_PRECHARGE:
            {
                if(Etat20V5 == 0)
                {
                    mode = GM_VEILLE;
                }
                else if(horloge1s_fin(TMR_1s_PRECHARGE))
                {
                    horloge1s_lance(TMR_1s_DEMARRAGE, TIMER_1s_ATTENTE_BAT);
                    mode = GM_ATTENTE_BAT;
                }
            } break;
            case GM_ATTENTE_BAT:
            {
                if(Etat20V5 == 0)
                {
                    mode = GM_VEILLE;
                }
                else if(horloge1s_fin(TMR_1s_DEMARRAGE))
                if(chargebat_mini())
                {
                    mode = GM_DEMARRAGE;
                    horloge1s_lance(TMR_1s_DEMARRAGE, TIMER_1s_ATTENTE_BAT);
                }
            } break;
            case GM_DEMARRAGE:
            {
                if(Etat20V5 == 0)
                {
                    mode = GM_DEMARRAGE_SECOURU;
                }
                else if(horloge1s_fin(TMR_1s_DEMARRAGE))
                {
                    mode = GM_NORMAL;
                }
            } break;
            case GM_DEMARRAGE_SECOURU:
            {
                if(Etat20V5 == 1)
                {
                    mode = GM_DEMARRAGE_SECOURU_RETOUR;
                    horloge1s_lance(TMR_1s_PRECHARGE, TIMER_1s_CHARGE);
                }
                else if(horloge1s_fin(TMR_1s_DEMARRAGE))
                {
                    horloge1s_lance(TMR_1s_MEVEILLE, TIMER_1s_MEVEILLE);
                    mode = GM_M_E_VEILLE;
                }
            } break;
            case GM_DEMARRAGE_SECOURU_RETOUR:
            {
                if(Etat20V5 == 0)
                {
                    mode = GM_DEMARRAGE_SECOURU;
                }
                else if(horloge1s_fin(TMR_1s_PRECHARGE))
                {
                    mode = GM_DEMARRAGE;
                }
            } break;
            case GM_NORMAL:
            {
                if(Etat20V5 == 0)
                {
                    horloge1s_lance(TMR_1s_HYSTOFF, TIMER_1s_HYSTOFF);
                    mode = GM_HYSTOFF;
                }
            } break;
            case GM_HYSTOFF:
            {
                if(Etat20V5 == 1)
                {
                    mode = GM_HYSTOFF_RETOUR;
                    horloge1s_lance(TMR_1s_PRECHARGE, TIMER_1s_CHARGE);
                }
                else if(horloge1s_fin(TMR_1s_HYSTOFF))
                {
                    mode = GM_M_E_VEILLE;
                    horloge1s_lance(TMR_1s_MEVEILLE, TIMER_1s_MEVEILLE);
                }
            } break;
            case GM_HYSTOFF_RETOUR:
            {
                if(Etat20V5 == 0)
                {
                    mode = GM_HYSTOFF;
                }
                else if(horloge1s_fin(TMR_1s_PRECHARGE))
                {
                    mode = GM_NORMAL;
                }
            } break;
            case GM_M_E_VEILLE:
            {
                if(horloge1s_fin(TMR_1s_MEVEILLE))
                {
                    mode = GM_VEILLE;
                }
            } break;
            case GM_VEILLE:
            {    
                if(Etat20V5 == 1)
                {
                    mode = GM_PRECHARGE;
                    horloge1s_lance(TMR_1s_PRECHARGE, TIMER_1s_PRECHARGE);
                }
            } break;
        }
                
        system_io();
        CLRWDT();
    }
}

// fonctionnement du système --------------------------------------------
void system_io(void)
{
    led_traite();
    adc_traite();
    chargebat_traite();
}

/**
 End of File
 */