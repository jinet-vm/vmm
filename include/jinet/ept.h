#ifndef EPT_H
#define EPT_H

#include <stdint.h>

typedef struct ept_entry
{
	union
	{
		struct
		{
			/// @brief Read access
			char r:1;
			/// @brief Write access
			char w:1;
			/// @brief Execute access for supervisor-mode
			char x:1;
			/// @brief EPT memory type
			char ept_mt:3;
			/// @brief Ignore PAT
			char ipat:1;
			/// @brief 1 if maps a physical page; 0 if referenced ept table
			char mr:1;
			/// @brief Accessed flag
			char a:1;
			/// @brief Dirty flag
			char d:1;
			/// @brief Execute access for usermode
			char xu:1;
			/// @brief Reserved/ignored
			char _0:1;
			/// @brief Cut address of referenced table or mapped page
			uint64_t addr_cut:36;
			/// @brief Reserved/ignored
			uint16_t _1:15;
			/// @brief Suppress #VE 
			char sve:1;
		}
		uint64_t raw;
	}
} ept_entry_t;

typedef struct eptp
{
	/// @brief EPT paging structures memory type
	char ept_ps_mt:3;
	/// @brief This value is 1 less than the EPT page-walk length
	char ept_pw_ln:3;
	/// @brief Enables (if 1) accessed/dirty flags
	char :1;
	/// @brief reserved
	char _0:5;
	/// @brief Cut address of referenced PML4
	uint64_t addr_cut:36;
	/// @brief reserved
	uint16_t _1:16;
} eptp_t;

eptp_t ept_init();

#endif