#ifndef BASICMM_H
#define BASICMM_H

#include <stdint.h>

// only for grub trampoline

/**
 * @brief      Add a new region to basicmm
 *
 * @param[in]  begin   The begin address
 * @param[in]  end  The end address
 */
void basicmm_add(uint32_t begin, uint32_t end);

/**
 * @brief      allocates memory
 *
 * @param[in]  size  The address
 *
 * @return     ptr to allocated memory
 */
void* basicmm_alloc(uint32_t size);

#endif