#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

#include <modules/lazy_importer.hpp>
#include <modules/xor_string.hpp>

namespace client
{
    class bad_processes
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
    public:
        /**
        * Checks for bad processes
        * @return Checks a bunch of processes and returns boolean containing status.
        */
        int check()
        {
            std::vector<char*> processes {
                _xor_("ollydbg.exe").c_str(),
                _xor_("ProcessHacker.exe").c_str(),
                _xor_("tcpview.exe").c_str(),
                _xor_("autoruns.exe").c_str(),
                _xor_("autorunsc.exe").c_str(),
                _xor_("filemon.exe").c_str(),
                _xor_("procmon.exe").c_str(),
                _xor_("regmon.exe").c_str(),
                _xor_("procexp.exe").c_str(),
                _xor_("idaq.exe").c_str(),
                _xor_("idaq64.exe").c_str(),
                _xor_("ida.exe").c_str(),
                _xor_("ida64.exe").c_str(),
                _xor_("ImmunityDebugger.exe").c_str(),
                _xor_("Wireshark.exe").c_str(),
                _xor_("dumpcap.exe").c_str(),
                _xor_("HookExplorer.exe").c_str(),
                _xor_("ImportREC.exe").c_str(),
                _xor_("PETools.exe").c_str(),
                _xor_("LordPE.exe").c_str(),
                _xor_("SysInspector.exe").c_str(),
                _xor_("proc_analyzer.exe").c_str(),
                _xor_("sysAnalyzer.exe").c_str(),
                _xor_("sniff_hit.exe").c_str(),
                _xor_("windbg.exe").c_str(),
                _xor_("joeboxcontrol.exe").c_str(),
                _xor_("joeboxserver.exe").c_str(),
                _xor_("ResourceHacker.exe").c_str(),
                _xor_("x32dbg.exe").c_str(),
                _xor_("x64dbg.exe").c_str(),
                _xor_("Fiddler.exe").c_str(),
                _xor_("httpdebugger.exe").c_str(),
                //_xor_("devenv.exe").c_str()
            };

            for (auto process : processes)
            {
                if (find(process)) return true;
            }
            
            return false;
        }
    };
}
