#ifndef MODULE_H
#define MODULE_H

// [MODULE] very important info
// [MODULE] (file_name.c : LN) very important info

void _mprint(char* m, char* fmt, ...);

#define MODULE(name) static char* _MODULE_NAME = name
#define mprint(...) _mprint(_MODULE_NAME,__VA_ARGS__)
#define panic(...) {mprint(__VA_ARGS__); mprint("KERNEL PANIC: %s:%d, func %s", __FILE__, __LINE__, __func__); for(;;);}

#endif