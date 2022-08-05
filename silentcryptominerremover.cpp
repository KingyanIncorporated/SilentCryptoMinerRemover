#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <tlhelp32.h>
#include <process.h>

using namespace std;

BOOL GetProcessList();
BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode);


//Searches for a string in a string array
bool in_array(string value, string* array)
{
    int size = (*array).size();
    for (int i = 0; i < size; i++)
    {
        if (value == array[i])
        {
            return true;
        }
    }

    return false;
}

//Searches for and shuts down a process list
BOOL GetProcessList()
{
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return(FALSE);
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);  // clean the snapshot object
        return(FALSE);
    }

    // Now walk the snapshot of processes 
    do
    {
        wstring ws(pe32.szExeFile);
        string str(ws.begin(), ws.end());

        string tab[6] = { "nslookup.exe", "notepad.exe", "svchost.exe", "conhost.exe", "explorer.exe", "cmd.exe"};
        if (in_array(str, tab))
        {
            TerminateMyProcess(pe32.th32ProcessID, 1);
            Sleep(500);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return(TRUE);
}

//Terminates process
BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

//Returns the path of the program executable
string GetProgramPath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string::size_type pos = string(buffer).find_last_of("\\/");

    return string(buffer).substr(0, pos);
}


//Sets a privilege
BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
)
{
    TOKEN_PRIVILEGES tp{};
    LUID luid;

    if (!LookupPrivilegeValue(
        NULL,            // lookup privilege on local system
        lpszPrivilege,   // privilege to lookup 
        &luid))        // receives LUID of privilege
    {
        printf("LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if (!AdjustTokenPrivileges(
        hToken,
        FALSE,
        &tp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES)NULL,
        (PDWORD)NULL))
    {
        printf("AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

    {
        printf("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}


//Where the magic happens
void RemoveMiner(HKEY hKey)
{
    constexpr int MAX_KEY_LENGTH = 255;
    constexpr int MAX_VALUE_NAME = 16384;
    DWORD    cbName = 0;                   // size of name string 
    wchar_t  achClass[MAX_PATH];  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode;

    wchar_t  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;


    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    // Enumerate the key values. 

    if (cValues)
    {
        printf("\nNumber of values: %d\n", cValues);

        for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
        {
            cchValue = MAX_VALUE_NAME;
            achValue[0] = '\0';
            retCode = RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL);

            if (retCode == ERROR_SUCCESS)
            {
                _tprintf(L"(%d) %s\n", i + 1, achValue);
                
            } else if (retCode == ERROR_ACCESS_DENIED) {
                cout << "Access to registry denied, did you enable Administrator privileges?" << endl;
            }
        }

        //Makes a regkey backup in the executable folder
        string strregsavepath = GetProgramPath()+"\\registrybackup.reg";
        wstring tempwstr = wstring(strregsavepath.begin(), strregsavepath.end());
        LPCWSTR lpregsavepath = tempwstr.c_str();
        const char* chregsaavepath = strregsavepath.c_str();

        cout << "saving registry backup file to ";
        wcout << lpregsavepath << endl;

        HANDLE ProcessToken;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ProcessToken)) {
            SetPrivilege(ProcessToken, SE_BACKUP_NAME, TRUE);

            LSTATUS lstatus = RegSaveKey(hKey, lpregsavepath, NULL);
            if (lstatus == ERROR_SUCCESS) {
                cout << "file saved successfully" << endl;
            } else {
                bool n = true;
                while (lstatus != ERROR_SUCCESS  && n) {
                    int prompt = 0;
                    cout << "registry key could not be backed up, do you want to try to save again or do you want to go without saving? (0/1) (This will delete the previous backup file)" << endl;
                    cin >> prompt;
                    if (prompt == 0) {
                        int r = remove(chregsaavepath);
                        if (r != 0) cout << r << chregsaavepath << endl;
                        lstatus = RegSaveKey(hKey, lpregsavepath, NULL);
                        if (lstatus == ERROR_SUCCESS) {
                            cout << "file saved successfully" << endl;
                        }
                        else {
                            cout << lstatus << endl;
                        }
                    }
                    else {
                        n = false;
                    }
                }
            }
        }


        //Asks user to chose the sus program
        int MinerKeyIndex = 0;
        cout << "Which program do you not recognise?" << endl;
        cin >> MinerKeyIndex;

        
        cchValue = MAX_VALUE_NAME;
        achValue[0] = '\0';
        retCode = RegEnumValue(hKey, MinerKeyIndex-1, achValue, &cchValue, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            cout << "Program " << MinerKeyIndex << " ";
            wcout << achValue;
            cout << " will be deleted" << endl;
            
            //Kill Watchdog process
            cout << "killing watchdog processes (sihost64.exe, conhost.exe)" << endl;
            system("taskkill /IM sihos64.exe /F");
            system("taskkill /IM conhost.exe \"fhweiowe\" /F");
            system("taskkill /IM conhost.exe \"/sihost64\" /F");
            cout << "killing miner processes (explorer.exe, nslookup.exe, notepad.exe, svchost.exe, conhost.exe, cmd.exe" << endl;
            //Kill Miner process
            GetProcessList();

            //Grabs data from sus regkay to delete the miner executable
            wstring strValue = L"";
            WCHAR szBuffer[512];
            DWORD dwBufferSize = sizeof(szBuffer);
            ULONG nError;
            nError = RegQueryValueEx(hKey, achValue, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
            if (ERROR_SUCCESS == nError)
            {
                strValue = szBuffer;
                //wstring to char conversion shenanigans (why can't it be simple?)
                wcout << strValue << endl;

                strValue.erase(0, 1);
                strValue.erase(strValue.size() - 1);

                const wchar_t* input = strValue.c_str();

                
                size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
                char* chValue = new char[size];
                size_t convertedSize;
                wcstombs_s(&convertedSize,chValue, size,input, size);
                cout << chValue << endl;
                int r = remove(chValue);
                if (r != 0) cout << "couldn't delete miner exe" << endl;

                //Deletes the sus regkey value to prevent the miner from starting when the computer boots up and make a new executable
                nError = RegDeleteValue(hKey, achValue);
                if (ERROR_SUCCESS == nError) {
                    cout << "regkey value deleted" << endl;

                    //give shutdown privilege and reboot computer
                    HANDLE ShutdownToken;
                    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &ProcessToken)) {
                        SetPrivilege(ShutdownToken, SE_SHUTDOWN_NAME, TRUE);
                        BOOL Shutdown = InitiateSystemShutdownEx(NULL, NULL, 0, TRUE, TRUE, SHTDN_REASON_MINOR_SECURITY);
                        if (!Shutdown) cout << "couldn't shut down computer" << GetLastError() << endl;
                    }
                }
                else {
                    cout << "couldn't delete regkey value" << endl;
                    cout << nError << endl;

                    char tempcin = (char)" ";
                    cin >> tempcin;
                }
            } else {
                cout << "couldn't get value from regkey value" << endl;
                cout << nError << endl;

                char tempcin = (char)" ";
                cin >> tempcin;
            }
        }
        else {
            cout << retCode << endl;
            cout << GetLastError() << endl;
            char tempcin = (char)" ";
            cin >> tempcin;
        }
    }
}


int main()
{
    //asks the user for consent (as you should always do)
    int prompt = 0;
    cout << "Welcome to the SilentCryptoMinerRemover tool (please run in admnistrator mode) \n this program will first close all the proccesses the miner is known to inject itself into\n and will then help with deleting the registry key that makes the miner boot on startup \n the program will then restart your computer \n please run this program in administrator mode \n do you want to proceed? (0/1)" << endl;
    cin >> prompt;
    if (prompt == 0) {
        return 0;
    }

    //Opens regkey and starts the other function
    HKEY hTestKey;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
        0,
        KEY_ALL_ACCESS,
        &hTestKey) == ERROR_SUCCESS
        )
    {
        RemoveMiner(hTestKey);
    }
    
    RegCloseKey(hTestKey);
}
