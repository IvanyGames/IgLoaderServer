#include <windows.h>

int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR cmdline, int)
{
	const char *dllName = "IgPatchServer.dll";

	STARTUPINFOA cif;
	ZeroMemory(&cif, sizeof(STARTUPINFOA));
	PROCESS_INFORMATION pi;

	BOOL bCreateProcess = CreateProcessA("Game.exe", cmdline, NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi);

	if (bCreateProcess == FALSE) {
		return 1;
	}

	HANDLE proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);

	if (proc_handle == NULL)
		return 2;

	HMODULE hModuleKernel32 = GetModuleHandleA("kernel32.dll");

	if (hModuleKernel32 == NULL)
		return 3;

	LPCVOID LoadLibAddy = GetProcAddress(hModuleKernel32, "LoadLibraryA");

	if (LoadLibAddy == NULL)
		return 4;

	LPVOID RemoteString = VirtualAllocEx(proc_handle, NULL, strlen(dllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (RemoteString == NULL)
		return 5;

	BOOL bWriteProcessMemory = WriteProcessMemory(proc_handle, RemoteString, dllName, strlen(dllName), NULL);

	if (bWriteProcessMemory == FALSE) {
		return 6;
	}

	HANDLE hRemoteThread = CreateRemoteThread(proc_handle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, RemoteString, NULL, NULL);

	if (hRemoteThread == NULL) {
		return 7;
	}

	CloseHandle(proc_handle);

	WaitForSingleObject(pi.hProcess, (DWORD)-1);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}