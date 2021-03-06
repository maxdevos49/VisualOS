/*
 * File:		bootloader.h
 * Description:	Interface for bootloader and kernel coupling
 * *****************************************************************************
 * Copyright 2020 Scott Maday
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see https://www.gnu.org/licenses/gpl-2.0
 */

#pragma once

#define PSF1_MAGIC0	0x36
#define PSF1_MAGIC1	0x04
#define PSF1_WIDTH	8
#define BMP_MAGIC0	0x42	// B
#define BMP_MAGIC1	0x4D	// M

#define BOOTLOADER_MEMORY_PAGE_SIZE	4096

// gop uses alpha(0x00=transparent), red, green, blue
struct FrameBuffer {
	void*				base_ptr;
	unsigned long long	size;		// size of buffer
	unsigned int		width;
	unsigned int		height;
	unsigned int		ppsl;		// pixels per scanline
};

struct PSF1Header {
	unsigned char	magic[2];	// identifier bytes
	unsigned char	mode;
	unsigned char	charsize;	// how large the characters are
} __attribute__((__packed__));

struct PSF1Font {
	struct PSF1Header*	header_ptr;
	void*				glyph_buffer;
};

struct TGAHeader {
	unsigned char	id_length;
	unsigned char	color_map_type;
	unsigned char	image_type;
	unsigned short	color_map_index;
	unsigned short	color_map_length;
	unsigned char	depth;
	unsigned short	xorg;				// x origin
	unsigned short	yorg;				// y origin
	unsigned short	width;
	unsigned short	height;
	unsigned char	bbp;				// bits per pixel
	unsigned char	descriptor;
} __attribute__((__packed__));

struct TGAImage {
	struct TGAHeader*	header_ptr;
	void*				buffer;
};


enum MemoryType {
	RESERVED_MEMORY_TYPE,
	LOADER_CODE,
	LOADER_DATA,
	BOOT_SERVICES_CODE,
	BOOT_SERVICES_DATA,
	RUNTIME_SERVICES_CODE,
	RUNTIME_SERVICES_DATA,
	CONVENTIONAL_MEMORY,
	UNUSABLE_MEMORY,
	ACPI_RECLAIM_MEMORY,
	ACPI_MEMORYNVS,
	MEMORY_MAPPED_IO,
	MEMORY_MAPPED_IO_PORT_SPACE,
	PAL_CODE,
};

struct MemoryDescriptor {
	enum MemoryType		type;			// type of memory segment
	void*				phys_address;
	void*				virt_address;
	unsigned long long	num_pages;
	unsigned long long	attribs;
};

struct KernelEntryInterface {
	struct FrameBuffer*			frame_buffer_ptr;
	struct PSF1Font*			font_ptr;
	struct TGAImage*			img_ptr;
	struct MemoryDescriptor*	mem_map;
	unsigned long int 			mem_map_size;
	unsigned long int 			mem_map_descriptor_size;
};
