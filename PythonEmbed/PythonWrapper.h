#ifndef __PYTHON_WRAPPER__
#define __PYTHON_WRAPPER__

#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include <string>
#include <map>

class PythonEmbedWrapper
{
public:
	PythonEmbedWrapper(char* exeName);
	~PythonEmbedWrapper();

	void PyCallFunc(const char* moduleName, const char* funcName);
	std::string GetPythonException();

	void Init(char* exeName);
	void UnInit();

private:
	void errorProcess();


private:
	std::map< std::string, PyObject* > m_AllPyModule;
	std::string m_LastError;
};


#endif // __PYTHON_WRAPPER__