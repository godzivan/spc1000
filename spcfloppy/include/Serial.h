
#ifndef SERIAL_H
#define SERIAL_H
#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;

public:
	//Initialize Serial communication with the given COM port
	Serial(void);
	void ConnectSerial(LPCSTR portName);
	//Close the connection
	//NOTA: for some reason you can't connect again before exiting
	//the program and running it again
	~Serial();
	//Read data in a buffer, if nbChar is greater than the
	//maximum number of bytes available, it will return only the
	//bytes available. The function return -1 when nothing could
	//be read, the number of bytes actually read.
	int ReadData(char *buffer, unsigned int nbChar);
	//Writes data from a buffer through the Serial connection
	//return true on success.
	bool WriteData(char *buffer, unsigned int nbChar);
	void  WriteShortData(unsigned short u16);
	void WriteByteData(unsigned char b );
	void WriteString(const char *str);
	void WriteIntData(unsigned int val);
	void WriteIntDataArray(int *val, int sz);
	//Check if we are actually connected
	bool IsConnected();
	void flush(void);

};

#endif // SERIALCLASS_H_INCLUDED
