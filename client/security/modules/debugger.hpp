#pragma once

#include <Windows.h>
#include <modules/lazy_importer.hpp>

namespace client
{
	class debugger
	{
	private:
		typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
		typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);
	public:
		/**
		* Checks if a debugger is present via the Windows API.
		* @return Integer containing returned value of the windows api call.
		*/
		int is_present()
		{
			return LI_FN(IsDebuggerPresent).safe()();
		}
		
		/**
		* Outputs to the debug console and checks if there is last error (An error occurs if no debugger is attached and outputting)
		* @return Integer containing returned value of the windows api call.
		*/
		int debug_string()
		{
			LI_FN(SetLastError).safe()(0);
			LI_FN(OutputDebugStringA).safe()(_xor_("xen0n.app security").c_str());
			const auto last_error = LI_FN(GetLastError).safe()();
			
			return last_error != 0;
		}
		
		/**
		* Hides the current thread from debugger.
		* @return Boolean returning status.
		*/
		int hide_thread()
		{
			unsigned long thread_hide_from_debugger = 0x11;

			const auto ntdll 
				= LI_FN(LoadLibraryA).safe()(
					_xor_("ntdll.dll").c_str()
					);

			if (ntdll == INVALID_HANDLE_VALUE || ntdll == NULL) { return false; }

			_NtQueryInformationProcess NtQueryInformationProcess = NULL;
			NtQueryInformationProcess = (_NtQueryInformationProcess)LI_FN(GetProcAddress).safe()(
				ntdll, _xor_("NtQueryInformationProcess").c_str()
				);

			if (NtQueryInformationProcess == NULL) { return false; }

			(_NtSetInformationThread)(GetCurrentThread(), thread_hide_from_debugger, 0, 0, 0);

			return true;
		}

		/**
		* Checks if a remote debugger is present in the current appliction.
		* @return Integer containing returned value of the windows api call.
		*/
		int remote_is_present()
		{
			int debugger_present = false;

			LI_FN(CheckRemoteDebuggerPresent).safe()(
				LI_FN(GetCurrentProcess).safe()(), &debugger_present
				);

			return debugger_present;
		}
		
		/**
		* Checks the thread context.
		* @return Status of thread contexts.
		*/
		int thread_context()
		{
			int found = false;
			CONTEXT ctx = { 0 };
			void* h_thread = LI_FN(GetCurrentThread).safe()();

			ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			if (LI_FN(GetThreadContext).safe()(h_thread, &ctx))
			{
				if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
				{
					found = true;
				}
			}

			return found;
		}
	};
}
