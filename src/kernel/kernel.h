/*
 * File:		kernel.h
 * Description:	Kernel-specific and auto-included definitions
 * *****************************************************************************
 * Copyright 2020 Scott Maday
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see https://www.gnu.org/licenses/gpl-2.0
 */

#pragma once

#define NULL	0


// From kernel.ld linker script
extern void** _kernel_start;
extern void** _kernel_end;


// Porta-debug
/*
#include <string.h>
#include "shell/shell.h"
#include "shell/tools.h"
*/