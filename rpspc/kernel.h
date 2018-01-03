//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _kernel_h
#define _kernel_h

#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/types.h>
#include <circle/screen.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

	TShutdownMode Run (void);

private:
	// do not change this order
	int getargs(int i)
	{
		//int cparams[256] = {0,4,4,0,7,1,1,0,4,4,0,4,4,2,6,6,0};
		int val = 0;
		switch (i)
		{
			case 1:
			case 2:
			case 8:
			case 9:
				val = 4;
				break;
			case 5:
			case 6:
				val = 1;
				break;
			case 10:
				val = 2;
				break;
			case 11:
			case 12:
				val = 6;
				break;
			case 4:
				val = 7;
				break;
		}
		return val;
	}
	CMemorySystem	m_Memory;
	CActLED		m_ActLED;
	//CKernelOptions		m_Options;	
	//CScreenDevice		m_Screen;
	//CLogger			m_Logger;	
	//CExceptionHandler	m_ExceptionHandler;
	//CInterruptSystem	m_Interrupt;
	//CTimer			m_Timer;
};

#endif
