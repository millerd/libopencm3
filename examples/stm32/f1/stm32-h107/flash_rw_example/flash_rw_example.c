/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2013 Damian Miller <damian.m.miller@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/f1/memorymap.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/usart.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/flash.h>

#define ECHO_EN 1
#define FLASH_PAGE_NUM_MAX 127
#define FLASH_PAGE_SIZE 0x800

void init_system(void);
void init_usart(void);

void usart_send_string(u32 usart, u8 *string, u16 str_size);
void usart_get_string(u32 usart, u8 *string, u16 str_max_size);
u8 flash_program_data(u32 start_address, u8 *input_data, u16 num_elements);
void flash_read_data(u32 start_address, u16 num_elements, u8 *out_string);
u16 local_strlen(u8 *string);

int main(void)
{
	u8 result = 0;
	u8 str_send[256], str_verify[256];

	init_system();

	while(1)
	{
		usart_send_string(USART1, (u8*)"Please enter string to write into Flash memory:\n\r", local_strlen((u8*)"Please enter string to write into Flash memory:\n\r"));
		usart_get_string(USART1, str_send, 256);

		result = flash_program_data(0x08030000, str_send, local_strlen(str_send));
		if( result != 0)
		{
			usart_send_string(USART1, (u8*)"Problem with programming flash memory.\r\nErrorCode: ", local_strlen((u8*)"Problem with programming flash memory.\r\nErrorCode: "));
			usart_send(USART1,  0x30|result);
		}
		else
		{
			usart_send_string(USART1, (u8*)"Verification of written data: ", local_strlen((u8*)"Verification of written data: "));
			flash_read_data(0x08030000, 256, str_verify);
			usart_send_string(USART1, str_verify, local_strlen(str_verify));
		}
		usart_send_string(USART1, (u8*)"\r\n", local_strlen((u8*)"\r\n"));
	}
	return 0;
}

void init_system(void)
{
	/* setup SYSCLK to work with 64Mhz HSI */
	rcc_clock_setup_in_hsi_out_64mhz();
	init_usart();
}

void init_usart()
{
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable(USART1);
}

void usart_send_string(u32 usart, u8 *string, u16 str_size)
{
	u16 iter = 0;
	do
	{
		usart_send_blocking(usart, string[iter++]);
	}while(string[iter] != 0 && iter < str_size);
}

void usart_get_string(u32 usart, u8 *out_string, u16 str_max_size)
{
	u8 sign = 0;
	u16 iter = 0;

	while(iter < str_max_size)
	{
		sign = usart_recv_blocking(usart);

#if ECHO_EN == 1
		usart_send_blocking(usart, sign);
#endif

		if(sign != '\n' && sign != '\r')
			out_string[iter++] = sign;
		else
		{
			out_string[iter] = 0;
			usart_send_blocking(usart, '\r');
			usart_send_blocking(usart, '\n');
			break;
		}
	}
}

u8 flash_program_data(u32 start_address, u8 *input_data, u16 num_elements)
{
	u16 iter;
	u32 current_address = start_address;
	u32 page_address = start_address;

	if((start_address - FLASH_BASE) >= (FLASH_PAGE_SIZE * (FLASH_PAGE_NUM_MAX+1)))
	{
		return 1;
	}


	if(start_address % FLASH_PAGE_SIZE)
		page_address -= (start_address % FLASH_PAGE_SIZE);

	flash_unlock();
	flash_erase_page(page_address);

	for(iter=0; iter<num_elements; iter += 4)
	{
		flash_program_word(current_address+iter, *((u32*)(input_data + iter)));
	}

	return 0;
}

void flash_read_data(u32 start_address, u16 num_elements, u8 *out_string)
{
	u16 iter;
	u32 *memory_ptr= (u32*)start_address;

	for(iter=0; iter<num_elements/4; iter++)
	{
		*(u32*)out_string = *(memory_ptr + iter);
		out_string += 4;
	}
}

u16 local_strlen(u8 *string)
{
	u16 iter = 0;

	while(string[iter++] != 0);

	return iter;
}
