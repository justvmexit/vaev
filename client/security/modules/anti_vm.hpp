#pragma once

#include <Windows.h>
#include <TlHelp32.h>

namespace client
{
    class anti_vm
    {
    private:
        /**
        * Looks for a process.
        * @param name Name of the target process.
        * @return A boolean, true if found, false if not.
        */
        bool find(const char* name)
        {
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            auto snapshot = LI_FN(CreateToolhelp32Snapshot).safe()(TH32CS_SNAPPROCESS, NULL);

            if (LI_FN(Process32First).safe()(snapshot, &entry) == TRUE)
            {
                while (LI_FN(Process32Next).safe()(snapshot, &entry) == TRUE)
                {
                    if (strstr(entry.szExeFile, name))
                    {
                        return true;
                    }
                }
            }

            LI_FN(CloseHandle).safe()(snapshot);
            return false;
        }
        
        /**
        * Puts a string to lowercase.
        * @param input A ptr to the char array.
        */
        void to_lower(unsigned char* input)
        {
            char* p = (char*)input;
            unsigned long length = strlen(p);
            for (unsigned long i = 0; i < length; i++) p[i] = tolower(p[i]);
        }
    public:
        /**
        * Checks for QEMU process.
        * @return find's response.
        */
        int qemu()
        {
            return find(_xor_("qemu-ga.exe").c_str());
        }
           
        /**
        * Checks for Xen process.
        * @return find's response.
        */
        int xen()
        {
            return find(_xor_("xenservice.exe").c_str());
        }
        
        /**
        * Checks for a virtualbox registry key
        * @return RegOpenKey's response.
        */
        int vbox_registry()
        {
            HKEY h_key = 0;
            if (LI_FN(RegOpenKeyExA).safe()(HKEY_LOCAL_MACHINE, _xor_("HARDWARE\\ACPI\\DSDT\\VBOX__").c_str(), 0, KEY_READ, &h_key) == ERROR_SUCCESS) { return true; }

            return false;
        }
        
        /**
        * Checks for a virtualbox drivers, dll's, and some registry keys.
        */
        int vbox()
        {
            if (LI_FN(CreateFileA).safe()(_xor_("\\\\.\\VBoxMiniRdrDN").c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0) != INVALID_HANDLE_VALUE) { return true; }

            if (LI_FN(LoadLibraryA).safe()(_xor_("VBoxHook.dll").c_str())) { return true; }

            HKEY h_key = 0;
            if ((ERROR_SUCCESS == LI_FN(RegOpenKeyExA).safe()(HKEY_LOCAL_MACHINE, _xor_("SOFTWARE\\Oracle\\VirtualBox Guest Additions").c_str(), 0, KEY_READ, &h_key)) && h_key) { LI_FN(RegCloseKey).safe()(h_key); return true; }

            h_key = 0;
            if (LI_FN(RegOpenKeyExA).safe()(HKEY_LOCAL_MACHINE, _xor_("HARDWARE\\DESCRIPTION\\System").c_str(), 0, KEY_READ, &h_key) == ERROR_SUCCESS)
            {
                unsigned long type = 0;
                unsigned long size = 0x100;
                char* systembiosversion = (char*)LI_FN(LocalAlloc).safe()(LMEM_ZEROINIT, size + 10);
                if (ERROR_SUCCESS == LI_FN(RegQueryValueExA).safe()(h_key, _xor_("SystemBiosVersion").c_str(), 0, &type, (unsigned char*)systembiosversion, &size))
                {
                    this->to_lower((unsigned char*)systembiosversion);
                    if (type == REG_SZ || type == REG_MULTI_SZ)
                    {
                        if (strstr(systembiosversion, _xor_("vbox").c_str()))
                        {
                            return true;
                        }
                    }
                }
                LI_FN(LocalFree).safe()(systembiosversion);

                type = 0;
                size = 0x200;
                char* videobiosversion = (char*)LI_FN(LocalAlloc).safe()(LMEM_ZEROINIT, size + 10);
                if (ERROR_SUCCESS == RegQueryValueEx(h_key, _xor_("VideoBiosVersion").c_str(), 0, &type, (unsigned char*)videobiosversion, &size))
                {
                    if (type == REG_MULTI_SZ)
                    {
                        char* video = videobiosversion;
                        while (*(unsigned char*)video)
                        {
                            to_lower((unsigned char*)video);
                            if (strstr(video, _xor_("oracle").c_str()) || strstr(video, _xor_("virtualbox").c_str())) { return true; }
                            video = &video[strlen(video) + 1];
                        }
                    }
                }
                LI_FN(LocalFree).safe()(videobiosversion);
                LI_FN(RegCloseKey).safe()(h_key);

                return false;
            }
        }

        /**
        * Checks for VMWare processes
        * @return RegOpenKey's response.
        */
        int vmware()
        {
            bool state = find(_xor_("vmtoolsd.exe").c_str());
            state = find(_xor_("vmwaretray.exe").c_str());
            state = find(_xor_("vmwareuser.exe").c_str());
            state = find(_xor_("VGAuthService.exe").c_str());
            state = find(_xor_("vmacthlp.exe").c_str());
            return state;
        }
        
        /**
        * Checks for wine function in kernel32.dll
        */
        int wine()
        {
            const auto kernel32 = GetModuleHandle(_xor_("kernel32.dll").c_str());
            if (!kernel32) return true; 

            if (GetProcAddress(kernel32, _xor_("wine_get_unix_file_name").c_str()) != NULL)
                return true;

            return false;
        }
    };
}
