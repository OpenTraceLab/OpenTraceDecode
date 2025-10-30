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

import opentracedecode as otd
from .lists import registers, channels

class Decoder(otd.Decoder):
    api_version = 3
    id = 'ad5593r'
    name = 'AD5593R'
    longname = 'Analog Devices AD5593R'
    desc = '12-bit ADC/DAC with 8 channels over I2C interface.'
    license = 'gplv3+'
    inputs = ['i2c']
    outputs = ['ad5593r']
    tags = ['IC', 'Analog/digital']
    options = (
        {'id': 'address', 'desc': 'Chip address (7-bit)', 'default': 0x10,
            'values': tuple(range(0x10, 0x18))},
    )
    annotations = (
        ('register', 'Register'),
        ('data', 'Data'),
        ('adc-data', 'ADC data'),
        ('dac-data', 'DAC data'),
        ('gpio-data', 'GPIO data'),
        ('warning', 'Warning'),
    )
    annotation_rows = (
        ('regs', 'Registers', (0,)),
        ('data', 'Data', (1, 2, 3, 4)),
        ('warnings', 'Warnings', (5,)),
    )

    def __init__(self):
        self.reset()

    def reset(self):
        self.state = 'IDLE'
        self.addr = None
        self.reg = None
        self.data_bytes = []

    def start(self):
        self.out_ann = self.register(otd.OUTPUT_ANN)
        self.out_python = self.register(otd.OUTPUT_PYTHON)

    def decode_register(self, reg_addr, data_bytes):
        """Decode register operation based on address and data"""
        reg_name = registers.get(reg_addr, f'Unknown(0x{reg_addr:02X})')
        
        if reg_addr == 0x02:  # ADC_SEQUENCE
            if len(data_bytes) >= 1:
                channels_mask = data_bytes[0]
                active_channels = []
                for i in range(8):
                    if channels_mask & (1 << i):
                        active_channels.append(channels[i])
                return f"ADC Sequence: {', '.join(active_channels)}"
        
        elif reg_addr == 0x05:  # DAC_CONFIG
            if len(data_bytes) >= 1:
                channels_mask = data_bytes[0]
                active_channels = []
                for i in range(8):
                    if channels_mask & (1 << i):
                        active_channels.append(channels[i])
                return f"DAC Config: {', '.join(active_channels)}"
        
        elif reg_addr == 0x09:  # GPIO_WRITE_DATA
            if len(data_bytes) >= 1:
                gpio_data = data_bytes[0]
                return f"GPIO Write: 0x{gpio_data:02X}"
        
        elif reg_addr == 0x0A:  # GPIO_READ_CONFIG
            if len(data_bytes) >= 1:
                channels_mask = data_bytes[0]
                active_channels = []
                for i in range(8):
                    if channels_mask & (1 << i):
                        active_channels.append(channels[i])
                return f"GPIO Read Config: {', '.join(active_channels)}"
        
        # Default case
        data_str = ' '.join([f'0x{b:02X}' for b in data_bytes])
        return f"{reg_name}: {data_str}" if data_bytes else reg_name

    def decode(self, ss, es, data):
        ptype, pdata = data

        if ptype == 'ADDRESS READ':
            if pdata == self.options['address']:
                self.state = 'READ_DATA'
                self.addr = pdata
                self.data_bytes = []
        
        elif ptype == 'ADDRESS WRITE':
            if pdata == self.options['address']:
                self.state = 'WRITE_REG'
                self.addr = pdata
                self.data_bytes = []
        
        elif ptype == 'DATA WRITE':
            if self.state == 'WRITE_REG':
                if self.reg is None:
                    # First byte is register address
                    self.reg = pdata
                    self.put(ss, es, self.out_ann, [0, [registers.get(pdata, f'Reg 0x{pdata:02X}')]])
                else:
                    # Subsequent bytes are data
                    self.data_bytes.append(pdata)
                    self.put(ss, es, self.out_ann, [1, [f'0x{pdata:02X}']])
        
        elif ptype == 'DATA READ':
            if self.state == 'READ_DATA':
                self.data_bytes.append(pdata)
                
                # Try to interpret read data based on context
                if len(self.data_bytes) == 2:
                    # 12-bit ADC data (2 bytes)
                    adc_value = (self.data_bytes[0] << 8) | self.data_bytes[1]
                    adc_value &= 0x0FFF  # 12-bit mask
                    voltage = (adc_value / 4095.0) * 2.5  # Assuming 2.5V reference
                    self.put(ss, es, self.out_ann, [2, [f'ADC: {adc_value} ({voltage:.3f}V)']])
                else:
                    self.put(ss, es, self.out_ann, [1, [f'0x{pdata:02X}']])
        
        elif ptype == 'STOP':
            if self.state in ('WRITE_REG', 'READ_DATA') and self.reg is not None:
                # Decode the complete transaction
                decoded = self.decode_register(self.reg, self.data_bytes)
                
                # Output Python data
                self.put(ss, es, self.out_python, {
                    'type': 'transaction',
                    'register': self.reg,
                    'register_name': registers.get(self.reg, 'Unknown'),
                    'data': self.data_bytes,
                    'decoded': decoded
                })
            
            # Reset state
            self.state = 'IDLE'
            self.reg = None
            self.data_bytes = []
