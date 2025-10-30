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

# OpenTherm message types
msg_type = {
    0: ['M2S', 'READ-DATA', 'RD'],
    1: ['M2S', 'WRITE-DATA', 'WD'],
    2: ['M2S', 'INVALID-DATA', 'INV'],
    3: ['M2S', 'RESERVED', 'RSV'],
    4: ['S2M', 'READ-ACK', 'RACK'],
    5: ['S2M', 'WRITE-ACK', 'WACK'],
    6: ['S2M', 'DATA-INVALID', 'INV'],
    7: ['S2M', 'UNKNOWN-DATAID', 'UNK'],
}

# Common OpenTherm data IDs
data_ids = {
    0: 'Status',
    1: 'Control setpoint',
    2: 'Master configuration',
    3: 'Slave configuration',
    4: 'Command',
    5: 'ASF-flags / OEM fault code',
    6: 'RBP-flags',
    7: 'Cooling control',
    8: 'Control setpoint 2',
    9: 'Room setpoint override',
    10: 'TSP number',
    11: 'TSP entry',
    12: 'FHB size',
    13: 'FHB entry',
    14: 'Max relative modulation level',
    15: 'Max boiler capacity',
    16: 'Room setpoint',
    17: 'Relative modulation level',
    18: 'CH water pressure',
    19: 'DHW flow rate',
    20: 'Day of week and time of day',
    21: 'Date',
    22: 'Year',
    23: 'Room setpoint 2',
    24: 'Room temperature',
    25: 'Boiler flow water temperature',
    26: 'DHW temperature',
    27: 'Outside temperature',
    28: 'Return water temperature',
    29: 'Solar storage temperature',
    30: 'Solar collector temperature',
    31: 'Flow water temperature 2',
    32: 'DHW temperature 2',
    33: 'Exhaust temperature',
}
