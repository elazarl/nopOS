#!/usr/bin/python

import ctypes

c_uint8 = ctypes.c_uint8
c_uint32 = ctypes.c_uint32
c_uint64 = ctypes.c_uint64


class Bitfield(object):
    def __init__(self, data=0):
        nbits = 0
        self.__dict__['_m'] = {}
        self.__dict__['_data'] = data
        for field, bits in self._fields_:
            self._m[field] = (nbits, bits)
            nbits += bits
        self.__dict__['_nbits'] = nbits

    def __getattr__(self, attr):
        nbits, bits = self._m[attr]
        return (self._data >> nbits) & ((1 << bits)-1)

    def __setattr__(self, attr, v):
        nbits, bits = self._m[attr]
        self.__dict__['_data'] = self._data & ~(((1 << bits)-1) << nbits) | (v << nbits)

    def aslong(self):
        return self._data

class Bitfield64bits(Bitfield):
    def __init__(self, data=0):
        super(Bitfield64bits, self).__init__(data)
        if self._nbits != 64:
            raise Exception('A class was defined with != 64 bits: %d' % self._nbits)


class PTE(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("dirty", 1),
            ("pat", 1),
            ("global", 1),
            ("ignored", 3),
            ("addr", 40),
            ("ignored2", 7),
            ("protectionkey", 4),
            ("xd", 1),
        ]

class PDE(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("ignored", 1),
            ("ps", 1),
            ("ignored2", 4),
            ("addr", 40),
            ("ignored3", 11),
            ("xd", 1),
        ]

class PDE2m(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("dirty", 1),
            ("ps", 1),
            ("global", 1),
            ("ignored", 3),
            ("pat", 1),
            ("reserved", 8),
            ("addr", 31),
            ("ignored2", 7),
            ("protectionkey", 4),
            ("xd", 1),
        ]

PDE(), PDE2m(), PTE()

def parsepde(data):
    pde = PDE(data)
    if pde.ps:
        return PDE2m(data)
    return pde

class PDPTE(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("ignored", 1),
            ("ps", 1),
            ("ignored2", 4),
            ("addr", 40),
            ("ignored3", 11),
            ("xd", 1),
        ]

class PDPTE1g(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("dirty", 1),
            ("ps", 1),
            ("global", 1),
            ("ignored", 3),
            ("pat", 1),
            ("reserved", 17),
            ("addr", 22),
            ("ignored2", 7),
            ("protectionkey", 4),
            ("xd", 1),
        ]

def parsepdpte(data):
    p = PDPTE(data)
    if p.ps:
        return PDPTE1g(data)
    return p



class PML4(Bitfield64bits):
    _fields_ = [
            ("present", 1),
            ("readwrite", 1),
            ("usersupervisor", 1),
            ("pwt", 1),
            ("pcd", 1),
            ("accessed", 1),
            ("ignored", 1),
            ("ps", 1), # must be 0
            ("ignored2", 4),
            ("addr", 40),
            ("ignored3", 11),
            ("xd", 1),
        ]

if __name__ == '__main__':
    p = PTE(0xf0f)
    p.xd = 1
    p.ignored2 = 0x3
    print p.xd
    s = 0
    for f in p._fields_:
        s += f[1]
        print f[0], hex(getattr(p, f[0])), bin(getattr(p, f[0]))
    print s

