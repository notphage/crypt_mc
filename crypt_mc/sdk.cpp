#include "context.h"
#include "sdk.h"

struct stack_trace_fields
{
	jclass stack_trace_element = nullptr;
	jclass throwable = nullptr;

	jobject obj_exception = nullptr;

	jfieldID fid_method_name = nullptr;
	jfieldID fid_line_number = nullptr;

	jmethodID mid_exception_init = nullptr;
	jmethodID mid_get_stack_trace_element = nullptr;
	jmethodID mid_get_stack_trace_depth = nullptr;
};

static stack_trace_fields stacktracefields;

void c_stack_trace::instantiate(JNIEnv* _jni)
{
	jni = (_jni) ? _jni : ctx.m_jni;

	static bool init_fields = false;

	if (!init_fields)
	{
		stacktracefields.stack_trace_element = (jclass)jni->NewGlobalRef(jni->FindClass(xors("java/lang/StackTraceElement")));
		stacktracefields.throwable = (jclass)jni->NewGlobalRef(jni->FindClass(xors("java/lang/Throwable")));

		stacktracefields.mid_exception_init = jni->GetMethodID(stacktracefields.throwable, xors("<init>"), xors("()V"));
		stacktracefields.mid_get_stack_trace_element = jni->GetMethodID(stacktracefields.throwable, xors("getStackTraceElement"), xors("(I)Ljava/lang/StackTraceElement;"));
		stacktracefields.mid_get_stack_trace_depth = jni->GetMethodID(stacktracefields.throwable, xors("getStackTraceDepth"), xors("()I"));

		stacktracefields.obj_exception = jni->NewGlobalRef(jni->NewObject(stacktracefields.throwable, stacktracefields.mid_exception_init));

		stacktracefields.fid_method_name = jni->GetFieldID(stacktracefields.stack_trace_element, xors("methodName"), xors("Ljava/lang/String;"));
		stacktracefields.fid_line_number = jni->GetFieldID(stacktracefields.stack_trace_element, xors("lineNumber"), xors("I"));

		init_fields = true;
	}
}

void c_stack_trace::get_stack_trace(int index, stack_trace& stack)
{
	jint stack_trace_depth = jni->CallIntMethod(stacktracefields.obj_exception, stacktracefields.mid_get_stack_trace_depth);

	if (stack_trace_depth <= index)
		return;

	jobject stack_trace_element = jni->CallObjectMethod(stacktracefields.obj_exception, stacktracefields.mid_get_stack_trace_element, index);
	jstring str_method_name = reinterpret_cast<jstring>(jni->GetObjectField(stack_trace_element, stacktracefields.fid_method_name));

	jint line_number = jni->GetIntField(stack_trace_element, stacktracefields.fid_line_number);

	jni->DeleteLocalRef(stack_trace_element);

	stack.valid = true;
	stack.method_name = jni->GetStringUTFChars(str_method_name, false);
	stack.str_name = str_method_name;
	stack.line_number = line_number;

	return;
}
