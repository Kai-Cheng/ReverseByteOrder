// swap.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>
#include <windows.h>

#if 0
const char *get_filename_ext(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}

int get_filename_extT_(_TCHAR* filename) {
	_TCHAR *dot = _tcsrchr(filename, _T('.'));
	if (!dot || dot == filename) return NULL;
	return (dot - filename);
}
#endif

_TCHAR *get_filename_extT(_TCHAR* filename) {
	_TCHAR *dot = _tcsrchr(filename, _T('.'));
	if (!dot || dot == filename) return NULL;
	return dot + 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *pfile;
	unsigned long  datasize, retSize, i;
	TCHAR newDir[MAX_PATH*10];
	TCHAR *pDir;
	DWORD dwError = 0;
	DWORD currentPahtlength = 0;
	char revBytes = 0;
	char cTemp0 = 0;
	char cTemp1 = 0;
	char cTemp2 = 0;

	// If the directory is not specified as a command-line argument,
	// print usage.

#if 0//[.4test.
	argc = 3;
	argv[0] = _T("swap.exe");
	//argv[1] = _T("E:\\n\\LCD\\圖片_2017-03-15\\圖示列\\20170327_PRINT X OFFS\\PRINT X OFFS 2 W214& H36PX_RGB565\\PRINT X OFFS 2 W214& H36PX.raw");
	argv[1] = _T("PRINT X OFFS 2 W214& H36PX.raw");
	argv[2] = _T("2");
	if (argc != 3)
	{
		_tprintf(TEXT("\nUsage: %s <file path> <reverse bytes, 2 or 4>\n"), argv[0]);
		return (-1);
	}
	if (_tcscmp(argv[2], _T("2")) && _tcscmp(argv[2], _T("4")))
	{
		printf("Command error: reverse bytes error!\r\n");
		_tprintf(TEXT("\nUsage: %s <file path> <reverse bytes, 2 or 4>:%s\n"), argv[0], argv[2]);
		return (-1);
	}
	LPWSTR pPathBuf = new TCHAR[100];
	pPathBuf = argv[1];
#else
	if (argc == 2 && (!_tcscmp(argv[1], _T("-?")) || !_tcscmp(argv[1], _T("-h")) || !_tcscmp(argv[1], _T("-help"))))
	{
		printf("Version 1.00\r\n");
		_tprintf(TEXT("\nUsage: %s <file path> <reverse bytes, 2 or 4>\n"), argv[0]);
		//printf("This program built for Win8.1x64\n");
		printf("Report error to kai.cheng.wang@gmail.com\n");
		return 0;
	}
	if (argc != 3)
	{
		printf("Command error!\r\n");
		_tprintf(TEXT("\nUsage: %s <file path> <reverse bytes, 2 or 4>\n"), argv[0]);
		return (-1);
	}
	if (_tcscmp(argv[2], _T("2")) && _tcscmp(argv[2], _T("4")))
	{
		printf("Command error: reverse bytes error!\r\n");
		_tprintf(TEXT("\nUsage: %s <file path> <reverse bytes, 2 or 4>:%s\n"), argv[0], argv[2]);
		return (-1);
	}
	currentPahtlength = GetCurrentDirectory(0, NULL);
	currentPahtlength += 1;

	LPWSTR pPathBuf = new TCHAR[currentPahtlength];
	dwError = GetCurrentDirectory(currentPahtlength, pPathBuf);
	if (dwError == 0)
	{
		dwError = GetLastError();
		if (pPathBuf) delete[] pPathBuf;
		return dwError;
	}
#endif//].

	if (!_tcscmp(argv[2], _T("2")))
	{
		revBytes = 2;
	}
	else if (!_tcscmp(argv[2], _T("4")))
	{
		revBytes = 4;
	}
	_tcscpy(newDir, argv[1]);
	pDir = get_filename_extT(newDir);
	pDir--;
	_tcscpy(pDir, _T("_R."));
	pDir = _tcsrchr(pDir, _T('.'));
	pDir++;
	_tcscpy(pDir, get_filename_extT(argv[1]));

	// Open file and reading data to pBuf
	dwError = _wfopen_s(&pfile, argv[1], _T("rb"));
	if (pfile == NULL)
	{
		_tprintf(TEXT("\nOpen Error:%s, %s\n"), argv[1], TEXT("rb"));
		return (-1);
	}

	fseek(pfile, 0, SEEK_END);
	datasize = ftell(pfile);
	if (datasize % revBytes)
	{
		printf("\nData size cannot align by %d. Total:%d\n", revBytes, datasize);
		return (-1);
	}

	unsigned char *pbuf = new unsigned char[datasize + 4];
	if (pbuf == NULL)
	{
		printf("Malloc memory fail.");
		return (-1);
	}

	fseek(pfile, 0, SEEK_SET);
	retSize = fread(pbuf, sizeof(unsigned char), datasize, pfile);
	fclose(pfile);
	if (retSize != datasize) {

		delete[] pbuf;
		pbuf = NULL;
		printf("Read file fail.");
		return (-1);
	}
	// TODO:Reverse the byte order
	if (revBytes == 2)
	{
		for (i = 0; i < datasize; i += 2)
		{
			cTemp0 = pbuf[i];
			pbuf[i] = pbuf[i + 1];
			pbuf[i + 1] = cTemp0;
		}
	}
	else
	{
		for (i = 0; i < datasize; i += 4)
		{
			cTemp0 = pbuf[i];
			cTemp1 = pbuf[i + 1];
			cTemp2 = pbuf[i + 2];
			pbuf[i] = pbuf[i + 3];
			pbuf[i + 1] = cTemp2;
			pbuf[i + 2] = cTemp1;
			pbuf[i + 3] = cTemp0;
		}
	}

	/* open file with w+b and write pBuf to file */
	//dwError = _wfopen_s(&pfile, argv[1], _T("w+b"));
	dwError = _wfopen_s(&pfile, newDir, _T("w+b"));

	if (pfile == NULL)
	{
		delete[] pbuf;
		pbuf = NULL;
		_tprintf(TEXT("\nOpen Error:%s, %s\n"), argv[1], TEXT("w+b"));
		return (-1);
	}
	retSize = fwrite(pbuf, sizeof(unsigned char), datasize, pfile);
	fclose(pfile);

	if (retSize != datasize)
	{
		delete[] pbuf;
		pbuf = NULL;
		printf("Write file fail.");
		return (-1);
	}

	if (pbuf)
	{
		delete[] pbuf;
		pbuf = NULL;
	}
	printf("Convert complete!");
	return 0;
}
