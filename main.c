#include "stm32f4xx.h"
#include "SimpleREG.h"

void RCC_INI(){
/*********************************************Set 84MHz Clock *******************************************************/
	RCC->CR |= RCC_CR_HSEON;                                			//Включаем внешний кварц
	while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET) {}				//Ждем готовности
	//FLASH------------------------------------------------------------------------------------------------------
	CLEAR_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);					//Выключить буфер предвыборки
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_3WS);		//Задержка согласно датащиту     //upd. на 2WS не работает, только на 3WS при текущих условиях
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);						//Вкл
	//Делители---------------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);			//AHB Prescaler
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);			//APB1
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);			//APB2
	MODIFY_REG(RCC->CFGR, RCC_CFGR_RTCPRE, 0x190000U);				//HSR_RTC // 25 // 0x190000 = 25 << 16
	//PLL--------------------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 0x19U << RCC_PLLCFGR_PLLM_Pos);  	// /M = 25
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, 0x150U << RCC_PLLCFGR_PLLN_Pos); 	// *N = 336
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, 0x4U << RCC_PLLCFGR_PLLP_Pos); 	// /P = 4
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ, 0x7U << RCC_PLLCFGR_PLLQ_Pos); 	// /Q = 7
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_HSE);		//Тактирование PLL от HSE
	//PLLI2S----------------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SN, 0x180U << RCC_PLLI2SCFGR_PLLI2SN_Pos);	// *N = 384
	MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SR, 0x2U << RCC_PLLI2SCFGR_PLLI2SR_Pos);	// /R = 2
	//----------------------------------------------------------------------------------------------------------
	SET_BIT(RCC->CR, RCC_CR_PLLON);							//Включить PLL
	while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {}				//Дождаться PLL
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);				//Выбор PLL как источника частоты
	while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) {}			//Ожидание выбора
	//----------------------------------------------------------------------------------------------------------
}/********************************************************************************************************************/
void TIMER_INI(){
/*********************************************************************************************************************/

}/********************************************************************************************************************/
void GPIO_pinOut(){
/*********************************************************************************************************************/
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN); 	//Включить тактирование порта C и A
	//----------------------------
	pinMODE(GPIOC, 13, pin_OUTPUT);						//Назначить GPIOC 13  на выход
	//----------------------------
	pinMODE(GPIOA, 0, pin_INPUT);						//Назначить GPIOA 0 на вход
	pinPULL(GPIOA, 0, pin_PULLUP);						//PULLUP на GPIOA 0
	//----------------------------
}/********************************************************************************************************************/

int main(void) {

	RCC_INI();
	GPIO_pinOut();

	WRITE_PIN(GPIOC, 13, HIGH);//Погасить светодиод (подать высокий уровень, управление катодом)

	while (1) {
		if (READ_PIN(GPIOA, 0) == HIGH) {
			WRITE_PIN(GPIOC, 13, HIGH);
		} else {
			WRITE_PIN(GPIOC, 13, LOW);
		}
	}
}
