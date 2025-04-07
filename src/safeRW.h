/**
 * @file safeRW.h
 * @author CreepNT
 * @brief API header for the SafeRW utility
 */

#include <stdint.h>

/**
 * @brief MemAccessError area bounds (for registration with SceExcpmgr)
 */
extern char _safeRWRegionStart;
extern char _safeRWRegionEnd;

/**
 * @brief Read value from an arbitrary (and potentially invalid) address
 *
 * @param addr  Address from which value should be read
 * @param val   Receives the value read from @p addr
 * @returns 0 on success, <0 on error
 *
 * @warning In case of error, no value is written to @p{val}!
 */
extern int safeRead8(uint8_t* addr, uint8_t* val);
extern int safeRead16(uint16_t* addr, uint16_t* val);
extern int safeRead32(uint32_t* addr, uint32_t* val);

/**
 * @brief Write value at an arbitrary (and potentially invalid) address
 *
 * @param addr  Address where value should be written
 * @param val   Value to write at @p addr
 * @returns 0 on success, <0 on error
 */
extern int safeWrite8(uint8_t* addr, uint8_t val);
extern int safeWrite16(uint16_t* addr, uint16_t val);
extern int safeWrite32(uint32_t* addr, uint32_t val);