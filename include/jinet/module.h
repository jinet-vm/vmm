/**
 * @file
 * @brief A very simple module-like output system.
 */

#ifndef MODULE_H
#define MODULE_H

// [MODULE] very important info
// [MODULE] (file_name.c : LN) very important info


/**
 * @brief      prints [m] pRiNtF_fOrMaTeD_StRiNg
 * Go away. It's bad in there.
 *
 * @param      m          Module name
 * @param      fmt        The format
 * @param[in]  <unnamed>  va_args!
 */
void _mprint(char* m, char* fmt, ...);


/**
 * @brief      Defines a module name for a file
 *
 * @param      name  The name
 */
#define MODULE(name) static char* _MODULE_NAME = name

/**
 * @brief      A module-in-mind interface to printf
 *
 * @param      ...   { parameter_description }
 *
 * @return     { description_of_the_return_value }
 */
#define mprint(...) _mprint(_MODULE_NAME,__VA_ARGS__)

#endif