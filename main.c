#include "stm32f4xx.h"

#define PinMode(PORT, PIN, MODE) MODIFY_REG((PORT)->MODER, 0x3U << (uint32_t)((PIN)*2U), (MODE) << (uint32_t)((PIN)*2U)) // Ибо удобнее

void RCC_DeINI(){

	SET_BIT(RCC->CR,RCC_CR_HSION);											//Включаем внутренний кварц
	while(READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET) {}						//Ждем готовности                                                                        v                                 v
	MODIFY_REG(RCC->CFGR, RCC_CR_HSITRIM, 0x10U << RCC_CR_HSITRIM_Pos);		//Сбрасываем калибровку внутренней RC частоты  //  0x10U << RCC_CR_HSITRIM_Pos/*3U*/ = 0x80U  // --> Reset value: 0x0000 XX81 where X is undefined
	CLEAR_REG(RCC->CFGR);													//Сброс конфигурации предделителей/умножителей/мультиплексоров
	while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {} 		   		   	//Ждем переключения системного тактирования на RC цепь
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);										//Выключаем умножитель
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {}					//Ждем готовности
	CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);									//Выключаем PLLI2S
	while (READ_BIT(RCC->CR, RCC_CR_PLLI2SRDY) != RESET) {}					//Ждем готовности
	CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);						//Выключаем внешний кварц
	while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {}					//Ждем отключения
	CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);										//Запрет использования внешнего кварца
	CLEAR_REG(RCC->CIR);													//Выключить прерывания
	SET_BIT(RCC->CSR, RCC_CSR_RMVF);										//Сброс прерываний от RCC

}

void RCC_INI(){

	RCC->CR |= RCC_CR_HSEON;                                				//Включаем внешний кварц
	while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET) {}						//Ждем готовности
	//FLASH--------------------------------------------------------------------------------------------------
	CLEAR_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);								//Выключить буфер предвыборки
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_3WS);		//Задержка согласно датащиту     //upd. на 2WS не работает, только на 3WS при текущих условиях
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);									//Вкл
	//Делители-----------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);				//AHB Prescaler
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);				//APB1
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);				//APB2
	MODIFY_REG(RCC->CFGR, RCC_CFGR_RTCPRE, 0x190000U);						//HSR_RTC // 25 // 0x190000 = 25 << 16
	//PLL----------------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 0x19U << RCC_PLLCFGR_PLLM_Pos);  	// /M = 25
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, 0x150U << RCC_PLLCFGR_PLLN_Pos); 	// *N = 336
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, 0x4U << RCC_PLLCFGR_PLLP_Pos); 		// /P = 4
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ, 0x7U << RCC_PLLCFGR_PLLQ_Pos); 		// /Q = 7
	MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC, RCC_PLLCFGR_PLLSRC_HSE);			//Тактирование PLL от HSE
	//PLLI2S------------------------------------------------------------------------------------------------
	MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SN, 0x180U << RCC_PLLI2SCFGR_PLLI2SN_Pos);  	// *N = 384
	MODIFY_REG(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SR, 0x2U << RCC_PLLI2SCFGR_PLLI2SR_Pos);  		// *N = 384
	//------------------------------------------------------------------------------------------------------
	SET_BIT(RCC->CR, RCC_CR_PLLON);											//Включить PLL
	while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {}						//Дождаться PLL
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);					//Выбор PLL как источника частоты
	while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) {}			//Ожидание выбора

}

int main(void){
	RCC_DeINI();
	RCC_INI();

	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN); 			//Включить тактирование порта C и A
	SET_BIT(GPIOC->MODER, 0x1U << (13U*2U));									//Назначить GPIOC 13  на выход
	PinMode(GPIOA, 0U, 0U);														//Назначить GPIOA 0 на вход
	SET_BIT(GPIOA->PUPDR, 0x1U);												//PULLUP на GPIOA 0

	SET_BIT(GPIOC->ODR, 1U << 13);	//Погасить светодиод (подать высокий уровень, управление катодом)

	while(1){
		if((GPIOA->IDR & (1U << 0U)) == 1){
			SET_BIT(GPIOC->ODR, 1U << 13);
		}
		else{
			CLEAR_BIT(GPIOC->ODR, 1U << 13);
		}
	}
}
