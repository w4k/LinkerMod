#include "stdafx.h"

//
// COMMAND LINE: launcher_ldr DLL EXE ARGUMENTS
//

char g_Directory[MAX_PATH];
char g_ExeDirectory[MAX_PATH];
char g_CommandLine[8192];

HANDLE InjectDll(HANDLE ProcessHandle, const char *Path, const char *Module);

bool StrDel(char* Source, char* Needle, char StopAt)
{
	// Find the location in the string first
	char* loc = strstr(Source, Needle);

	if (!loc)
		return false;

	// "Delete" the word by shifting it over
	int needleLen = strlen(Needle);

	memcpy(loc, loc + needleLen, strlen(loc) - needleLen + 1);

	return true;
}

void FixCommandLine(int argc, char *argv[])
{
	// Get the current directory
	GetCurrentDirectoryA(ARRAYSIZE(g_Directory), g_Directory);

	// Skip the first argument, but add the rest
	strcpy_s(g_CommandLine, "\"");
	strcat_s(g_CommandLine, g_Directory);
	strcat_s(g_CommandLine, "\\");
	strcat_s(g_CommandLine, argv[2]);
	strcat_s(g_CommandLine, "\"");

	for(int i = 3; i < argc; i++)
	{
		strcat_s(g_CommandLine, " ");
		strcat_s(g_CommandLine, argv[i]);
	}

	// If this is Black Ops, fix the fs_game variable
	if (strstr(g_CommandLine, "BlackOps"))
	{
		StrDel(g_CommandLine, "+set fs_game raw", '\0');

		if (StrDel(g_CommandLine, "+set fs_game dev", '\0'))
			strcat_s(g_CommandLine, "+set fs_game raw");
	}
}

void FixDirectory(int argc, char *argv[])
{
	//
	// Get the current directory to resolve relative paths
	//
	char temp[MAX_PATH];
	sprintf_s(temp, "%s\\%s", g_Directory, argv[2]);

	char *filePart = nullptr;
	GetFullPathNameA(temp, ARRAYSIZE(temp), g_ExeDirectory, &filePart);

	//
	// Trim off the file name
	//
	if (filePart)
		*filePart = '\0';
}

int main(int argc, char *argv[])
{
	//
	// Disable STDOUT buffering
	//
	setbuf(stdout, nullptr);

	if (argc < 3)
	{
		printf("USAGE: launcher_ldr DLL EXE ARGUMENTS\n");
		return 0;
	}

	STARTUPINFOA startupInfo;
	memset(&startupInfo, 0, sizeof(STARTUPINFOA));

	//
	// Redirect output to this console
	//
	startupInfo.cb = sizeof(STARTUPINFOA);
	startupInfo.hStdError = stderr;
	startupInfo.hStdInput = stdin;
	startupInfo.hStdOutput = stdout;

	//
	// Process/thread handles
	//
	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0 , sizeof(PROCESS_INFORMATION));

	//
	// Create a process job object to kill children on exit
	//
	HANDLE ghJob = CreateJobObject(nullptr, nullptr);// GLOBAL

	if (ghJob == nullptr)
	{
		printf("Could not create job object\n");
		return 1;
	}
	else
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;
		memset(&jeli, 0, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));

		//
		// Configure all child processes associated with the job to terminate when the
		// parent process does
		//
		jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		if (!SetInformationJobObject(ghJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
		{
			printf("Could not SetInformationJobObject\n");
			return 1;
		}
	}

	//
	// Call the real process
	//
	FixCommandLine(argc, argv);
	FixDirectory(argc, argv);

	if(!CreateProcessA(nullptr, g_CommandLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, g_ExeDirectory, &startupInfo, &processInfo))
	{
		printf("Failed to create '%s' process\n", argv[2]);
		return 1;
	}

	if (!AssignProcessToJobObject(ghJob, processInfo.hProcess))
	{
		printf("Unable to assign child process job object\n");
		return 1;
	}

	//
	// Inject the DLL
	//
	HANDLE injectThread = InjectDll(processInfo.hProcess, g_Directory, argv[1]);

	if (!injectThread)
	{
		printf("DLL injection failed\n");
		return 1;
	}

	//
	// Flush IO
	//
	fflush(stdout);

	//
	// Resume the process
	//
	ResumeThread(injectThread);
	WaitForSingleObject(injectThread, INFINITE);

	ResumeThread(processInfo.hThread);

	//
	// Close thread handles
	//
	CloseHandle(processInfo.hThread);
	CloseHandle(injectThread);

	//
	// Wait until the process exits
	//
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	CloseHandle(processInfo.hProcess);
	CloseHandle(ghJob);

	return 0;
}