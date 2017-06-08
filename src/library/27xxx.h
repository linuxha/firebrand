/*
 * Routines specific to the 27xxx series of chips.
 *
 * Copyright 2005 Paul Millar
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Notes:
 *
 * These routines supply standard support for the 27xxx series of
 * chips. Those starting "27C" are equivalent to the similarly named
 * 27xxx chip, but are manufactured with CMOS technology.
 */

#include "chipinfo.h"

int H_27xxx_write_chip( chip_info_t *ci, chip_request_t *req);
int H_27xxx_read_chip( chip_info_t *ci, chip_request_t *req);
int H_27xxx_is_blank( chip_info_t *ci, chip_request_t *req);
