// ==============================================================================
/**
 * mmu.c
 */
// ==============================================================================



// ==============================================================================
// INCLUDES

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mmu.h"
#include "vmsim.h"

// ==============================================================================



// ==============================================================================
// MACROS AND GLOBALS
static u_int32_t upt_end_index = 10;
static uint32_t lpt_end_index = 20;
static u_int32_t pt_end_index = 32;
/** 
 * The (real) address of the upper page table.  Initialized by a call
 * to `mmu_init()`.
 */
static vmsim_addr_t upper_pt_addr = 0;
// ==============================================================================



// ==============================================================================
void
mmu_init (vmsim_addr_t new_upper_pt_addr) {

  upper_pt_addr = new_upper_pt_addr;
  
}
// ==============================================================================


/**
 * @brief isolates a specific number of bits and returns their value.
 *  Works by creating a value of 32 ones. It then shifts that value to the left 
 *  based on how many bits we are isolating. Then it takes the address and shifts 
 *  it right until the least significant digit of the bits we want is the
 *  least significant digit of the address. 
 * 
 *  It then nots the bitmask, which means that everything will be zero
 *  except for the numBits zereos we added that will 
 *  become one now. Lastly it ANDs both results together and returns.  
 * @param address the address we are isolating bits from.
 * @param startIndex the index we want to start isolating from (inclusive).
 * @param endIndex the index where we are stopping the bit isolation(exclusive).
 * @return int the value of the bits we returned.
 * @throw an error if the passed in end index is more than the size of a 32 bit int. 
 */
uint32_t getBits(vmsim_addr_t address, uint32_t startIndex, uint32_t endIndex){
  assert((endIndex <= sizeof(uint32_t) * 8) & (endIndex > startIndex) & (startIndex >=0));
  // calculate how many bits we need to get
  uint32_t numBits = endIndex-startIndex;
  // create an integer of 32 ones.
  uint32_t fullInteger = 0xFFFFFFFF;
  // insert numBits zereos at the least significant digits of the full integer.
  uint32_t bitMask = fullInteger<<numBits;
  return (address >> (32-endIndex)) & (~bitMask);
}

/**
 * @brief checks if the mapping of the passed in entry exists or not.
 * if it doesn't exist, it creates it.
 * 
 * @param address of the object passed in.
 * @return valid address that is created. 
 */


// ==============================================================================
vmsim_addr_t
mmu_translate (vmsim_addr_t sim_addr) {
  // isolate first ten bits 
  u_int32_t upper_pt_index = getBits(sim_addr, 0, upt_end_index);
  // isolate second ten bits
  printf("size %d\n", sizeof(u_int32_t));
  uint32_t lower_pt_index = getBits(sim_addr, upt_end_index, lpt_end_index);
  // isolate the the last 12 bits
  uint32_t page_offset = getBits(sim_addr, lpt_end_index, pt_end_index);

  pt_entry_t lower_pt_sim = upper_pt_addr+ (sizeof(vmsim_addr_t)*upper_pt_index);
  // fetch the lower page table.
  vmsim_addr_t lpt_base; 
  vmsim_read_real(&lpt_base,lower_pt_sim,sizeof(vmsim_addr_t));
  // ensure the lower page table exists
  if (lpt_base == 0){
    vmsim_map_fault(sim_addr);
    vmsim_read_real(&lpt_base,lower_pt_sim,sizeof(vmsim_addr_t));
    assert(lpt_base != 0);
  }
  // get the page entry from the LPT.
  pt_entry_t page_sim = lpt_base+ (sizeof(vmsim_addr_t)*lower_pt_index);
  // fetch the page 
  vmsim_addr_t page_base; 
  vmsim_read_real(&page_base,page_sim,sizeof(vmsim_addr_t));
  // ensure the page exists.
  if (page_base == 0){
    vmsim_map_fault(sim_addr);
    vmsim_read_real(&page_base,page_sim,sizeof(vmsim_addr_t));
    assert(page_base!=0);
  }
  // get the real address.
  vmsim_addr_t real_address = page_base + page_offset;
  printf("The simulated address %x is translated into the real address %x \n\n", sim_addr, real_address);
  return real_address;
}
// ==============================================================================
