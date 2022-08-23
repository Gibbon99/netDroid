#include "../../hdr/classes/c_shaders.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Return the error string
std::string _droidShaders::getLastError ()
//----------------------------------------------------------------------------------------------------------------------
{
	return m_lastError;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get the m_programID for this shader
unsigned int _droidShaders::getProgramID ()
//----------------------------------------------------------------------------------------------------------------------
{
	return m_programID;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Return if the shader is compiled or not
GLint _droidShaders::isCompiled ()
//----------------------------------------------------------------------------------------------------------------------
{
	return m_linkedOk;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the value of a variable in a shader - single FLOAT version
bool _droidShaders::setUniformVariable (int location, float setTo)
//----------------------------------------------------------------------------------------------------------------------
{
	GL_CHECK(glUniform1f (location, setTo));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the value of a variable in a shader - single INT version
bool _droidShaders::setUniformVariable (int location, int setTo)
//----------------------------------------------------------------------------------------------------------------------
{
	GL_CHECK(glUniform1i (location, setTo));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the value of a vec3 variable in a shader - VEC3 version
bool _droidShaders::setUniformVariable3f (int location, float v1, float v2, float v3)
//----------------------------------------------------------------------------------------------------------------------
{
	GL_CHECK(glUniform3f (location, v1, v2, v3));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the value of a vec3 variable in a shader - VEC4 version
bool _droidShaders::setUniformVariable4f (int location, float v1, float v2, float v3, float v4)
//----------------------------------------------------------------------------------------------------------------------
{
	GL_CHECK(glUniform4f (location, v1, v2, v3, v4));

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get errors from Shaders
std::string _droidShaders::getGLSLError (int object, int objectType)
//----------------------------------------------------------------------------------------------------------------------
{
	GLint       logLength;
	GLint       charsWritten;
	GLchar      *infoLog;
	std::string errorLog{};

	switch (objectType)
	{
		case GLSL_SHADER:
			glGetShaderiv (object, GL_INFO_LOG_LENGTH, &logLength);
			break;

		case GLSL_PROGRAM:
			glGetProgramiv (object, GL_INFO_LOG_LENGTH, &logLength);
			break;

		default:
			break;
	}

	if (logLength > 0)
	{
		infoLog = (GLchar *) malloc (logLength);

		if (nullptr == infoLog)
		{
			return "ERROR: Couldn't get memory to store GLSL error log.";
		}

		switch (objectType)
		{
			case GLSL_SHADER:
				glGetShaderInfoLog (object, logLength, &charsWritten, infoLog);
				break;

			case GLSL_PROGRAM:
				glGetProgramInfoLog (object, logLength, &charsWritten, infoLog);
				break;

			default:
				return "ERROR: Invalid object type passed to getGLSLError";
				break;
		}

		errorLog = infoLog;
		free (infoLog);

		return errorLog;
	}
	return "No error";
}

//----------------------------------------------------------------------------------------------------------------------
//
// Get and store the location of the common variables for a shader
bool _droidShaders::cacheVarLocation ()
//----------------------------------------------------------------------------------------------------------------------
{
	for (auto &varItr: m_locationID)
	{
		if (varItr.second.varType == "uniform")
		{
			auto tempLocation = glGetUniformLocation (m_programID, varItr.first.c_str ());
			m_locationID[varItr.first] = varInfo{true, 0, tempLocation, "uniform"}; // TODO Error check this

#ifdef VERBOSE_SHADERS
			printf("Uniform [ %s ] - Location [ %i ]\n", varItr.first.c_str(), tempLocation);
#endif
		}
		else
		{
			auto tempLocation = glGetAttribLocation (m_programID, varItr.first.c_str ());
			m_locationID[varItr.first] = varInfo{true, 0, tempLocation, "attribute"};
#ifdef VERBOSE_SHADERS
			printf("Attribute [ %s ] - Location [ %i ]\n", varItr.first.c_str(), tempLocation);
#endif
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load the shader from memory and compile
GLint _droidShaders::compileShader (std::string shaderSource, int shaderType)
//----------------------------------------------------------------------------------------------------------------------
{
	//
	// Parse all the in variables from the shader and cache them
//	if (shaderType == GL_VERTEX_SHADER)
	parseVariables (shaderSource);

	auto shaderID = glCreateShader (shaderType);
	glShaderSource (shaderID, 1, reinterpret_cast<const GLchar *const *>(&shaderSource), nullptr);
	glCompileShader (shaderID);

	m_compiledOk = GL_FALSE;
	glGetShaderiv (shaderID, GL_COMPILE_STATUS, &m_compiledOk);

	if (m_compiledOk == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv (shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		char *error = (char *) malloc (maxLength);
		glGetShaderInfoLog (shaderID, maxLength, &maxLength, error);
		m_lastError = sys_getString ("Shader compile error : [ %s ]", error);
		free (error);
		return -1;
	}
	return shaderID;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Parse the variables from a vertex file
//
// Ignores lines that contain "//" - looks for keywords, "in", "uniform"
void _droidShaders::parseVariables (const std::string &textToParse)
//----------------------------------------------------------------------------------------------------------------------
{
	std::string        line;
	std::string        regExpression{' '};
	std::istringstream f (textToParse);

	while (std::getline (f, line))
	{
		if (line.find ("//") == std::string::npos)
		{
			if (line.find ("main") == std::string::npos)
			{
				if (line.find ("uniform") != std::string::npos)
				{
					auto shaderTokens = tokeniseLine (line, regExpression);
					if (shaderTokens[2].c_str () != nullptr)
					{
						shaderTokens[2].pop_back ();
						m_locationID.insert (std::pair<std::string, varInfo> (shaderTokens[2], varInfo{true, 0, 0, "uniform"}));
#ifdef VERBOSE_SHADERS
						printf ("Added shader uniform [ %s ]\n", shaderTokens[2].c_str ());
#endif
					}
				}

				if (line.find ("in") != std::string::npos)
				{
					auto shaderTokens = tokeniseLine (line, regExpression);
					if (shaderTokens.size () > 1)
					{
						if (shaderTokens[0] == "in")
						{
							if (shaderTokens[2].c_str () != nullptr)
							{
								shaderTokens[2] = shaderTokens[2].substr (0, shaderTokens[2].size () - 1);
								m_locationID.insert (std::pair<std::string, varInfo> (shaderTokens[2], varInfo{true, 0, 0, "attribute"}));
#ifdef VERBOSE_SHADERS
								printf ("Added shader attribute [ %s ]\n", shaderTokens[2].c_str ());
#endif
							}
						}
						else if (shaderTokens.size () > 5)
						{
							if (shaderTokens[4] == "in")
							{
								if (shaderTokens[6].c_str () != nullptr)
								{
									shaderTokens[6] = shaderTokens[6].substr (0, shaderTokens[6].size () - 1);   // Remove ; from end
									m_locationID.insert (std::pair<std::string, varInfo> (shaderTokens[6], varInfo{true, 0, 0, "attribute"}));

#ifdef VERBOSE_SHADERS
									printf ("Added shader attribute [ %s ]\n", shaderTokens[6].c_str ());
#endif
								}
							}
						}
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
// Load and compile shaders
bool _droidShaders::create (std::string vertFilename, std::string fragFilename, std::string shaderName)
//----------------------------------------------------------------------------------------------------------------------
{
	m_vertFilename = std::move (vertFilename);
	m_fragFilename = std::move (fragFilename);
	m_shaderName   = std::move (shaderName);

	m_programID      = glCreateProgram ();
	//
	// Load the shaders and set their object ID
	m_vertexShaderID = compileShader (m_vertFilename, GL_VERTEX_SHADER);
	if (m_vertexShaderID < 0)
	{
		m_lastError = sys_getString ("Failed to compile shader [ %s ]", m_lastError.c_str ());
		return false;
	}

	m_fragmentShaderID = compileShader (m_fragFilename, GL_FRAGMENT_SHADER);
	if (m_fragmentShaderID < 0)
	{
		m_lastError = sys_getString ("Failed to compile shader [ %s ] [ %s ]", m_shaderName.c_str (), m_lastError.c_str ());
		return false;
	}
	//
	// Attach the object ID to the program ID ready for compiling
	glAttachShader (m_programID, m_vertexShaderID);
	glAttachShader (m_programID, m_fragmentShaderID);
	//
	// Link the shaders
	GL_CHECK (glLinkProgram (m_programID));
	//
	// Get the status
	GL_CHECK (glGetProgramiv (m_programID, GL_LINK_STATUS, &m_linkedOk));

	if (!m_linkedOk)
	{
		m_lastError = sys_getString ("Shader [ %s ] failed to link - [ %s ]", m_shaderName.c_str (), getGLSLError (m_programID, GLSL_PROGRAM).c_str ());
		return false;
	}

#ifdef VERBOSE_SHADERS
	printf ("INFO: Shader [ %s ] linked and compiled ok.\n", m_shaderName.c_str ());
#endif

	//
	// Needed to avoid error in glUniform in gl_setShaderVars
	GL_CHECK (glUseProgram (m_programID));
	//
	// Setup variables for this shader
	cacheVarLocation ();

	glDeleteShader (m_vertexShaderID);
	glDeleteShader (m_fragmentShaderID);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
//
// Set the value of a shader uniform or attribute
//
// If variable is not found, still record, but mark has invalid - count how many
// times it is called.
int _droidShaders::getLocation (const std::string &keyName)
//----------------------------------------------------------------------------------------------------------------------
{
	std::string u_keyName;
	GLint       keyLocation = 0;
	varInfo     varInfoType;

	u_keyName = keyName;

	auto shaderItr = m_locationID.find (u_keyName);
	if (shaderItr == m_locationID.end ())   // Key doesn't exist
	{
		m_lastError = sys_getString ("Key [ %s ] doesn't exist in map.", u_keyName.c_str ());

		keyLocation = glGetUniformLocation (m_programID, u_keyName.c_str ());
		if (keyLocation == 0)
		{
			m_lastError = sys_getString ("keyName wasn't found in shader using getUniformLocation [ %s - %i ]", u_keyName.c_str (), m_programID);
			keyLocation = glGetAttribLocation (m_programID, u_keyName.c_str ());
		}

		if (keyLocation == -1)
		{
			m_lastError = sys_getString ("Shader [ %i ] Value [ %s ] does not exist.", m_programID, u_keyName.c_str ());
			varInfoType.location = keyLocation;
			varInfoType.refCount = 0;
			varInfoType.isFound  = false;
			m_locationID.insert (std::pair<std::string, varInfo> (u_keyName, varInfoType));
			return 0;
		}

		varInfoType.location = keyLocation;
		varInfoType.refCount = 0;
		varInfoType.isFound  = true;

		m_locationID.insert (std::pair<std::string, varInfo> (u_keyName, varInfoType));
		return keyLocation;
	}
	else
	{
		if (shaderItr->second.isFound)
		{
			shaderItr->second.refCount++;
//		con_print(CON_INFO, true, "Got key [ %s ] from map. refCount [ %i ]", keyName.c_str(), shaderItr->second.refCount);
			return shaderItr->second.location;
		}
		else
		{
			shaderItr->second.refCount++;
			return -1;  // Count how many times it was called, and not found
		}
	}
}