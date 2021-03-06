/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2011 Gareth McMullin <gareth@blacksphere.co.nz>
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

#ifndef __USB_FX07_COMMON_H_
#define __USB_FX07_COMMON_H_

void stm32fx07_set_address(usbd_device *usbd_dev, u8 addr);
void stm32fx07_ep_setup(usbd_device *usbd_dev, u8 addr, u8 type, u16 max_size,
			       void (*callback)(usbd_device *usbd_dev, u8 ep));
void stm32fx07_endpoints_reset(usbd_device *usbd_dev);
void stm32fx07_ep_stall_set(usbd_device *usbd_dev, u8 addr, u8 stall);
u8 stm32fx07_ep_stall_get(usbd_device *usbd_dev, u8 addr);
void stm32fx07_ep_nak_set(usbd_device *usbd_dev, u8 addr, u8 nak);
u16 stm32fx07_ep_write_packet(usbd_device *usbd_dev, u8 addr, const void *buf,
			      u16 len);
u16 stm32fx07_ep_read_packet(usbd_device *usbd_dev, u8 addr, void *buf,
			     u16 len);
void stm32fx07_poll(usbd_device *usbd_dev);
void stm32fx07_disconnect(usbd_device *usbd_dev, bool disconnected);


#endif /* __USB_FX07_COMMON_H_ */
