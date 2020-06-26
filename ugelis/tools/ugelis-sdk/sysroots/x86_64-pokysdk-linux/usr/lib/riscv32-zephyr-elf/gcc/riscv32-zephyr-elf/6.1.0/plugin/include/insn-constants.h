/* Generated automatically by the program `genconstants'
   from the machine description file `md'.  */

#ifndef GCC_INSN_CONSTANTS_H
#define GCC_INSN_CONSTANTS_H

#define RETURN_ADDR_REGNUM 1
#define T1_REGNUM 6
#define T0_REGNUM 5

enum unspec {
  UNSPEC_EH_RETURN = 0,
  UNSPEC_ADDRESS_FIRST = 1,
  UNSPEC_PCREL = 2,
  UNSPEC_LOAD_GOT = 3,
  UNSPEC_TLS = 4,
  UNSPEC_TLS_LE = 5,
  UNSPEC_TLS_IE = 6,
  UNSPEC_TLS_GD = 7,
  UNSPEC_AUIPC = 8,
  UNSPEC_GPR_SAVE = 9,
  UNSPEC_GPR_RESTORE = 10,
  UNSPEC_BLOCKAGE = 11,
  UNSPEC_FENCE = 12,
  UNSPEC_FENCE_I = 13,
  UNSPEC_COMPARE_AND_SWAP = 14,
  UNSPEC_SYNC_OLD_OP = 15,
  UNSPEC_SYNC_EXCHANGE = 16,
  UNSPEC_ATOMIC_STORE = 17,
  UNSPEC_MEMORY_BARRIER = 18
};
#define NUM_UNSPEC_VALUES 19
extern const char *const unspec_strings[];

#endif /* GCC_INSN_CONSTANTS_H */
