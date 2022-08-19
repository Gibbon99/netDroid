#include <execinfo.h>
#include "../../hdr/openGL/c_glDebug.h"

//-----------------------------------------------------------------------------
//
// Get all the openGL errors
void gl_getAllGLErrors (int errorNum, const char *calledFrom, int line)
//-----------------------------------------------------------------------------
{
	errorNum = glGetError ();

	while (errorNum != GL_NO_ERROR)
	{
		clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("OpenGL Error: [ %i ] - [ %i ] - [ %s ]", line, errorNum, calledFrom));

		switch (errorNum)
		{
			case GL_INVALID_ENUM:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument."));
				break;

			case GL_INVALID_VALUE:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "GL_INVALID_VALUE: A numeric argument is out of range."));
				break;

			case GL_INVALID_OPERATION:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "GL_INVALID_OPERATION: The specified operation is not allowed in the current state."));
				break;

			case GL_OUT_OF_MEMORY:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command."));
				break;

			case GL_STACK_UNDERFLOW:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "An attempt has been made to perform an operation that would cause an internal stack to underflow."));
				break;

			case GL_STACK_OVERFLOW:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "An attempt has been made to perform an operation that would cause an internal stack to overflow."));
				break;

			default:
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("[ %s ]", "Unknown error."));
				break;
		}

		errorNum = glGetError ();
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - return string of type source
//
//Parsing code from OpenGL Shader Language CookBook SE
//https://github.com/daw42/glslcookbook
//
std::string getStringForType (GLenum type)
//--------------------------------------------------------------------------------------------
{
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			return "Error";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			return "Deprecated behavior";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			return "Undefined behavior";
		case GL_DEBUG_TYPE_PORTABILITY:
			return "Portability issue";
		case GL_DEBUG_TYPE_PERFORMANCE:
			return "Performance issue";
		case GL_DEBUG_TYPE_MARKER:
			return "Stream annotation";
		case GL_DEBUG_TYPE_OTHER:
			return "Other";
		default:
			assert (false);
			return "";
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - return string of error source
std::string getStringForSource (GLenum source)
//--------------------------------------------------------------------------------------------
{
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:
			return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			return "Window system";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			return "Shader compiler";
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			return "Third party";
		case GL_DEBUG_SOURCE_APPLICATION:
			return "Application";
		case GL_DEBUG_SOURCE_OTHER:
			return "Other";
		default:
			assert (false);
			return "";
	}
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - get string for severity
std::string getStringForSeverity (GLenum severity)
//--------------------------------------------------------------------------------------------
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:
			return "Medium";
		case GL_DEBUG_SEVERITY_LOW:
			return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "Notification";
		default:
			assert (false);
			return ("");
	}
}

//--------------------------------------------------------------------------------------------
//
// Obtain a backtrace and print it to stdout.
void print_trace () // TODO Move to better file
//--------------------------------------------------------------------------------------------
{
#ifdef __linux__

	void   *array[10];
	size_t size;
	char   **strings;
	size_t i;

	size    = static_cast<size_t>(backtrace (array, 10));
	strings = backtrace_symbols (array, static_cast<int>(size));

	printf ("Obtained %zd stack frames.\n", size);

	for (i = 0; i < size; i++)
	{
		printf ("%s\n", strings[i]);
	}

//	free (strings);
#endif
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug Callback
void GLAPIENTRY gl_DebugCallback (GLenum source, GLenum type, GLenum id, GLenum severity, [[maybe_unused]]GLsizei length, const GLchar *msg, [[maybe_unused]]const void *data)
//--------------------------------------------------------------------------------------------
{
//	if ( false == g_debugOpenGL )
//		return;

	if ((131185 == id) | (131169 == id) | (131218 == id))
		return;

	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString ("ID [ %i ]", id));
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString ("source [ %s ]", getStringForSource (source).c_str ()));
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString ("type [ %s ]", getStringForType (type).c_str ()));
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString ("severity [ %s ]", getStringForSeverity (severity).c_str ()));
	clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE | MESSAGE_TARGET_CONSOLE, sys_getString ("debug call [ %s ]", msg));
}

//--------------------------------------------------------------------------------------------
//
// OpenGL Debug - Register callback
void gl_registerDebugCallback ()
//--------------------------------------------------------------------------------------------
{
	glEnable (GL_DEBUG_OUTPUT);
	glDebugMessageCallback (gl_DebugCallback, nullptr);
	glDebugMessageControl (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}