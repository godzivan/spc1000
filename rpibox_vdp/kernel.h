//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
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
#include <circle/devicenameservice.h>
#include "screen8.h"
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>
#include <circle/multicore.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>
#include "tms9918.h"


#ifdef ARM_ALLOW_MULTI_CORE
class MyMulticore 
: public CMultiCoreSupport 
{
	tms9918 vdp;
public:	
	MyMulticore (CMemorySystem *pMemorySystem)
	: CMultiCoreSupport (pMemorySystem)
	{}
	boolean Initialize(tms9918 vdp)
	{
		this->vdp = vdp;
		return CMultiCoreSupport::Initialize();
	}
	void Run (unsigned nCore)
	{
		if (nCore == 1)
		{
			int time = 0;
			while(true)
			{
				time++;
				if (time > (int)(250000000/30/261))
				{
					tms9918_periodic(vdp);
					time = 0;
				};
			}
		}
	}
};
#endif
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

	void SetPalette(u8 num, u16 color)
	{
		m_Screen.SetPalette(num, color);
	}
	void SetPalette(u8 num, u32 color)
	{
		m_Screen.SetPalette(num, color);
	}
	void UpdatePalette(void)
	{
		m_Screen.UpdatePalette();
	}
	TScreenColor *GetBuffer(void)
	{
		return m_Screen.GetBuffer();
	}
	int printf(const char *format, ...)
	{
		CString str;
		va_list argptr;
		va_start(argptr, format);
		str.FormatV(format, argptr);
		va_end(argptr);
		m_Screen.Write(str, str.GetLength());		
		return 0;
	}
private:
	// do not change this order
	CMemorySystem		m_Memory;
	CActLED			m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CScreenDevice8		m_Screen;
	CSerialDevice		m_Serial;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CLogger			m_Logger;
	CEMMCDevice		m_EMMC;
	FATFS			m_FileSystem;

	//CMandelbrotCalculator	m_Mandelbrot;
	MyMulticore		m_Core;
	tms9918 vdp;
};

#endif
