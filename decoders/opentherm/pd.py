##
## This file is part of the libopentracedecode project.
##
## Copyright (C) 2024 MaxWolf (ported to OpenTraceDecode)
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
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
from .lists import msg_type, data_ids

class Decoder(otd.Decoder):
    api_version = 3
    id = 'opentherm'
    name = 'OpenTherm'
    longname = 'OpenTherm protocol'
    desc = 'OpenTherm is a standard for communication between modulating boilers and room thermostats.'
    license = 'gplv2+'
    inputs = ['logic']
    outputs = ['opentherm']
    tags = ['Embedded/industrial', 'HVAC']
    channels = (
        {'id': 'data', 'name': 'Data', 'desc': 'OpenTherm data line'},
    )
    options = (
        {'id': 'polarity', 'desc': 'Polarity', 'default': 'active-high',
            'values': ('active-high', 'active-low')},
    )
    annotations = (
        ('bit', 'Bit'),
        ('start', 'Start bit'),
        ('msg-type', 'Message type'),
        ('data-id', 'Data ID'),
        ('data-value', 'Data value'),
        ('parity', 'Parity'),
        ('stop', 'Stop bit'),
        ('frame', 'Frame'),
        ('warning', 'Warning'),
    )
    annotation_rows = (
        ('bits', 'Bits', (0, 1, 6)),
        ('fields', 'Fields', (2, 3, 4, 5)),
        ('frames', 'Frames', (7,)),
        ('warnings', 'Warnings', (8,)),
    )

    def __init__(self):
        self.reset()

    def reset(self):
        self.samplerate = None
        self.state = 'WAIT_START'
        self.bit_start = None
        self.bit_end = None
        self.bits = []
        self.frame_start = None
        self.last_edge = None
        self.bit_time = None

    def start(self):
        self.out_ann = self.register(otd.OUTPUT_ANN)
        self.out_python = self.register(otd.OUTPUT_PYTHON)

    def metadata(self, key, value):
        if key == otd.SRD_CONF_SAMPLERATE:
            self.samplerate = value
            # OpenTherm bit time is 1ms
            self.bit_time = int(self.samplerate * 0.001)

    def decode_manchester(self, pin):
        """Decode Manchester encoded data"""
        if self.last_edge is None:
            self.last_edge = self.samplenum
            return None

        # Calculate time since last edge
        edge_time = self.samplenum - self.last_edge
        
        # Check if this is approximately half bit time (Manchester transition)
        if abs(edge_time - self.bit_time // 2) < self.bit_time // 4:
            # This is a Manchester transition in the middle of a bit
            self.last_edge = self.samplenum
            return None
        
        # This should be a bit boundary
        if abs(edge_time - self.bit_time) < self.bit_time // 4:
            # Determine bit value based on transition direction
            bit_value = 1 if pin else 0
            if self.options['polarity'] == 'active-low':
                bit_value = 1 - bit_value
            
            self.last_edge = self.samplenum
            return bit_value
        
        # Invalid timing
        self.last_edge = self.samplenum
        return None

    def decode_frame(self, bits):
        """Decode 32-bit OpenTherm frame"""
        if len(bits) != 32:
            return None

        # Convert bits to 32-bit value
        frame_value = 0
        for bit in bits:
            frame_value = (frame_value << 1) | bit

        # Extract fields
        parity = (frame_value >> 31) & 1
        msg_type_val = (frame_value >> 28) & 7
        spare = (frame_value >> 24) & 15
        data_id = (frame_value >> 16) & 255
        data_value = frame_value & 65535

        # Calculate expected parity (even parity)
        expected_parity = 0
        temp = frame_value & 0x7FFFFFFF  # Exclude parity bit
        while temp:
            expected_parity ^= temp & 1
            temp >>= 1

        return {
            'parity': parity,
            'parity_ok': parity == expected_parity,
            'msg_type': msg_type_val,
            'spare': spare,
            'data_id': data_id,
            'data_value': data_value,
            'frame_value': frame_value
        }

    def put_annotation(self, start, end, ann_type, data):
        self.put(start, end, self.out_ann, [ann_type, data])

    def put_python(self, start, end, data):
        self.put(start, end, self.out_python, data)

    def decode(self):
        if not self.samplerate:
            raise Exception('Cannot decode without samplerate.')

        while True:
            # Wait for any edge
            pin, = self.wait({'data': 'e'})

            if self.state == 'WAIT_START':
                # Look for start bit (should be low for 1ms)
                if not pin:  # Falling edge
                    self.frame_start = self.samplenum
                    self.bit_start = self.samplenum
                    self.bits = []
                    self.last_edge = self.samplenum
                    self.state = 'DECODE_BITS'

            elif self.state == 'DECODE_BITS':
                bit = self.decode_manchester(pin)
                if bit is not None:
                    self.bits.append(bit)
                    
                    # Annotate the bit
                    bit_end = self.samplenum
                    if len(self.bits) == 1:
                        self.put_annotation(self.bit_start, bit_end, 1, ['Start'])
                    else:
                        self.put_annotation(self.bit_start, bit_end, 0, [str(bit)])
                    
                    self.bit_start = self.samplenum

                    # Check if we have a complete frame (32 bits + start bit)
                    if len(self.bits) >= 33:
                        # Remove start bit
                        frame_bits = self.bits[1:33]
                        
                        # Decode the frame
                        frame = self.decode_frame(frame_bits)
                        if frame:
                            # Annotate frame fields
                            frame_end = self.samplenum
                            
                            # Message type
                            msg_type_desc = msg_type.get(frame['msg_type'], ['UNK', 'Unknown', 'UNK'])
                            self.put_annotation(self.frame_start, frame_end, 2, 
                                [f"{msg_type_desc[1]} ({frame['msg_type']})", msg_type_desc[2]])
                            
                            # Data ID
                            data_id_desc = data_ids.get(frame['data_id'], f"ID {frame['data_id']}")
                            self.put_annotation(self.frame_start, frame_end, 3,
                                [f"{data_id_desc} ({frame['data_id']})", f"ID{frame['data_id']}"])
                            
                            # Data value
                            self.put_annotation(self.frame_start, frame_end, 4,
                                [f"Value: {frame['data_value']}", f"{frame['data_value']}"])
                            
                            # Parity
                            parity_status = "OK" if frame['parity_ok'] else "ERR"
                            self.put_annotation(self.frame_start, frame_end, 5,
                                [f"Parity: {parity_status}", parity_status])
                            
                            # Complete frame
                            self.put_annotation(self.frame_start, frame_end, 7,
                                [f"OpenTherm Frame: {msg_type_desc[1]}, ID={frame['data_id']}, Value={frame['data_value']}",
                                 f"OT: {msg_type_desc[2]} ID{frame['data_id']}"])
                            
                            # Python output
                            self.put_python(self.frame_start, frame_end, {
                                'type': 'frame',
                                'msg_type': frame['msg_type'],
                                'data_id': frame['data_id'],
                                'data_value': frame['data_value'],
                                'parity_ok': frame['parity_ok']
                            })
                            
                            if not frame['parity_ok']:
                                self.put_annotation(self.frame_start, frame_end, 8,
                                    ['Parity error', 'PE'])

                        self.state = 'WAIT_START'
