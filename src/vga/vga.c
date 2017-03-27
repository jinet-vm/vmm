/**
 * @file vga.c
 * @brief Interface to VGA text mode.
 */
#include <kernel/vga.h>
#include <kernel/io.h>
#include <kernel/memory.h>

volatile uint16_t* vga_buffer;

/**
 * @brief      Initialize VGA.
 */
void vga_init()
{
	vga_buffer = 0xB8000;
	vga_clear();
	vga_set_cursor(0,0);
}

/**
 * @brief      Scroll one row in VGA buffer.
 */
void vga_scroll_row()
{
	size_t i; uint16_t entry;
	for(char y = 1; y < VGA_HEIGHT; y++)
		for(char x = 0; x<VGA_WIDTH;x++)
		{
			i = y*VGA_WIDTH+x;
			entry = vga_buffer[i];
			i -= VGA_WIDTH;
			vga_buffer[i] = entry;
		}
}


/**
 * @brief      Set cursor.
 *
 * @param[in]  row   The row of the cursor
 * @param[in]  col   The column of the cursor
 */
void vga_set_cursor(int row, int col)
{
    unsigned short position = (row * VGA_WIDTH) + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

/**
 * @brief      Puts a char to VGA buffer
 *
 * @param[in]  c      The char
 * @param[in]  color  The color
 * @param[in]  x      The row of the cursor
 * @param[in]  y      The column of the cursor
 */
void vga_putc(char c, uint8_t color, int x, int y)
{
	vga_buffer[y*VGA_WIDTH+x] = vga_entry(c,color);
}

/**
 * @brief      Clears everything in VGA buffer.
 */
void vga_clear()
{
	memset(vga_buffer,0,VGA_WIDTH*VGA_HEIGHT*2);
}