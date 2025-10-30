##
## This file is part of the libopentracedecode project.
##
## Copyright (C) 2024 Analog Devices Inc. (ported to OpenTraceDecode)
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, see <http://www.gnu.org/licenses/>.
##

# AD5593R register addresses
registers = {
    0x00: 'NOP',
    0x01: 'DAC_READBACK',
    0x02: 'ADC_SEQUENCE',
    0x03: 'GP_CONTROL',
    0x04: 'ADC_CONFIG',
    0x05: 'DAC_CONFIG',
    0x06: 'PULLDOWN_CONFIG',
    0x07: 'LDAC_MODE',
    0x08: 'GPIO_WRITE_CONFIG',
    0x09: 'GPIO_WRITE_DATA',
    0x0A: 'GPIO_READ_CONFIG',
    0x0B: 'POWER_REF_CTRL',
    0x0C: 'OPEN_DRAIN_CFG',
    0x0D: 'TRISTATE_CFG',
    0x0E: 'RESET',
}

# Channel names
channels = {
    0: 'CH0/IO0',
    1: 'CH1/IO1', 
    2: 'CH2/IO2',
    3: 'CH3/IO3',
    4: 'CH4/IO4',
    5: 'CH5/IO5',
    6: 'CH6/IO6',
    7: 'CH7/IO7',
}
