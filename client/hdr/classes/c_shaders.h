#pragma once

#include <sstream>
#include <string>
#include <map>
#include "../../hdr/defines.h"
#include "com_util.h"

class _droidShaders
{
public:
	unsigned int getProgramID ();

	GLint isCompiled ();

	bool setUniformVariable (int location, int setTo);

	bool setUniformVariable3f (int location, float v1, float v2, float v3);

	bool setUniformVariable4f (int location, float v1, float v2, float v3, float v4);

	int getLocation (const std::string &keyName);

	std::string getGLSLError (int object, int objectType);

	std::string getLastError ();

	bool cacheVarLocation ();

	GLint compileShader (std::string shaderSource, int shaderType);

	void parseVariables (const std::string &textToParse);

	bool create (std::string vertFilename, std::string fragFilename, std::string shaderName);

private:

	struct varInfo
	{
		bool        isFound{false};
		int         refCount{0};
		int         location{0};
		std::string varType{};
	};

	GLint  m_linkedOk{GL_FALSE};
	GLint  m_compiledOk{GL_FALSE};
	GLuint m_programID{};
	int    m_vertexShaderID{};
	int    m_fragmentShaderID{};

	std::string                    m_shaderName{};
	std::string                    m_vertFilename{};
	std::string                    m_fragFilename{};
	std::string                    m_lastError{};
	std::map<std::string, varInfo> m_locationID{};
};
