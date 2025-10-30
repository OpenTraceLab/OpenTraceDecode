##
## This file is part of the libopentracedecode project.
##
## Copyright (C) 2024 Andrea Orazi (ported to OpenTraceDecode)
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

class Decoder(otd.Decoder):
    api_version = 3
    id = 'keeloq'
    name = 'KeeLoq'
    longname = 'KeeLoq remote keyless entry'
    desc = 'KeeLoq block cipher for remote keyless entry systems.'
    license = 'gplv2+'
    inputs = ['logic']
    outputs = ['keeloq']
    tags = ['Security/crypto', 'Automotive', 'Wireless/RF']
    channels = (
        {'id': 'data', 'name': 'Data', 'desc': 'KeeLoq PWM data line'},
    )
    options = (
        {'id': 'te_time', 'desc': 'TE time (µs)', 'default': 400, 'values': (260, 400, 520)},
    )
    annotations = (
        ('bit', 'Bit'),
        ('preamble', 'Preamble'),
        ('header', 'Header'),
        ('encrypted', 'Encrypted portion'),
        ('serial', 'Serial number'),
        ('button', 'Button code'),
        ('status', 'Status'),
        ('frame', 'Frame'),
        ('warning', 'Warning'),
    )
    annotation_rows = (
        ('bits', 'Bits', (0,)),
        ('fields', 'Fields', (1, 2, 3, 4, 5, 6)),
        ('frames', 'Frames', (7,)),
        ('warnings', 'Warnings', (8,)),
    )

    def __init__(self):
        self.reset()

    def reset(self):
        self.samplerate = None
        self.state = 'WAIT_PREAMBLE'
        self.bits = []
        self.frame_start = None
        self.bit_start = None
        self.te_samples = None
        self.preamble_count = 0

    def start(self):
        self.out_ann = self.register(otd.OUTPUT_ANN)
        self.out_python = self.register(otd.OUTPUT_PYTHON)

    def metadata(self, key, value):
        if key == otd.SRD_CONF_SAMPLERATE:
            self.samplerate = value
            # Convert TE time from microseconds to samples
            te_time_us = self.options['te_time']
            self.te_samples = int(self.samplerate * te_time_us / 1000000)

    def decode_bit(self, high_time, low_time):
        """Decode PWM bit based on pulse widths"""
        total_time = high_time + low_time
        
        # Check if this looks like a valid TE period
        if abs(total_time - self.te_samples) > self.te_samples * 0.3:
            return None
        
        # Determine bit value based on duty cycle
        # Bit 0: ~25% duty cycle, Bit 1: ~75% duty cycle
        duty_cycle = high_time / total_time
        
        if duty_cycle < 0.4:
            return 0
        elif duty_cycle > 0.6:
            return 1
        else:
            return None  # Invalid duty cycle

    def decode_frame(self, bits):
        """Decode 66-bit KeeLoq frame"""
        if len(bits) != 66:
            return None

        # Convert bits to integers (LSB first)
        encrypted = 0
        for i in range(32):
            encrypted |= bits[i] << i
        
        serial = 0
        for i in range(28):
            serial |= bits[32 + i] << i
        
        # Button and status bits
        button = (bits[60] << 3) | (bits[61] << 0) | (bits[62] << 1) | (bits[63] << 2)
        vlow = bits[64]
        repeat = bits[65]

        return {
            'encrypted': encrypted,
            'serial': serial,
            'button': button,
            'vlow': vlow,
            'repeat': repeat
        }

    def put_annotation(self, start, end, ann_type, data):
        self.put(start, end, self.out_ann, [ann_type, data])

    def put_python(self, start, end, data):
        self.put(start, end, self.out_python, data)

    def decode(self):
        if not self.samplerate:
            raise Exception('Cannot decode without samplerate.')

        last_edge = None
        high_start = None
        
        while True:
            pin, = self.wait({'data': 'e'})
            
            if last_edge is None:
                last_edge = self.samplenum
                if pin:  # Rising edge
                    high_start = self.samplenum
                continue

            if self.state == 'WAIT_PREAMBLE':
                if pin:  # Rising edge
                    if high_start is not None:
                        # We had a complete low-high-low cycle
                        low_time = high_start - last_edge
                        high_time = self.samplenum - high_start
                        
                        # Check if this looks like preamble (50% duty cycle)
                        total_time = low_time + high_time
                        if abs(total_time - self.te_samples) < self.te_samples * 0.3:
                            duty_cycle = high_time / total_time
                            if 0.4 < duty_cycle < 0.6:  # ~50% duty cycle
                                if self.preamble_count == 0:
                                    self.frame_start = last_edge
                                self.preamble_count += 1
                                if self.preamble_count >= 23:
                                    self.put_annotation(self.frame_start, self.samplenum, 1, 
                                        ['Preamble', 'Pre'])
                                    self.state = 'WAIT_HEADER'
                                    self.preamble_count = 0
                    high_start = self.samplenum
                
            elif self.state == 'WAIT_HEADER':
                if not pin:  # Falling edge - end of potential header
                    header_time = self.samplenum - last_edge
                    # Header should be ~10 TE periods low
                    if abs(header_time - 10 * self.te_samples) < 2 * self.te_samples:
                        self.put_annotation(last_edge, self.samplenum, 2, ['Header', 'Hdr'])
                        self.state = 'DECODE_DATA'
                        self.bits = []
                        self.bit_start = self.samplenum

            elif self.state == 'DECODE_DATA':
                if pin:  # Rising edge
                    if high_start is not None:
                        # Complete bit period
                        low_time = high_start - last_edge
                        high_time = self.samplenum - high_start
                        
                        bit = self.decode_bit(high_time, low_time)
                        if bit is not None:
                            self.bits.append(bit)
                            self.put_annotation(last_edge, self.samplenum, 0, [str(bit)])
                            
                            # Check if we have complete frame
                            if len(self.bits) == 66:
                                frame = self.decode_frame(self.bits)
                                if frame:
                                    frame_end = self.samplenum
                                    
                                    # Annotate frame parts
                                    self.put_annotation(self.frame_start, frame_end, 3,
                                        [f"Encrypted: 0x{frame['encrypted']:08X}", f"Enc: {frame['encrypted']:08X}"])
                                    self.put_annotation(self.frame_start, frame_end, 4,
                                        [f"Serial: 0x{frame['serial']:07X}", f"SN: {frame['serial']:07X}"])
                                    self.put_annotation(self.frame_start, frame_end, 5,
                                        [f"Button: {frame['button']}", f"Btn: {frame['button']}"])
                                    
                                    status_str = []
                                    if frame['vlow']:
                                        status_str.append('VLOW')
                                    if frame['repeat']:
                                        status_str.append('RPT')
                                    status = ' '.join(status_str) if status_str else 'OK'
                                    self.put_annotation(self.frame_start, frame_end, 6,
                                        [f"Status: {status}", status])
                                    
                                    # Complete frame
                                    self.put_annotation(self.frame_start, frame_end, 7,
                                        [f"KeeLoq: SN=0x{frame['serial']:07X}, Btn={frame['button']}, {status}",
                                         f"KL: {frame['serial']:07X}"])
                                    
                                    # Python output
                                    self.put_python(self.frame_start, frame_end, {
                                        'type': 'frame',
                                        'encrypted': frame['encrypted'],
                                        'serial': frame['serial'],
                                        'button': frame['button'],
                                        'vlow': frame['vlow'],
                                        'repeat': frame['repeat']
                                    })
                                
                                self.state = 'WAIT_PREAMBLE'
                                self.preamble_count = 0
                    
                    high_start = self.samplenum
                else:  # Falling edge
                    high_start = None

            last_edge = self.samplenum
