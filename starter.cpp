//////////////////////////////////////////////////////////////////////////
//
// starter - with this small application you can load all kind of
//           code fragments stored in binary files and run them
//           I'm using it to load & test output code from the polymorphic
//           engine
//
// Bartosz Wójcik
// http://www.pelock.com
//
//////////////////////////////////////////////////////////////////////////

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main(int argc, char* argv[])
{
	printf("[i] starter by Bartosz Wójcik - www.pelock.com\n");

	// check number of parameters
	if (argc != 2)
	{
		printf("[!] usage %s filename", argv[0]);
		getch();

		return 1;
	}

	// open input file (read only mode, we don't need anything more)
	HANDLE hFile = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("[!] cannot open %s file\n", argv[1]);
		getch();

		return 2;
	}

	printf("[i] file %s opened successfully\n", argv[1]);

	// check file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	if (dwFileSize != 0)
	{
		printf("[i] filesize = %u bytes (%u kB)\n", dwFileSize, dwFileSize / 1024);

		// allocate virtual memory with executable rights
		PBYTE pbFileContents = (PBYTE)VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (pbFileContents != NULL)
		{
			printf("[i] memory allocated at %08X\n", pbFileContents);

			DWORD dwWritten = 0;

			if (ReadFile(hFile, pbFileContents, dwFileSize, &dwWritten, NULL) != 0)
			{
				printf("[i] file successfully read\n");

				printf("[*] executing file contents (catch int3 with debugger)\n");

				// prepare execution
				__asm
				{
					// save all registers state
					pushad

					// put file contents pointer to EAX register
					mov eax,pbFileContents

					// trigger int3 (catch it with debugger)
					int 3

					// step into the file contents code
					call eax

					// restore all registers
					popad
				}
			}
			else
			{
				printf("[!] cannot read %s file\n", argv[1]);
				getch();
			}

			VirtualFree(pbFileContents, 0, MEM_RELEASE);
		}
		else
		{
			printf("[!] cannot allocate %u bytes (%u KB) of memory\n", dwFileSize, dwFileSize / 1024);
			getch();
		}
	}
	else
	{
		printf("[!] file %s is empty\n", argv[1]);
		getch();
	}

	CloseHandle(hFile);

	return 0;
}
