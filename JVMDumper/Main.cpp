#include <windows.h>
#include <jni.h>
#include <jvmti.h>

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

char DUMP_DIRECTORY[1024];

static jrawMonitorID mutex;

static void check(jvmtiEnv *jvmti, jvmtiError errnum, const char *str)
{
	if (errnum != JVMTI_ERROR_NONE)
	{
		char *errnum_str = NULL;
		jvmti->GetErrorName(errnum, &errnum_str);
		char b[1024];
		sprintf_s(b, 1024, "ERROR: JVMTI: %d(%s): %s\n", errnum, (errnum_str == NULL ? "Unknown" : errnum_str), (str == NULL ? "" : str));
		MessageBoxA(0, b, "Error", MB_OK);
	}
}

static void enter_critical_section(jvmtiEnv *jvmti)
{
	jvmtiError error = jvmti->RawMonitorEnter(mutex);
	check(jvmti, error, "Cannot enter with raw monitor");
}

static void exit_critical_section(jvmtiEnv *jvmti)
{
	jvmtiError error = jvmti->RawMonitorExit(mutex);
	check(jvmti, error, "Cannot exit with raw monitor");
}

static void JNICALL loadClass(jvmtiEnv *jvmti,
	JNIEnv* env,
	jclass class_being_redefined,
	jobject loader,
	const char* name,
	jobject protection_domain,
	jint class_data_len,
	const unsigned char* class_data,
	jint* new_class_data_len,
	unsigned char** new_class_data)
{
	enter_critical_section(jvmti);
	{
		std::string str(name);
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == '/')
				str[i] = '.';
		}

		char file[512] = "";
		sprintf_s(file, 512, "%s%s.class", DUMP_DIRECTORY, str.c_str());

		std::ofstream myfile;
		myfile.open(file);
		for (int i = 0; i < class_data_len; i++)
			myfile << class_data[i];
		myfile.close();

		printf("Dumped: %s%s.class\n", DUMP_DIRECTORY, str.c_str());
	}
	exit_critical_section(jvmti);
}

void dump_shit()
{
	const char* dumpdir = "dump\\";
	char b[1024];
	if (GetModuleFileNameA(NULL, DUMP_DIRECTORY, 1023) > 0) {
		char* x = NULL;
		for (x = DUMP_DIRECTORY + strlen(DUMP_DIRECTORY) - 1; x != DUMP_DIRECTORY && *x != '\\'; --x);
		x++;
		*x = '\0';
		strncat_s(DUMP_DIRECTORY, 1023, dumpdir, strlen(dumpdir));
	}
	if (!(CreateDirectoryA(DUMP_DIRECTORY, NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError()))
	{
		sprintf_s(b, 1023, "Could not create dump directory %s for dumping. Perhaps it's not writable or path is too long?", DUMP_DIRECTORY);
		MessageBoxA(NULL, b, "Insider", MB_ICONERROR);
		return;
	}

	JavaVM* m_jvm;
	JNIEnv* m_jni;

	typedef jint(__stdcall *get_created_java_vms_proc)(JavaVM**, jsize, jsize*);

	get_created_java_vms_proc fn_get_created_java_vms = (get_created_java_vms_proc)GetProcAddress(GetModuleHandle("jvm.dll"), "JNI_GetCreatedJavaVMs");
	fn_get_created_java_vms(&m_jvm, 1, nullptr);

	jint err = m_jvm->AttachCurrentThread(reinterpret_cast<void**>(&m_jni), nullptr);
	if (err != JNI_OK)
		m_jni = nullptr;

	jint rc;
	jvmtiCapabilities capabilities;
	jvmtiEventCallbacks callbacks;

	jvmtiEnv* jvmti;
	rc = m_jvm->GetEnv(reinterpret_cast<void**>(&jvmti), JVMTI_VERSION_1_2);

	if (rc != JNI_OK)
	{
		sprintf_s(b, 1023, "ERROR: Unable to create jvmtiEnv, GetEnv failed, error=%d\n", rc);

		MessageBoxA(NULL, b, "Insider", MB_OK);

		return;
	}
	else
	{
		(jvmti)->GetCapabilities(&capabilities);
		capabilities.can_tag_objects = 1;
		capabilities.can_generate_garbage_collection_events = 1;
		capabilities.can_redefine_any_class = 1;
		capabilities.can_redefine_classes = 1;
		capabilities.can_generate_all_class_hook_events = 1;
		(jvmti)->AddCapabilities(&capabilities);

		(void)memset(&callbacks, 0, sizeof(callbacks));
		callbacks.ClassFileLoadHook = &loadClass;

		jvmtiError error = jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
		check(jvmti, error, "Cannot set callbacks");

		error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
			JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, (jthread)NULL);
		check(jvmti, error, "Cannot set event notification");

		error = jvmti->CreateRawMonitor("agent data", &mutex);
		check(jvmti, error, "Cannot create raw monitor");
	}
}

void hack()
{
	dump_shit();

	//MessageBoxA(NULL, "Exiting in 5s...", "Insider", MB_OK);
	//std::this_thread::sleep_for(std::chrono::seconds(5));
	//ExitThread(0);
}


int __stdcall DllMain(HINSTANCE instance, DWORD reason, void* reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);

		if (AllocConsole())
		{
			HWND  con_hwndw{ GetConsoleWindow() };
			RECT    con_bound{ 904 + 219, 420 + 420 + 69 };
			RECT    wndw_rect{};

			SetConsoleTitle("JVMDumper by phage");

			GetWindowRect(con_hwndw, &wndw_rect);
			MoveWindow(con_hwndw, wndw_rect.left, wndw_rect.top, con_bound.left, con_bound.top, true);

			SetWindowLong(con_hwndw, GWL_STYLE, GetWindowLong(con_hwndw, GWL_STYLE) | WS_BORDER);
			SetWindowLong(con_hwndw, GWL_EXSTYLE, GetWindowLong(con_hwndw, GWL_EXSTYLE) | WS_EX_LAYERED);

			SetLayeredWindowAttributes(con_hwndw, 0, 230, 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

			freopen("CONOUT$", "w", stdout);
		}

		CreateThread(0, 0, LPTHREAD_START_ROUTINE(hack), instance, 0, 0);

		return true;
	}

	return TRUE;
}