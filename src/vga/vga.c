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
	for(int x = 0; x < VGA_WIDTH; x++)
	{
		for(int y = 1; y < VGA_HEIGHT; y++)
			vga_buffer[(y-1)*VGA_WIDTH+x]=vga_buffer[y*VGA_WIDTH+x];
	}
	memset(vga_buffer+(VGA_HEIGHT-1)*VGA_WIDTH,0,4*VGA_WIDTH);
	//vga_set_cursor(0,VGA_HEIGHT-2);
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