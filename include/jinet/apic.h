/**
 * @file
 * @brief Local APIC interactions.
 */

#ifndef APIC_H
#define APIC_H

#include <stdint.h>

/**
 * @brief      Sets up LAPIC.
 */
void lapic_setup();

#define LAPIC_ID_N 0x2
#define EOIR_N 0xB
#define SVR_N 0xF

#define LAPIC_REG_A_FIRST 0
#define LAPIC_REG_B_FIRST 1

/**
 * @brief      Reads from a LAPIC register
 *
 * @param[in]  n  Number of a register.
 *
 * @return     32-bit value of a reg.
 */
uint32_t lapic_reg_read(int n);

/**
 * @brief      Writes to a LAPIC register
 *
 * @param[in]  n     Number of a register
 * @param[in]  val   The value
 */
void lapic_reg_write(int n, uint32_t val);

/**
 * @brief      Sends EOI-notification to LAPIC.
 * 
 * Basically writes zero to EOI-register. Also check out ls_eoi in lapic.asm which is written for _fast_ sending eoi.
 */
void lapic_eoi_send();
#endif