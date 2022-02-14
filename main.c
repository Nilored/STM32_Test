#include "stm32f4xx.h"

void RCC_INI(){
	RCC->CR |= RCC_CR_HSEON;
	while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0) {}



}

int main(void){
	RCC_INI();








}
