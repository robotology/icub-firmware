#!/usr/bin/env python3
# tethercodec - codec of TETHER, the ROP protocol (Remote Operation Protocol, version 6) of the amcmj1 board.
# shared by the two services of tether.py (rpc and str) and by the tools of the Speedgoat link (amc_gtw.py).
# one file for all the boards and examples: embot/svc/host/tether/tethercodec.py, next to tether.py
#
# wire format (all little endian):
#   frame  = header (16 bytes) + numOfROPs ROPs one after the other, max 32 ROPs and max 1472 bytes (the board tells
#            its own limits in MGbrowseINFO)
#   header = { uint32 signature, uint8 layout = 0, uint8 numOfROPs, uint16 filler = 0, uint64 timeinmicroseconds }
#   ROP    = { uint8 opc, uint8 id, uint16 size } + data[size] + zero padding up to a multiple of 4
#     layout : 0 = variable-length ROPs. any other value (e.g. the uniform slots of version 5) is refused
#     opc    : bits 0-3 = CMD, bits 4-7 = in ask, set, sig, run, ping the options (bit 4 = ack required),
#              in ack, nak the CMD that is acknowledged
#     size   : at most 512 bytes. the padding is ignored by the receiver
#
# IDs:
#   0x00          no ID
#   0x01 - 0x9F   variables     IDxxx   ask, set[+ack], sig
#   0xA0 - 0xDF   operations    OPxxx   run[+ack]
#   0xE0 - 0xFF   management    MGxxx   built in here: they are not in the protocol table
#
# an ack is light: it carries data only for the result of a run (at most 36 bytes). in a nak the data is a text (no NUL,
# size = number of chars, at most 36): why the board refused. a value travels in say (the reply to ask) and in sig.
# all the replies to a frame go in one frame: Codec.split() makes frames whose replies fit the frames of the board
#
# keep it aligned with embot_svc_tether_ROP.h and embot_svc_tether_SharedVariables.h on the board

import datetime
import re
import struct
import sys
import time
import tomllib
import zlib
from collections import namedtuple
from enum import IntEnum

PROTOCOL_VERSION = 6
SIGNATURE = 0xFEEDC0DE          # "feed code"
LAYOUT_VARIABLE = 0             # byte 4 of the header: variable-length ROPs
MAX_ROPS = 32                   # of this host. the board says its own in MGbrowseINFO: the smaller one is used
MAX_FRAME = 1472                # a UDP datagram that does not fragment. the board says its own in MGbrowseINFO
MIN_FRAME = 128                 # the smallest frame a board may declare
FRAME_HEADER_FMT = '<IBBHQ'     # signature, layout, numOfROPs, filler, timeinmicroseconds
FRAME_HEADER_SIZE = struct.calcsize(FRAME_HEADER_FMT)
assert FRAME_HEADER_SIZE == 16
ROP_HEADER_FMT = '<BBH'         # opc, id, size
ROP_HEADER_SIZE = struct.calcsize(ROP_HEADER_FMT)
assert ROP_HEADER_SIZE == 4
DATASIZE = 512                  # max data of a ROP: a variable, the argument of an operation
RESULTSIZE = 36                 # max result of an operation, carried by its ack
TEXTSIZE = 36                   # max text of a nak
NAME_SIZE = 28


def pad4(size):
    return (size + 3) & ~3


def rop_size(datasize):
    """the bytes taken in a frame by a ROP w/ datasize bytes of data"""
    return ROP_HEADER_SIZE + pad4(datasize)


SHORT_REPLY = rop_size(max(RESULTSIZE, TEXTSIZE))      # 40: the biggest ack or nak


# ---- IDs

ID_NONE = 0x00


def is_variable(vid):
    return 0x01 <= vid <= 0x9F


def is_op(vid):
    return 0xA0 <= vid <= 0xDF


def is_mg(vid):
    return 0xE0 <= vid <= 0xFF


def kind_of(vid):
    return 'var' if is_variable(vid) else 'op' if is_op(vid) else 'mg' if is_mg(vid) else 'none'


MG_STREAM_ADD = 0xE0
MG_STREAM_REM = 0xE1
MG_STREAM_LIST = 0xE2
MG_STREAM_START = 0xE3
MG_STREAM_STOP = 0xE4
MG_BROWSE_INFO = 0xE8
MG_BROWSE_ITEM = 0xE9
MG_BROWSE_FIELD = 0xEA
MG_VERSION = 0xF0
MG_STATS = 0xF1


class CMD(IntEnum):
    ask = 0
    say = 1
    set = 2
    sig = 3
    ack = 4
    nak = 5
    ping = 6
    run = 7
    none = 15


CMD_ALIASES = {'get': CMD.ask}

# options in the high nibble of opc, given by the user as cmd[+name]
OPT_ACK = 0x1
OPTIONS = {'+ack': OPT_ACK}
OPTIONS_ALLOWED = {CMD.set: OPT_ACK, CMD.run: OPT_ACK}

# the management IDs: name -> (id, size of the value of set, its scalar type, the CMDs the host may send)
MG_TABLE = {
    'MGstreamADD':   (MG_STREAM_ADD, 1, 'u8', (CMD.set, CMD.sig)),
    'MGstreamREM':   (MG_STREAM_REM, 1, 'u8', (CMD.set, CMD.sig)),
    'MGstreamLIST':  (MG_STREAM_LIST, 0, None, (CMD.ask,)),
    'MGstreamSTART': (MG_STREAM_START, 2, 'u16', (CMD.set, CMD.sig, CMD.ask)),    # 0 = default period, else ms
    'MGstreamSTOP':  (MG_STREAM_STOP, 0, None, (CMD.set, CMD.sig)),             # set<MGstreamSTOP>
    'MGbrowseINFO':  (MG_BROWSE_INFO, 0, None, (CMD.ask,)),
    'MGbrowseITEM':  (MG_BROWSE_ITEM, 0, None, (CMD.ask,)),     # ask<MGbrowseITEM, index>
    'MGbrowseFIELD': (MG_BROWSE_FIELD, 0, None, (CMD.ask,)),    # ask<MGbrowseFIELD, IDname, n>
    'MGversion':     (MG_VERSION, 0, None, (CMD.ask,)),
    'MGstats':       (MG_STATS, 0, None, (CMD.ask,)),
}


class ProtocolError(Exception):
    pass


# a ROP and a frame, decoded
# opts: the options of ask, set, sig, run, ping. refcmd: the CMD acknowledged by an ack or a nak (in the place of the options)
Rop = namedtuple('Rop', 'cmd opts id size refcmd data')
Frame = namedtuple('Frame', 'numOfROPs time size rops warnings errors')


def make_rop(cmd, vid, data=b'', opts=0):
    return Rop(int(cmd), opts, vid, len(data), 0, bytes(data))


def make_opc(cmd, opts, refcmd=0):
    """an ack or a nak carries refcmd in the high nibble, the others their options"""
    high = refcmd if int(cmd) in (CMD.ack, CMD.nak) else opts
    return ((high & 0x0F) << 4) | (int(cmd) & 0x0F)


def split_opc(opc):
    """(cmd, opts, refcmd)"""
    cmd, high = opc & 0x0F, (opc >> 4) & 0x0F
    if cmd in (CMD.ack, CMD.nak):
        return cmd, 0, high
    return cmd, high, 0


def format_options(opts):
    names = [name for name, bit in OPTIONS.items() if opts & bit]
    unknown = opts & ~sum(OPTIONS.values())
    if unknown:
        names.append(f'+0x{unknown:X}')
    return f'[{",".join(names)}]' if names else ''


def cmd_name(code):
    return CMD(code).name if code in CMD._value2member_map_ else f'cmd#{code}'


def format_time(us):
    """formats microseconds as 00H:07M:55S:337m:046u"""
    s, us = divmod(us, 1000000)
    ms, us = divmod(us, 1000)
    m, s = divmod(s, 60)
    h, m = divmod(m, 60)
    return f'{h:02d}H:{m:02d}M:{s:02d}S:{ms:03d}m:{us:03d}u'


def format_application(v):
    """the version of the application: 4 bytes, as the application gives them to the board. 0 = not given"""
    return 'not given' if v == 0 else f'0x{v:08X}'


def parse_int(text):
    try:
        return int(text, 0)
    except ValueError:
        raise ProtocolError(f'"{text}" is not a valid number')


# ---- types

# scalar: (code on the wire, size, struct format). the codes are Dictionary::Type on the board
SCALARS = {
    'bool': (1, 1, '?'), 'u8': (2, 1, 'B'), 'i8': (3, 1, 'b'), 'u16': (4, 2, 'H'), 'i16': (5, 2, 'h'),
    'u32': (6, 4, 'I'), 'i32': (7, 4, 'i'), 'u64': (8, 8, 'Q'), 'i64': (9, 8, 'q'), 'f32': (10, 4, 'f'), 'f64': (11, 8, 'd'),
}
CODE_RAW = 0
CODE_STRUCT = 16
SCALAR_BY_CODE = {v[0]: k for k, v in SCALARS.items()}
NAME_RE = re.compile(r'[A-Za-z_]\w*')


def type_name(code):
    if code == CODE_RAW:
        return 'raw'
    if code == CODE_STRUCT:
        return 'struct'
    return SCALAR_BY_CODE.get(code, f'type#{code}')


class Ident(str):
    """an IDname used as a value, e.g. set<MGstreamADD, IDdummyUINT32>"""


_NUM_RE = re.compile(r'[-+]?(?:0[xX][0-9a-fA-F]+|0[bB][01]+|(?:\d+\.\d*|\.\d+|\d+)(?:[eE][-+]?\d+)?|inf(?!\w)|nan(?!\w))')


class _ValueParser:
    """value := number | true | false | IDname | [value, ...] | {name = value, ...}"""

    def __init__(self, text):
        self.s = text
        self.i = 0

    def error(self, msg):
        raise ProtocolError(f'{msg} at position {self.i} of "{self.s}"')

    def skip(self):
        while self.i < len(self.s) and self.s[self.i].isspace():
            self.i += 1

    def parse(self):
        v = self.value()
        self.skip()
        if self.i != len(self.s):
            self.error('unexpected text')
        return v

    def expect(self, ch):
        self.skip()
        if self.i >= len(self.s) or self.s[self.i] != ch:
            self.error(f'expected "{ch}"')
        self.i += 1

    def value(self):
        self.skip()
        if self.i >= len(self.s):
            self.error('missing value')
        c = self.s[self.i]
        if c == '[':
            return self.sequence()
        if c == '{':
            return self.structure()
        m = _NUM_RE.match(self.s, self.i)
        if m:
            self.i = m.end()
            return self.number(m.group(0))
        m = NAME_RE.match(self.s, self.i)
        if m:
            self.i = m.end()
            word = m.group(0)
            return {'true': True, 'false': False}.get(word, Ident(word))
        self.error('unexpected character')

    @staticmethod
    def number(tok):
        t = tok.lower().lstrip('+-')
        if t.startswith(('0x', '0b')):
            return int(tok, 0)
        if t in ('inf', 'nan') or '.' in t or 'e' in t:
            return float(tok)
        return int(tok, 10)

    def sequence(self):
        self.expect('[')
        items = [self.value()]
        self.skip()
        while self.i < len(self.s) and self.s[self.i] == ',':
            self.i += 1
            items.append(self.value())
            self.skip()
        self.expect(']')
        return items

    def structure(self):
        self.expect('{')
        fields = {}
        while True:
            self.skip()
            m = NAME_RE.match(self.s, self.i)
            if not m:
                self.error('expected a field name')
            self.i = m.end()
            if m.group(0) in fields:
                self.error(f'field "{m.group(0)}" given twice')
            self.expect('=')
            fields[m.group(0)] = self.value()
            self.skip()
            if self.i < len(self.s) and self.s[self.i] == ',':
                self.i += 1
                continue
            break
        self.expect('}')
        return fields


def parse_value(text):
    return _ValueParser(text).parse()


def split_top(text, sep):
    """splits at sep outside of [] and {}"""
    parts, depth, cur = [], 0, []
    for ch in text:
        if ch in '[{':
            depth += 1
        elif ch in ']}':
            depth -= 1
        if ch == sep and depth == 0:
            parts.append(''.join(cur))
            cur = []
        else:
            cur.append(ch)
    parts.append(''.join(cur))
    return [p.strip() for p in parts]


def format_scalar(scalar, v):
    """a scalar of the Dictionary as text"""
    if scalar == 'bool':
        return 'true' if v else 'false'
    if scalar == 'f32':
        return format(v, '.7g')
    if scalar == 'f64':
        return format(v, '.15g')
    return str(v)


def _encode_scalar(scalar, v, resolve):
    code, size, fmt = SCALARS[scalar]
    if isinstance(v, Ident):
        v = resolve(v)
    if scalar == 'bool':
        if isinstance(v, bool) or v in (0, 1):
            return struct.pack('<?', bool(v))
        raise ProtocolError(f'bool needs true or false, not {v!r}')
    if scalar in ('f32', 'f64'):
        if isinstance(v, bool) or not isinstance(v, (int, float)):
            raise ProtocolError(f'{scalar} needs a number, not {v!r}')
        try:
            return struct.pack('<' + fmt, float(v))
        except (OverflowError, struct.error):
            raise ProtocolError(f'{v} does not fit a {scalar}')
    if isinstance(v, bool) or not isinstance(v, int):
        raise ProtocolError(f'{scalar} needs an integer, not {v!r}')
    bits = 8 * size
    if scalar.startswith('i'):
        lo, hi = -(1 << (bits - 1)), (1 << (bits - 1)) - 1
        if hi < v < (1 << bits):        # a bit pattern, e.g. 0xFFFE for -2
            v -= 1 << bits
    else:
        lo, hi = 0, (1 << bits) - 1
    if not lo <= v <= hi:
        raise ProtocolError(f'{v} is out of the range of {scalar}')
    return struct.pack('<' + fmt, v)


def _raw_bytes(v, size):
    if isinstance(v, bool) or not isinstance(v, int):
        raise ProtocolError(f'a raw value of {size} bytes must be an integer, not {v!r}')
    nbits = 8 * size
    if v < 0:
        if v < -(1 << (nbits - 1)):
            raise ProtocolError(f'{v} does not fit in {size} bytes')
        v &= (1 << nbits) - 1
    elif v >= (1 << nbits):
        raise ProtocolError(f'0x{v:X} does not fit in {size} bytes')
    return v.to_bytes(size, 'little')


def format_text(data):
    """the text of a nak"""
    return bytes(data).decode('ascii', 'replace')


def format_raw(data):
    return f'{len(data)}, 0x{data[::-1].hex().upper()}' if data else '0, -'


class TypeSpec:
    """how the bytes of a value are read: raw, a scalar (or an array of them), a struct of scalar fields.
    the fields are (name, scalar, count, offset): the gaps between them are padding"""

    RAW, SCALAR, STRUCT = 'raw', 'scalar', 'struct'

    def __init__(self, kind, size, scalar=None, count=1, fields=(), name=None):
        self.kind = kind
        self.size = size
        self.scalar = scalar
        self.count = count
        self.fields = list(fields)
        self.name = name          # only for the scalar (or array) argument of an operation, e.g. "periodms"

    @classmethod
    def raw(cls, size):
        return cls(cls.RAW, size)

    @staticmethod
    def parse_scalar(text):
        m = re.fullmatch(r'\s*([a-z]+\d*)\s*(?:\[\s*(\d+)\s*\])?\s*', text)
        if not m or m.group(1) not in SCALARS:
            raise ProtocolError(f'unknown type "{text}" (known: {", ".join(SCALARS)})')
        count = int(m.group(2)) if m.group(2) else 1
        if count < 1:
            raise ProtocolError(f'the array "{text}" is empty')
        return m.group(1), count

    @classmethod
    def from_type(cls, text, size):
        """type = "u32" or "f32[6]" """
        scalar, count = cls.parse_scalar(text)
        if SCALARS[scalar][1] * count != size:
            raise ProtocolError(f'type {text} takes {SCALARS[scalar][1] * count} bytes but size is {size}')
        return cls(cls.SCALAR, size, scalar, count)

    @classmethod
    def from_arg(cls, text, size):
        """arg = "periodms:u32" or "xyz:f32[3]": the named argument of an operation"""
        if not isinstance(text, str) or ':' not in text:
            raise ProtocolError(f'arg "{text}" must be "name:type"')
        name, spec = (x.strip() for x in text.split(':', 1))
        if not NAME_RE.fullmatch(name):
            raise ProtocolError(f'arg name "{name}" is not valid')
        out = cls.from_type(spec, size)
        out.name = name
        return out

    @classmethod
    def from_fields(cls, items, size):
        """fields = ["a:i16", "_:pad2", "force:f32[3]"], in order: the offsets follow from the sizes"""
        if not isinstance(items, list) or not items:
            raise ProtocolError('fields must be a non empty list of "name:type"')
        fields, offset, names = [], 0, set()
        for it in items:
            if not isinstance(it, str) or ':' not in it:
                raise ProtocolError(f'field "{it}" must be "name:type"')
            name, spec = (x.strip() for x in it.split(':', 1))
            pad = re.fullmatch(r'pad(\d+)', spec)
            if pad:
                offset += int(pad.group(1))
                continue
            if not NAME_RE.fullmatch(name) or name in names:
                raise ProtocolError(f'field name "{name}" is not valid or is duplicated')
            scalar, count = cls.parse_scalar(spec)
            fields.append((name, scalar, count, offset))
            names.add(name)
            offset += SCALARS[scalar][1] * count
        if not fields:
            raise ProtocolError('fields has only padding')
        if offset != size:
            raise ProtocolError(f'the fields cover {offset} bytes but size is {size}: is some padding missing?')
        return cls(cls.STRUCT, size, fields=fields)

    @classmethod
    def from_board(cls, code, size, fields):
        """from the browse. fields: list of (name, code, count, offset). a scalar w/ one field is the named argument
        of an operation"""
        if code in SCALAR_BY_CODE and len(fields) == 1 and size > 0:
            name, fcode, count, offset = fields[0]
            unit = SCALARS[SCALAR_BY_CODE[code]][1]
            if fcode == code and offset == 0 and unit * count == size:
                return cls(cls.SCALAR, size, SCALAR_BY_CODE[code], count, name=name)
        if code == CODE_STRUCT and fields:
            fl = []
            for name, fcode, count, offset in fields:
                if fcode not in SCALAR_BY_CODE:
                    return cls.raw(size)
                fl.append((name, SCALAR_BY_CODE[fcode], count, offset))
            return cls(cls.STRUCT, size, fields=fl)
        if code in SCALAR_BY_CODE and size > 0:
            unit = SCALARS[SCALAR_BY_CODE[code]][1]
            if size % unit == 0:
                return cls(cls.SCALAR, size, SCALAR_BY_CODE[code], size // unit)
        return cls.raw(size)

    @classmethod
    def from_result(cls, size, kind, fields):
        """the result of an operation from the browse. kind: 0 none / raw, 1 a named scalar (or array), 2 a struct"""
        if size == 0 or kind == 0:
            return cls.raw(size)
        if kind == 1 and len(fields) == 1:
            return cls.from_board(fields[0][1], size, fields)
        if kind == 2 and fields:
            return cls.from_board(CODE_STRUCT, size, fields)
        return cls.raw(size)

    @staticmethod
    def _spec(scalar, count):
        return scalar if count == 1 else f'{scalar}[{count}]'

    def toml(self, argkey='arg', fieldskey='fields'):
        """the part of the dictionary file that describes the type. for the result of an operation the keys are
        result and resultfields"""
        if self.kind == self.SCALAR and self.name:
            return f'{argkey} = "{self.name}:{self._spec(self.scalar, self.count)}"'
        if self.kind == self.SCALAR:
            return f'type = "{self._spec(self.scalar, self.count)}"'
        if self.kind == self.STRUCT:
            parts, offset = [], 0
            for name, scalar, count, off in self.fields:
                if off > offset:
                    parts.append(f'"_:pad{off - offset}"')
                parts.append(f'"{name}:{self._spec(scalar, count)}"')
                offset = off + SCALARS[scalar][1] * count
            if offset < self.size:
                parts.append(f'"_:pad{self.size - offset}"')
            return f'{fieldskey} = [{", ".join(parts)}]'
        return ''

    def describe(self):
        if self.kind == self.SCALAR and self.name:
            return f'{self.name}:{self._spec(self.scalar, self.count)}'
        if self.kind == self.SCALAR:
            return self._spec(self.scalar, self.count)
        if self.kind == self.STRUCT:
            return '{' + ', '.join(f'{n}:{self._spec(s, c)}' for n, s, c, _ in self.fields) + '}'
        return 'raw'

    def usage(self):
        """what to write as the value, e.g. periodms, [xyz0, xyz1, xyz2], {periodms=.., mask=..}"""
        def many(name, count):
            items = [f'{name}{k}' for k in range(count)] if count <= 4 else [f'{name}0', '..', f'{name}{count - 1}']
            return '[' + ', '.join(items) + ']'
        if self.kind == self.SCALAR:
            name = self.name or 'value'
            return name if self.count == 1 else many(name, self.count)
        if self.kind == self.STRUCT:
            return '{' + ', '.join(f'{n}=..' if c == 1 else f'{n}=[{c} values]' for n, _, c, _ in self.fields) + '}'
        return f'0x.. ({self.size} bytes)'

    # ---- values

    def encode(self, value, resolve):
        """value from parse_value(). resolve(Ident) gives the ID of an IDname. a plain integer is accepted for any
        type as the raw bytes of the value (e.g. 0x3FC00000FFFE0001 for a struct of 8 bytes)"""
        if self.kind == self.RAW:
            if isinstance(value, Ident):
                value = resolve(value)
            return _raw_bytes(value, self.size)
        if self.kind == self.SCALAR:
            if self.count == 1:
                return _encode_scalar(self.scalar, value, resolve)
            if isinstance(value, int) and not isinstance(value, bool):
                return _raw_bytes(value, self.size)
            if not isinstance(value, list) or len(value) != self.count:
                raise ProtocolError(f'{self.describe()} needs a list of {self.count} values')
            return b''.join(_encode_scalar(self.scalar, v, resolve) for v in value)
        # struct
        if isinstance(value, int) and not isinstance(value, bool):
            return _raw_bytes(value, self.size)
        if not isinstance(value, dict):
            raise ProtocolError(f'{self.describe()} needs {{name = value, ...}}')
        names = [f[0] for f in self.fields]
        missing = [n for n in names if n not in value]
        extra = [n for n in value if n not in names]
        if missing or extra:
            raise ProtocolError(f'struct {self.describe()}: ' +
                                (f'missing {", ".join(missing)}' if missing else '') +
                                ('; ' if missing and extra else '') +
                                (f'unknown {", ".join(extra)}' if extra else ''))
        out = bytearray(self.size)
        for name, scalar, count, offset in self.fields:
            spec = TypeSpec(self.SCALAR, SCALARS[scalar][1] * count, scalar, count)
            b = spec.encode(value[name], resolve)
            out[offset:offset + len(b)] = b
        return bytes(out)

    def _values(self, scalar, count, data):
        fmt = '<' + SCALARS[scalar][2] * count
        return list(struct.unpack(fmt, data))

    def format(self, data):
        if len(data) != self.size or self.kind == self.RAW:
            return format_raw(data)
        if self.kind == self.SCALAR:
            vals = [format_scalar(self.scalar, v) for v in self._values(self.scalar, self.count, data)]
            return vals[0] if self.count == 1 else '[' + ', '.join(vals) + ']'
        parts = []
        for name, scalar, count, offset in self.fields:
            n = SCALARS[scalar][1] * count
            vals = [format_scalar(scalar, v) for v in self._values(scalar, count, data[offset:offset + n])]
            parts.append(f'{name}={vals[0] if count == 1 else "[" + ", ".join(vals) + "]"}')
        return '{' + ', '.join(parts) + '}'

    def flatten(self, prefix, data):
        """[(column, value)] for a csv file: an array gives name[k], a struct gives name.field"""
        if len(data) != self.size or self.kind == self.RAW:
            return [(prefix, '0x' + data[::-1].hex().upper() if data else '')]
        out = []
        if self.kind == self.SCALAR:
            items = [(prefix, self.scalar, self.count, 0)]
        else:
            items = [(f'{prefix}.{name}', scalar, count, offset) for name, scalar, count, offset in self.fields]
        for col, scalar, count, offset in items:
            n = SCALARS[scalar][1] * count
            vals = [format_scalar(scalar, v) for v in self._values(scalar, count, data[offset:offset + n])]
            if count == 1:
                out.append((col, vals[0]))
            else:
                out.extend((f'{col}[{k}]', v) for k, v in enumerate(vals))
        return out


# ---- the protocol table

class Entry:
    """an ID known to the host. kind: var, op, mg. size: of the variable, or of the argument of an op.
    access: RO, RW or None (unknown, op, mg)"""

    __slots__ = ('name', 'id', 'kind', 'size', 'access', 'spec', 'result')

    def __init__(self, name, vid, kind, size, access, spec, result=None):
        self.name = name
        self.id = vid
        self.kind = kind
        self.size = size
        self.access = access
        self.spec = spec
        self.result = result if result is not None else TypeSpec.raw(0)     # only an operation: what the ack carries


class Protocol:
    """IDname <-> Entry. the MG IDs are built in. the variables and the operations come from a TOML file, which
    tether.py writes from the browse of the board:

        _dictionary   = { protocol = 6, application = 0x01000000, crc32 = 0x1234ABCD, maxframesize = 1472 }   # written by the browse
        IDdummyUINT32 = { id = 0x02, size = 4, access = "RW", type = "u32" }
        IDdummySTRUCT = { id = 0x03, size = 8, access = "RW", fields = ["a:i16", "b:i16", "c:f32"] }
        OPdummyECHO   = { id = 0xC0, size = 4, arg = "value:u32" }                     # the argument: name and type
        OPstartTX     = { id = 0xC3, size = 4, fields = ["periodms:u16", "mask:u8", "_:pad1"] }   # a struct argument
        OPdummyECHO   = { ..., arg = "value:u32", result = "echo:u32" }                # what the ack carries back
        OPgetPOSE     = { ..., size = 0, resultfields = ["x:f32", "y:f32"] }          # a struct result

    type and fields are optional (w/out them the value is shown as raw bytes). a field is "name:type",
    "name:type[count]" or "_:padN" for N bytes of padding: the fields must cover exactly size.
    """

    def __init__(self):
        self.path = None
        self.by_name = {}
        self.by_id = {}
        self.meta = {}
        for name, (mid, size, scalar, _cmds) in MG_TABLE.items():
            spec = TypeSpec.from_type(scalar, size) if scalar else TypeSpec.raw(size)
            self.add(Entry(name, mid, 'mg', size, None, spec))

    @property
    def crc32(self):
        return self.meta.get('crc32')

    def add(self, entry):
        if entry.name in self.by_name or entry.id in self.by_id:
            raise ProtocolError(f'"{entry.name}" or its id 0x{entry.id:02X} is duplicated')
        self.by_name[entry.name] = entry
        self.by_id[entry.id] = entry

    def entry(self, vid):
        return self.by_id.get(vid)

    def name_of(self, vid):
        e = self.by_id.get(vid)
        return e.name if e else ('none' if vid == ID_NONE else f'ID#{vid}')

    def entries(self, kind):
        return sorted((e for e in self.by_id.values() if e.kind == kind), key=lambda e: e.id)

    @classmethod
    def load(cls, path):
        """raises OSError if the file cannot be read or is not TOML. a wrong entry is skipped w/ a warning"""
        proto = cls()
        proto.path = path
        with open(path, 'rb') as f:
            try:
                table = tomllib.load(f)
            except tomllib.TOMLDecodeError as e:
                raise OSError(f'{path}: {e}')
        for name, val in table.items():
            if name.startswith('_'):
                if name == '_dictionary' and isinstance(val, dict):
                    proto.meta = dict(val)
                continue
            try:
                proto.add(cls._entry_from_toml(name, val))
            except ProtocolError as e:
                print(f'[warn] {path}: {name}: {e}, ignored')
        return proto

    @staticmethod
    def _entry_from_toml(name, val):
        if not NAME_RE.fullmatch(name):
            raise ProtocolError('not a valid name')
        if not isinstance(val, dict) or 'id' not in val or 'size' not in val:
            raise ProtocolError('must be { id = .., size = .. }')
        vid, size = val['id'], val['size']
        if not isinstance(vid, int) or isinstance(vid, bool) or not 0 <= vid <= 255:
            raise ProtocolError('id must be an integer in [0, 255]')
        if not isinstance(size, int) or isinstance(size, bool) or not 0 <= size <= DATASIZE:
            raise ProtocolError(f'size must be an integer in [0, {DATASIZE}]')
        kind = kind_of(vid)
        if kind == 'mg':
            raise ProtocolError('the MG IDs are built in the tools')
        if kind == 'none':
            raise ProtocolError('id 0 is reserved')
        if kind == 'var' and size == 0:
            raise ProtocolError('a variable cannot have size 0')
        access = val.get('access')
        if access is not None and (kind != 'var' or access not in ('RO', 'RW')):
            raise ProtocolError('access must be "RO" or "RW", only for a variable')
        if sum(k in val for k in ('type', 'fields', 'arg')) > 1:
            raise ProtocolError('give only one of type, fields, arg')
        if 'arg' in val:
            if kind != 'op':
                raise ProtocolError('only an operation has arg')
            spec = TypeSpec.from_arg(val['arg'], size)
        elif 'fields' in val:
            spec = TypeSpec.from_fields(val['fields'], size)
        elif 'type' in val:
            spec = TypeSpec.from_type(val['type'], size) if size > 0 else TypeSpec.raw(0)
        else:
            spec = TypeSpec.raw(size)
        result = TypeSpec.raw(0)
        keys = [k for k in ('result', 'resultfields', 'resultsize') if k in val]
        if keys and kind != 'op':
            raise ProtocolError('only an operation has a result')
        if len(keys) > 1:
            raise ProtocolError('give only one of result, resultfields, resultsize')
        if 'result' in val:
            r = val['result']
            if not isinstance(r, str) or ':' not in r:
                raise ProtocolError(f'result "{r}" must be "name:type"')
            rname, rspec = (x.strip() for x in r.split(':', 1))
            scalar, count = TypeSpec.parse_scalar(rspec)
            result = TypeSpec.from_arg(r, SCALARS[scalar][1] * count)
        elif 'resultfields' in val:
            fl = val['resultfields']
            size_of = 0
            for it in fl if isinstance(fl, list) else []:
                spec_txt = it.split(':', 1)[1].strip() if isinstance(it, str) and ':' in it else ''
                pad = re.fullmatch(r'pad(\d+)', spec_txt)
                if pad:
                    size_of += int(pad.group(1))
                else:
                    scalar, count = TypeSpec.parse_scalar(spec_txt)
                    size_of += SCALARS[scalar][1] * count
            result = TypeSpec.from_fields(fl, size_of)
        elif 'resultsize' in val:
            rs = val['resultsize']
            if not isinstance(rs, int) or isinstance(rs, bool) or not 0 <= rs <= RESULTSIZE:
                raise ProtocolError(f'resultsize must be an integer in [0, {RESULTSIZE}]')
            result = TypeSpec.raw(rs)
        if result.size > RESULTSIZE:
            raise ProtocolError(f'a result is at most {RESULTSIZE} bytes')
        return Entry(name, vid, kind, size, access, spec, result)

    def save(self, path, source):
        """it writes the variables and the operations as a TOML protocol table"""
        width = max([len(e.name) for e in self.by_id.values() if e.kind != 'mg'] + [12])

        def line(e):
            parts = [f'id = 0x{e.id:02X}', f'size = {e.size}']
            if e.access:
                parts.append(f'access = "{e.access}"')
            t = e.spec.toml()
            if t:
                parts.append(t)
            if e.kind == 'op':
                if e.result.kind == TypeSpec.RAW and e.result.size > 0:
                    parts.append(f'resultsize = {e.result.size}')
                elif e.result.size > 0:
                    parts.append(e.result.toml('result', 'resultfields'))
            return f'{e.name:<{width}} = {{ {", ".join(parts)} }}'

        meta = ', '.join(f'{k} = 0x{v:08X}' if k in ('crc32', 'application') else f'{k} = {v}' for k, v in self.meta.items())
        stamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        lines = [
            '# amcmj1 Dictionary, the variables and the operations of the board, for tether.py (TETHER, ROP protocol 6)',
            f'# generated by tether.py from the browse of {source} on {stamp}.',
            '# tether.py rewrites it when the Dictionary of the board changes. the MG IDs are built in the tools.',
            '#',
            '# one key per ID:  IDname = { id = <ID>, size = <bytes, max 512>, access = "RO" | "RW", type = "<t>" | fields = [...] }',
            '#   variables  0x01 - 0x9F: ask, set[+ack], sig',
            '#   operations 0xA0 - 0xDF: run[+ack]. size is the size of the argument, arg = "name:<t>" its name and type,',
            '#                           or fields = [...] if it is a struct. result = "name:<t>" or resultfields = [...]',
            '#                           tell what the ack carries back, at most 36 bytes (resultsize = <bytes> if only raw bytes)',
            '#   types: bool u8 i8 u16 i16 u32 i32 u64 i64 f32 f64, also as arrays e.g. f32[3]. "_:padN" is padding',
            '',
            f'_dictionary = {{ {meta} }}',
            '',
            '# variables',
        ]
        lines += [line(e) for e in self.entries('var')]
        lines += ['', '# operations']
        lines += [line(e) for e in self.entries('op')]
        with open(path, 'w') as f:
            f.write('\n'.join(lines) + '\n')

    @staticmethod
    def usage(e):
        """how to call an operation, e.g. run[+ack]<OPsensorTRQstartTX, periodms>"""
        return f'run[+ack]<{e.name}>' if e.size == 0 else f'run[+ack]<{e.name}, {e.spec.usage()}>'

    def table(self):
        out = []
        for kind, title in (('var', 'variables'), ('op', 'operations'), ('mg', 'management')):
            out.append(f'  {title}:')
            for e in self.entries(kind):
                acc = e.access or ''
                line = f'    {e.name:<24} 0x{e.id:02X}  {e.size:3d} B  {acc:<2}  '
                if kind == 'op':
                    what = e.spec.describe() if e.size > 0 else '-'
                    if e.result.size > 0:
                        what += ' -> ' + e.result.describe()
                    line += f'{what:<40}  {self.usage(e)}'
                else:
                    line += e.spec.describe()
                out.append(line.rstrip())
        if self.meta:
            parts = []
            if 'protocol' in self.meta:
                parts.append(f'protocol {self.meta["protocol"]}')
            if 'application' in self.meta:
                parts.append(f'application {format_application(self.meta["application"])}')
            if 'crc32' in self.meta:
                parts.append(f'crc32 0x{self.meta["crc32"]:08X}')
            if 'maxframesize' in self.meta:
                parts.append(f'frames up to {self.meta["maxframesize"]} bytes')
            out.append(f'  dictionary: {", ".join(parts)}')
        return '\n'.join(out)


# ---- the codec

class Codec:
    ROP_RE = re.compile(r'^\s*(\w+)\s*(?:\[\s*([^\]]*?)\s*\])?\s*<(.*)>\s*$')

    def __init__(self, protocol):
        self.protocol = protocol
        self.maxrops = MAX_ROPS         # the smaller of the host and of the board: see set_limits()
        self.maxframesize = MAX_FRAME   # idem
        self.t0 = time.monotonic_ns()

    # ---- user text -> Rop

    def resolve_id(self, token):
        """returns (id, entry, byname). entry is None if the ID is not in the table"""
        if token in self.protocol.by_name:
            e = self.protocol.by_name[token]
            return e.id, e, True
        if not token[:1].isdigit():
            raise ProtocolError(f'unknown ID "{token}"')
        vid = parse_int(token)
        if not 0 <= vid <= 255:
            raise ProtocolError(f'ID {token} is out of range [0, 255]')
        return vid, self.protocol.entry(vid), False

    def resolve_value(self, ident):
        if ident in self.protocol.by_name:
            return self.protocol.by_name[ident].id
        raise ProtocolError(f'unknown name "{ident}" used as a value')

    @staticmethod
    def parse_options(cmd, optstr):
        opts = 0
        if not optstr:
            return opts
        for token in re.split(r'[\s,]+', optstr.strip()):
            if token not in OPTIONS:
                raise ProtocolError(f'unknown option "{token}" (known: {", ".join(OPTIONS)})')
            bit = OPTIONS[token]
            if not OPTIONS_ALLOWED.get(cmd, 0) & bit:
                raise ProtocolError(f'option {token} is not allowed on {cmd.name}')
            opts |= bit
        return opts

    @staticmethod
    def check(cmd, vid, entry, name):
        """friendly checks done by the host only for an ID given by name. a numeric ID goes as it is"""
        kind = kind_of(vid)
        if kind == 'none':
            raise ProtocolError('ID 0 accepts only ping')
        if kind == 'var':
            if cmd == CMD.run:
                raise ProtocolError(f'{name} is a variable: use ask, set or sig')
            if cmd in (CMD.set, CMD.sig) and entry is not None and entry.access == 'RO':
                raise ProtocolError(f'{name} is RO')
        elif kind == 'op':
            if cmd != CMD.run:
                raise ProtocolError(f'{name} is an operation: use run')
        elif kind == 'mg':
            allowed = MG_TABLE[name][3] if name in MG_TABLE else ()
            if cmd not in allowed:
                raise ProtocolError(f'{name} accepts only {", ".join(c.name for c in allowed)}')

    def byte_arg(self, text):
        v = parse_value(text)
        if isinstance(v, Ident):
            v = self.resolve_value(v)
        if isinstance(v, bool) or not isinstance(v, int) or not 0 <= v <= 255:
            raise ProtocolError(f'the arguments of ask are bytes or IDnames, not "{text}"')
        return v

    def parse_rop(self, text):
        m = self.ROP_RE.match(text)
        if m is None:
            raise ProtocolError(f'cannot parse "{text.strip()}", expected cmd<args> or cmd[options]<args>')
        cmdname, optstr, argstr = m.group(1).lower(), m.group(2), m.group(3).strip()
        args = split_top(argstr, ',') if argstr else []
        if any(a == '' for a in args):
            raise ProtocolError(f'empty argument in "{text.strip()}"')

        if cmdname in CMD_ALIASES:
            cmd = CMD_ALIASES[cmdname]
        elif cmdname in CMD.__members__:
            cmd = CMD[cmdname]
        else:
            raise ProtocolError(f'unknown command "{cmdname}"')

        opts = self.parse_options(cmd, optstr)

        if cmd == CMD.ping:
            if len(args) > 1:
                raise ProtocolError('ping accepts at most one argument: ping<> or ping<ID>')
            vid = self.resolve_id(args[0])[0] if args else ID_NONE
            return Rop(int(cmd), opts, vid, 0, 0, b'')

        if cmd == CMD.ask:
            if not args:
                raise ProtocolError('ask needs ask<ID> or ask<ID, byte, ...>')
            vid, entry, byname = self.resolve_id(args[0])
            if byname:
                self.check(cmd, vid, entry, args[0])
            data = bytes(self.byte_arg(a) for a in args[1:])
            if len(data) > DATASIZE:
                raise ProtocolError(f'more than {DATASIZE} arguments')
            if entry is not None and entry.kind == 'var' and FRAME_HEADER_SIZE + rop_size(entry.size) > self.maxframesize:
                raise ProtocolError(f'the say of {args[0]} ({entry.size} bytes) does not fit a frame of {self.maxframesize} bytes')
            return Rop(int(cmd), opts, vid, len(data), 0, data)

        if cmd in (CMD.set, CMD.sig, CMD.run):
            if cmd == CMD.run and len(args) == 1:
                vid, entry, byname = self.resolve_id(args[0])
                if byname:
                    self.check(cmd, vid, entry, args[0])
                    if entry.size > 0:
                        raise ProtocolError(f'{args[0]} needs its argument ({entry.spec.describe()}): '
                                            f'{Protocol.usage(entry)}')
                return Rop(int(cmd), opts, vid, 0, 0, b'')
            if len(args) == 1 and cmd in (CMD.set, CMD.sig):
                # a management ID w/out value, e.g. set<MGstreamSTOP>
                vid, entry, byname = self.resolve_id(args[0])
                if byname:
                    self.check(cmd, vid, entry, args[0])
                if entry is None or entry.kind != 'mg' or entry.size != 0:
                    raise ProtocolError(f'{cmd.name} needs {cmd.name}<ID, value> or {cmd.name}<ID, size, value>')
                return Rop(int(cmd), opts, vid, 0, 0, b'')
            if len(args) == 2:
                idtok, sizetok, valtok = args[0], None, args[1]
            elif len(args) == 3:
                idtok, sizetok, valtok = args
            else:
                raise ProtocolError(f'{cmd.name} needs {cmd.name}<ID, value> or {cmd.name}<ID, size, value>')
            vid, entry, byname = self.resolve_id(idtok)
            if byname:
                self.check(cmd, vid, entry, idtok)
            size = entry.size if entry is not None else None
            if sizetok is not None:
                s = parse_int(sizetok)
                if size is not None and s != size:
                    raise ProtocolError(f'size {s} differs from size {size} in the protocol table')
                size = s
            if size is None:
                raise ProtocolError(f'size of ID {idtok} is unknown: use {cmd.name}<ID, size, value>')
            if size > DATASIZE:
                raise ProtocolError(f'size {size} exceeds {DATASIZE}')
            if FRAME_HEADER_SIZE + rop_size(size) > self.maxframesize:
                raise ProtocolError(f'{size} bytes do not fit a frame of {self.maxframesize} bytes')
            if size == 0:
                raise ProtocolError(f'{idtok} takes no value')
            spec = entry.spec if entry is not None and entry.spec.size == size else TypeSpec.raw(size)
            data = spec.encode(parse_value(valtok), self.resolve_value)
            return Rop(int(cmd), opts, vid, len(data), 0, data)

        raise ProtocolError(f'{cmd.name} cannot be sent by the host')

    def parse_line(self, line):
        """a line w/ ROPs separated by ;. they are sent in as many frames as needed: see split()"""
        tokens = [t for t in split_top(line, ';') if t]
        rops = []
        for i, t in enumerate(tokens, start=1):
            try:
                rops.append(self.parse_rop(t))
            except ProtocolError as e:
                raise ProtocolError(f'ROP #{i}: {e}')
        return rops

    def set_limits(self, info):
        """the ROPs and the bytes per frame that the board accepts and produces (MGbrowseINFO). info = None: the host ones"""
        self.maxrops = min(MAX_ROPS, info.maxrops) if info and info.maxrops else MAX_ROPS
        self.maxframesize = min(MAX_FRAME, info.maxframesize) if info and info.maxframesize else MAX_FRAME

    def set_maxrops(self, board_maxrops):
        """the ROPs per frame the board accepts (MGbrowseINFO), 0 if unknown. kept for the tools that know only this"""
        self.maxrops = min(MAX_ROPS, board_maxrops) if board_maxrops else MAX_ROPS

    def reply_size(self, rop):
        """the bytes that the reply to rop may take in the reply frame. a ROP that expects a reply is executed by the board
        only if the biggest short reply (40 bytes) has room: so at least 40"""
        cmd = int(rop.cmd)
        if cmd == CMD.ping:
            return ROP_HEADER_SIZE
        wants = cmd == CMD.ask or (rop.opts & OPT_ACK and cmd in (CMD.set, CMD.run))
        if not wants:
            return 0
        if cmd == CMD.ask:
            entry = self.protocol.entry(rop.id)
            if entry is not None and entry.kind == 'var':
                return max(SHORT_REPLY, rop_size(entry.size))
            if is_mg(rop.id):
                return max(SHORT_REPLY, rop_size(40))      # the biggest management value: BrowseItem
            return max(SHORT_REPLY, rop_size(DATASIZE))     # unknown: the worst case
        return SHORT_REPLY

    def split(self, rops):
        """the rops in as many frames as needed, in order: in each frame at most maxrops ROPs, a request and a reply of at
        most maxframesize bytes"""
        frames, cur, req, rep = [], [], FRAME_HEADER_SIZE, FRAME_HEADER_SIZE
        for r in rops:
            if len(r.data) > DATASIZE:
                raise ProtocolError(f'{self.format_rop(r)}: {len(r.data)} bytes of data, max is {DATASIZE}')
            rq, rp = rop_size(len(r.data)), self.reply_size(r)
            if FRAME_HEADER_SIZE + rq > self.maxframesize or FRAME_HEADER_SIZE + rp > self.maxframesize:
                raise ProtocolError(f'{self.format_rop(r)} does not fit a frame of {self.maxframesize} bytes')
            if cur and (len(cur) >= self.maxrops or req + rq > self.maxframesize or rep + rp > self.maxframesize):
                frames.append(cur)
                cur, req, rep = [], FRAME_HEADER_SIZE, FRAME_HEADER_SIZE
            cur.append(r)
            req += rq
            rep += rp
        if cur:
            frames.append(cur)
        return frames

    # ---- Rop -> bytes

    def now_us(self):
        return (time.monotonic_ns() - self.t0) // 1000

    def build_frame(self, rops):
        """one frame w/ all the rops. use split() first if they may not fit one frame"""
        if len(rops) > MAX_ROPS:
            raise ProtocolError(f'{len(rops)} ROPs in a frame, max is {MAX_ROPS}')
        out = [struct.pack(FRAME_HEADER_FMT, SIGNATURE, LAYOUT_VARIABLE, len(rops), 0, self.now_us())]
        for r in rops:
            if len(r.data) > DATASIZE:
                raise ProtocolError(f'a ROP w/ {len(r.data)} bytes of data, max is {DATASIZE}')
            head = struct.pack(ROP_HEADER_FMT, make_opc(r.cmd, r.opts, r.refcmd), r.id, len(r.data))
            out.append(head + r.data.ljust(pad4(len(r.data)), b'\x00'))
        return b''.join(out)

    def build_frames(self, rops):
        """the frames of split(rops)"""
        return [self.build_frame(chunk) for chunk in self.split(rops)]

    # ---- bytes -> Frame

    @staticmethod
    def decode_frame(frame):
        """returns a Frame. raises ProtocolError if the frame as a whole is incorrect: as the board, a frame is valid
        only if its ROPs fill it exactly"""
        if len(frame) < FRAME_HEADER_SIZE:
            raise ProtocolError(f'too short: {len(frame)} bytes < header size {FRAME_HEADER_SIZE}')
        signature, layout, num, filler, t = struct.unpack_from(FRAME_HEADER_FMT, frame, 0)
        if signature != SIGNATURE:
            raise ProtocolError(f'wrong signature 0x{signature:08X} (expected 0x{SIGNATURE:08X})')
        if layout != LAYOUT_VARIABLE:
            raise ProtocolError(f'layout = {layout}: not a frame of protocol {PROTOCOL_VERSION} '
                                f'(8 ... 40 is the slot of protocol 5: update the firmware or the host)')
        if num > MAX_ROPS:
            raise ProtocolError(f'numOfROPs = {num} exceeds the max of {MAX_ROPS}')

        warnings, rops = [], []
        if filler != 0:
            warnings.append(f'[warn] filler = 0x{filler:04X} is not zero')
        off = FRAME_HEADER_SIZE
        for i in range(num):
            if off + ROP_HEADER_SIZE > len(frame):
                raise ProtocolError(f'{len(frame)} bytes, but ROP #{i + 1} of {num} starts at {off}')
            opc, vid, size = struct.unpack_from(ROP_HEADER_FMT, frame, off)
            if size > DATASIZE:
                raise ProtocolError(f'ROP #{i + 1}: size {size} exceeds {DATASIZE}')
            if off + rop_size(size) > len(frame):
                raise ProtocolError(f'ROP #{i + 1}: {size} bytes of data go past the end of the frame ({len(frame)} bytes)')
            cmd, opts, refcmd = split_opc(opc)
            data = bytes(frame[off + ROP_HEADER_SIZE: off + ROP_HEADER_SIZE + size])
            rops.append(Rop(cmd, opts, vid, size, refcmd, data))
            off += rop_size(size)
        if off != len(frame):
            raise ProtocolError(f'{len(frame)} bytes, but its {num} ROPs take {off}')
        return Frame(num, t, len(frame), rops, warnings, [])

    # ---- Rop -> text

    def format_mg(self, vid, data, refcmd=None):
        """the values of the management IDs. None if not special"""
        name_of = self.protocol.name_of
        if vid in (MG_STREAM_ADD, MG_STREAM_REM) and len(data) == 1:
            return name_of(data[0])
        if vid == MG_STREAM_START and len(data) == 2:
            ms = int.from_bytes(data, 'little')
            if refcmd is None:
                # say: the period of the stream, 0 if stopped
                return 'stopped' if ms == 0 else f'{ms} ms'
            return f'{ms} ms'
        if vid == MG_STATS and len(data) == 36:
            v = struct.unpack('<9I', data)
            return (f'rpc: {v[0]} frames rx ({v[1]} wrong), {v[2]} ROPs rx, {v[3]} replies dropped; '
                    f'stream: {v[4]} frames tx, {v[5]} read failures, {v[6]} pings, {v[7]} discarded, '
                    f'{v[8]} keepalive stops')
        if vid == MG_STREAM_LIST and len(data) >= 1:
            count = min(data[0], len(data) - 1)
            return '[' + ', '.join(name_of(x) for x in data[1:1 + count]) + ']'
        if vid == MG_VERSION and len(data) == 8:
            protocol, _r, application = struct.unpack('<HHI', data)
            return f'protocol {protocol}, application {format_application(application)}'
        if vid == MG_BROWSE_INFO and len(data) == BROWSE_INFO_SIZE:
            nv, no, mr, _r, mf, md, crc = struct.unpack(BROWSE_INFO_FMT, data)
            return (f'{nv} variables, {no} operations, max {mr} ROPs and {mf} bytes per frame, max {md} bytes of data, '
                    f'crc32 0x{crc:08X}')
        if vid == MG_BROWSE_ITEM and len(data) == BROWSE_ITEM_SIZE:
            it = unpack_item(data)
            kind = {1: 'var', 2: 'op'}.get(it.kind, f'kind#{it.kind}')
            access = {0: 'RO', 1: 'RW'}.get(it.access, '?') if it.kind == 1 else '-'
            extra = f', {it.nfields} fields' if it.nfields else ''
            if it.kind == 2:
                extra += f', result {it.resultsize} B' + (f' in {it.nresfields} fields' if it.nresfields else '')
            return f'#{it.index} {it.name}: {kind} 0x{it.id:02X}, {it.size} B, {access}, {type_name(it.type)}{extra}'
        if vid == MG_BROWSE_FIELD and len(data) == BROWSE_FIELD_SIZE:
            fd = unpack_field(data)
            spec = type_name(fd.type) + (f'[{fd.count}]' if fd.count != 1 else '')
            return f'{name_of(fd.id)} #{fd.n} {fd.name}: {spec} @ {fd.offset}'
        return None

    def format_value(self, vid, data, refcmd=None):
        if is_mg(vid):
            s = self.format_mg(vid, data, refcmd)
            if s is not None:
                return s
        entry = self.protocol.entry(vid)
        if refcmd == CMD.run:
            # the result of an operation: described by the table, else raw
            if entry is None or entry.result.size == 0 or len(data) != entry.result.size or entry.result.kind == TypeSpec.RAW:
                return format_raw(data)
            r = entry.result
            text = r.format(data)
            return f'{r.name}={text}' if (r.kind == TypeSpec.SCALAR and r.name) else text
        if entry is None:
            return format_raw(data)
        s = entry.spec.format(data)
        if len(data) != entry.size:
            s += f'   [warn] size {len(data)} differs from {entry.size} in the protocol table'
        return s

    def format_rop(self, rop):
        name = self.protocol.name_of(rop.id)
        try:
            c = CMD(rop.cmd)
        except ValueError:
            return f'[error] unknown cmd {rop.cmd} on {name}, data {format_raw(rop.data)}'
        head = f'{c.name}{format_options(rop.opts)}'

        if c in (CMD.say, CMD.set, CMD.sig):
            return f'{head}<{name}, {self.format_value(rop.id, rop.data)}>'
        if c in (CMD.ack, CMD.nak):
            ref = rop.refcmd & 0x0F
            refname = cmd_name(ref)
            head = c.name
            if rop.size == 0:
                return f'{head}<{name}, {refname}>'
            if c == CMD.nak:
                return f'{head}<{name}, {refname}, "{format_text(rop.data)}">'
            return f'{head}<{name}, {refname}, {self.format_value(rop.id, rop.data, ref)}>'
        if rop.size:
            return f'{head}<{name}, {format_raw(rop.data)}>'
        return f'{head}<{name}>'

    def flatten(self, rop):
        """[(column, value)] of a sig, for a csv file"""
        name = self.protocol.name_of(rop.id)
        entry = self.protocol.entry(rop.id)
        spec = entry.spec if entry is not None else TypeSpec.raw(len(rop.data))
        return spec.flatten(name, rop.data)


# ---- browse

BoardInfo = namedtuple('BoardInfo', 'protocol application nvars nops crc32 maxrops maxframesize maxdatasize')
BrowseItem = namedtuple('BrowseItem', 'index id kind access type nfields nresfields resultkind size resultsize name raw')
BrowseField = namedtuple('BrowseField', 'id n type count offset name raw')

BROWSE_INFO_FMT = '<BBBBHHI'        # nvars nops maxrops reserved maxframesize maxdatasize crc32
BROWSE_INFO_SIZE = struct.calcsize(BROWSE_INFO_FMT)
BROWSE_ITEM_FMT = '<8BHH28s'        # index id kind access type nfields nresfields resultkind size resultsize name
BROWSE_ITEM_SIZE = struct.calcsize(BROWSE_ITEM_FMT)
BROWSE_FIELD_FMT = '<4BHH28s'       # id n type reserved count offset name
BROWSE_FIELD_SIZE = struct.calcsize(BROWSE_FIELD_FMT)
assert (BROWSE_INFO_SIZE, BROWSE_ITEM_SIZE, BROWSE_FIELD_SIZE) == (12, 40, 36)


def _name(b):
    return b.split(b'\x00', 1)[0].decode('ascii', 'replace')


def unpack_item(data):
    index, vid, kind, access, tcode, nfields, nresfields, rkind, size, rsize, name = struct.unpack(BROWSE_ITEM_FMT, data)
    return BrowseItem(index, vid, kind, access, tcode, nfields, nresfields, rkind, size, rsize, _name(name), bytes(data))


def unpack_field(data):
    vid, n, tcode, _r, count, offset, name = struct.unpack(BROWSE_FIELD_FMT, data)
    return BrowseField(vid, n, tcode, count, offset, _name(name), bytes(data))


def _says(frame, mgid, expected):
    """the say<mgid> of a reply, which must hold exactly expected ROPs"""
    if frame is None:
        raise ProtocolError(f'no reply from the board to {expected} x ask<{mgid:#04x}>')
    says = [r for r in frame.rops if r.id == mgid and r.cmd == CMD.say]
    if len(says) != expected:
        naks = sum(1 for r in frame.rops if r.cmd == CMD.nak)
        raise ProtocolError(f'expected {expected} say<0x{mgid:02X}>, got {len(says)} ({naks} nak)')
    return says


def browse_info(transact):
    """asks MGversion and MGbrowseINFO. returns a BoardInfo, or None if the board does not answer.
    transact(list of Rop) -> Frame or None"""
    frame = transact([make_rop(CMD.ask, MG_VERSION), make_rop(CMD.ask, MG_BROWSE_INFO)])
    if frame is None:
        return None
    version = _says(frame, MG_VERSION, 1)[0].data
    if len(version) != 8:
        raise ProtocolError('MGversion has a wrong size')
    protocol, _r, application = struct.unpack('<HHI', version)
    if protocol != PROTOCOL_VERSION:
        raise ProtocolError(f'the board speaks protocol {protocol}, this host {PROTOCOL_VERSION}')
    info = _says(frame, MG_BROWSE_INFO, 1)[0].data
    if len(info) != BROWSE_INFO_SIZE:
        raise ProtocolError(f'MGbrowseINFO has {len(info)} bytes, not {BROWSE_INFO_SIZE}')
    nv, no, maxrops, _r, maxframesize, maxdatasize, crc = struct.unpack(BROWSE_INFO_FMT, info)
    return BoardInfo(protocol, application, nv, no, crc, maxrops, maxframesize, maxdatasize)


def browse(transact, info):
    """reads the whole dictionary of the board, checks it against the crc32 of MGbrowseINFO and returns a Protocol"""
    total = info.nvars + info.nops
    # every reply is at most 4 + 40 bytes (a BrowseItem): as many asks as their says fit a frame of the board
    maxframe = min(MAX_FRAME, info.maxframesize) if info.maxframesize else MAX_FRAME
    step = min(MAX_ROPS, info.maxrops if info.maxrops else MAX_ROPS, (maxframe - FRAME_HEADER_SIZE) // rop_size(BROWSE_ITEM_SIZE))
    items = []
    for start in range(0, total, step):
        idx = list(range(start, min(total, start + step)))
        frame = transact([make_rop(CMD.ask, MG_BROWSE_ITEM, bytes([i])) for i in idx])
        for r, i in zip(_says(frame, MG_BROWSE_ITEM, len(idx)), idx):
            it = unpack_item(r.data)
            if it.index != i:
                raise ProtocolError(f'MGbrowseITEM: asked #{i}, got #{it.index}')
            items.append(it)

    requests = [(it.id, n) for it in items for n in range(it.nfields + it.nresfields)]   # the fields: the ones of the argument, then the ones of the result
    fields = []
    for start in range(0, len(requests), step):
        chunk = requests[start:start + step]
        frame = transact([make_rop(CMD.ask, MG_BROWSE_FIELD, bytes(req)) for req in chunk])
        for r, (vid, n) in zip(_says(frame, MG_BROWSE_FIELD, len(chunk)), chunk):
            fd = unpack_field(r.data)
            if (fd.id, fd.n) != (vid, n):
                raise ProtocolError(f'MGbrowseFIELD: asked {vid:#04x} #{n}, got {fd.id:#04x} #{fd.n}')
            fields.append(fd)

    crc = zlib.crc32(b''.join(it.raw for it in items) + b''.join(fd.raw for fd in fields))
    if crc != info.crc32:
        raise ProtocolError(f'the crc32 of the browse is 0x{crc:08X} but MGbrowseINFO says 0x{info.crc32:08X}')

    proto = Protocol()
    for it in items:
        kind = {1: 'var', 2: 'op'}.get(it.kind)
        if kind is None or kind != kind_of(it.id):
            raise ProtocolError(f'{it.name}: kind {it.kind} does not match its id 0x{it.id:02X}')
        if kind == 'var':
            fl = [(fd.name, fd.type, fd.count, fd.offset) for fd in fields if fd.id == it.id]
            spec = TypeSpec.from_board(it.type, it.size, fl)
            access = {0: 'RO', 1: 'RW'}.get(it.access)
            result = None
        else:
            mine = sorted((fd for fd in fields if fd.id == it.id), key=lambda fd: fd.n)
            fl = [(fd.name, fd.type, fd.count, fd.offset) for fd in mine if fd.n < it.nfields]
            rl = [(fd.name, fd.type, fd.count, fd.offset) for fd in mine if fd.n >= it.nfields]
            spec = TypeSpec.from_board(it.type, it.size, fl)
            result = TypeSpec.from_result(it.resultsize, it.resultkind, rl)
            access = None
        proto.add(Entry(it.name, it.id, kind, it.size, access, spec, result))
    proto.meta = {'protocol': info.protocol, 'application': info.application, 'crc32': info.crc32,
                  'maxframesize': info.maxframesize}
    return proto


# ---- sockets

def bind_or_exit(sock, port):
    """binds the local UDP port, or exits w/ an explanation instead of a traceback"""
    try:
        sock.bind(('', port))
    except PermissionError:
        sys.exit(f'[error] cannot bind UDP port {port}: ports below 1024 need privileges on linux.\n'
                 f'        use a port >= 1024, or run once:  sudo sysctl net.ipv4.ip_unprivileged_port_start={port}')
    except OSError as e:
        sys.exit(f'[error] cannot bind UDP port {port}: {e}')
