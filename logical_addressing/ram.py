RAM_SIZE = 1024


class RAM:
    def __init__(self, size=RAM_SIZE):
        self._minAddr = 0
        self._maxAddr = RAM_SIZE - 1
        self._memory = []   # a list of values.  Could be #s or instructions.
        for i in range(size):
            self._memory.append(0)

    def __getitem__(self, addr):
        return self._memory[addr]

    def __setitem__(self, addr, val):
        self._memory[addr] = val

    def is_legal_addr(self, addr):
        return self._minAddr <= addr <= self._maxAddr

class MMU:
    def __init__(self, ram):
        self._ram = ram
        self.reloc = 0 # relocation register
        self.limit = 0 # limit register


    # get_memory() gets the physical memory location at the translated logical address passed to value (if valid)
    # valid addr: self.reloc <= valid addr < self.reloc + self.limit
    def get_memory(self, addr):
        if addr < self.limit:
            if addr >= 0:
                return self._ram.__getitem__(addr + self.reloc)
            else:
                print("BAD ADDRESS in ram.get_memory, {} too low".format(addr))
        else:
            print("BAD ADDRESS in ram.get_memory, {} too high".format(addr))


    # set_memory() sets the physical memory location at the translated logical address passed to value (if valid)
    # self.reloc <= valid addr < self.reloc + self.limit
    def set_memory(self, addr, value):
        if addr < self.limit:
            if addr >= 0:
                return self._ram.__setitem__(addr + self.reloc, value)
            else: 
                print("BAD ADDRESS in ram.set_memory, {} too low".format(addr))
        else: 
            print("BAD ADDRESS in ram.set_memory, {} too high".format(addr))


    def set_reloc_register(self, addr):
        if addr >= 0:
            self.reloc = addr
    
    def set_limit_register(self, addr):
        if addr >= 0:
            self.limit = addr

