#include "PythonWrapper.h"

void Wrapper_Py_DECREF(PyObject* pObject)
{
	if (pObject != NULL)
	{
		Py_DECREF(pObject);
	}
}

//PyRun_SimpleString("print 'aaaaa'");						// 运行单个字符串
//PyRun_SimpleFile(FILE *fp, const char *filename)			// 运行整个py文件
//PyArg_ParseTuple(pValue, "ii", &aaa, &b)					// 解析元祖返回值
////aaa = PyInt_AsLong(pValue);								// 解析数值返回值
//PyTuple_New(2);											// 新建元祖，准备传递参数给py
//pValue = PyInt_FromLong(4);								// C类型数据转换为PyObject类型数据
//PyTuple_SetItem(pArgs, i, pValue);						// 设置PyObject数据值pValue到元祖pArgs里
//pArgs = PyTuple_New(0);
//pArgs = PyTuple_New(2);
//for (int i = 0; i < 2; ++i) 
//{
//	pValue = PyInt_FromLong(4);
//	if (!pValue) 
//	{
//		Py_DECREF(pArgs);
//		return;
//	}
//	PyTuple_SetItem(pArgs, i, pValue);
//}

PythonEmbedWrapper::PythonEmbedWrapper(char* exeName)
{
	Init(exeName);
}

PythonEmbedWrapper::~PythonEmbedWrapper()
{
	UnInit();
}

int callPythonAdd()
{
	char *fileName = "python";
	char *funcName = "add";
	PyObject *pName, *pModule, *pDict, *pFunc;
	PyObject *pArgs, *pValue;
	pName = PyString_FromString(fileName);
	pModule = PyImport_Import(pName);
	Wrapper_Py_DECREF(pName);
	if (pModule != NULL) 
	{
		pFunc = PyObject_GetAttrString(pModule, funcName);
		if (pFunc && PyCallable_Check(pFunc)) 
		{
			pArgs = PyTuple_New(2);
			for (int i = 0; i < 2; ++i) {
				pValue = PyInt_FromLong(4);
				if (!pValue) 
				{
					Wrapper_Py_DECREF(pArgs);
					Wrapper_Py_DECREF(pModule);
					return 1;
				}
				PyTuple_SetItem(pArgs, i, pValue);
			}
			pValue = PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);
			if (pValue != NULL) 
			{
				int aaa = 0;
				int b = 0;
				if (!PyArg_ParseTuple(pValue, "ii", &aaa, &b))
				{
					//MessageBox(0,L"bbbbb",0,0);
				}
				
				wchar_t buf[256] = {0};
				swprintf(buf, sizeof(buf), L"%d,%d", aaa, b);
				//MessageBox(0,buf,0,0);
				Py_DECREF(pValue);
			}
			else 
			{
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				return 1;
			}
		}
		else 
		{
			if (PyErr_Occurred())
				PyErr_Print();

		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);

	}
	else 
	{
		//MessageBox(0,L"cccc",0,0);
		PyErr_Print();
		return 1;
	}

}

std::string Wrapper_GetPythonException()
{
	std::string strErrorMsg;
	if (!Py_IsInitialized())
	{
		strErrorMsg = "Python 运行环境没有初始化！";
		return strErrorMsg;
	}

	if (PyErr_Occurred() != NULL)
	{
		PyObject *type_obj, *value_obj, *traceback_obj;
		PyErr_Fetch(&type_obj, &value_obj, &traceback_obj);
		if (value_obj == NULL)
		{
			return strErrorMsg;
		}

		PyErr_NormalizeException(&type_obj, &value_obj, 0);
		if (PyString_Check(PyObject_Str(value_obj)))
		{
			strErrorMsg = PyString_AsString(PyObject_Str(value_obj));
		}

		if (traceback_obj != NULL)
		{
			strErrorMsg += "Traceback:";

			PyObject * pModuleName = PyString_FromString("traceback");
			PyObject * pTraceModule = PyImport_Import(pModuleName);
			Py_XDECREF(pModuleName);
			if (pTraceModule != NULL)
			{
				PyObject * pModuleDict = PyModule_GetDict(pTraceModule);
				if (pModuleDict != NULL)
				{
					PyObject * pFunc = PyDict_GetItemString(pModuleDict,"format_exception");
					if (pFunc != NULL)
					{
						PyObject * errList = PyObject_CallFunctionObjArgs(pFunc,type_obj,value_obj, traceback_obj,NULL);
						if (errList != NULL)
						{
							int listSize = PyList_Size(errList);
							for (int i=0;i < listSize;++i)
							{
								strErrorMsg += PyString_AsString(PyList_GetItem(errList,i));
							}
						}
					}
				}
				Py_XDECREF(pTraceModule);
			}
		}
		Py_XDECREF(type_obj);
		Py_XDECREF(value_obj);
		Py_XDECREF(traceback_obj);
	}
	return strErrorMsg;
}

void PythonEmbedWrapper::errorProcess()
{
	if (PyErr_Occurred())
	{
		m_LastError = Wrapper_GetPythonException();
	}
}

std::string PythonEmbedWrapper::GetPythonException()
{
	return m_LastError;
}

void PythonEmbedWrapper::PyCallFunc(const char* moduleName, const char* funcName)
{
	m_LastError = "";
	std::string strModuleName = moduleName;
	PyObject *pModule = NULL;
	std::map< std::string, PyObject* >::iterator it = m_AllPyModule.find(strModuleName);
	if (it == m_AllPyModule.end())
	{
		PyObject *pName = PyString_FromString(strModuleName.c_str());
		pModule = PyImport_Import(pName);
		Wrapper_Py_DECREF(pName);
		if (pModule == NULL)
		{
			errorProcess();
			return;
		}
		m_AllPyModule[strModuleName] = pModule;
	}
	else
	{
		pModule = it->second;
	}

	PyObject *pFunc = PyObject_GetAttrString(pModule, funcName);
	if (pFunc && PyCallable_Check(pFunc)) 
	{
		// 准备参数
		PyObject *pValue = NULL;
		PyObject *pArgs = NULL;

		// 调用函数
		pValue = PyObject_CallObject(pFunc, pArgs);
		Wrapper_Py_DECREF(pArgs);
		if (pValue != NULL) 
		{
			int aaa = 0;
			int b = 0;
			if (!PyArg_ParseTuple(pValue, "ii", &aaa, &b))
			{
				//MessageBox(0,L"bbbbb",0,0);
			}

			wchar_t buf[256] = {0};
			swprintf(buf, sizeof(buf), L"%d,%d", aaa, b);
			Wrapper_Py_DECREF(pValue);
		}
		else 
		{
			errorProcess();
			Wrapper_Py_DECREF(pFunc);
			Wrapper_Py_DECREF(pValue);
			return;
		}
	}
	else
	{
		errorProcess();
		Wrapper_Py_DECREF(pFunc);
		return;
	}
	Wrapper_Py_DECREF(pFunc);
}

void PythonEmbedWrapper::Init(char* exeName)
{
	Py_SetProgramName(exeName);
	Py_Initialize();
}

void PythonEmbedWrapper::UnInit()
{
	for (std::map< std::string, PyObject* >::iterator it=m_AllPyModule.begin(); it!=m_AllPyModule.end(); ++it)
	{
		Wrapper_Py_DECREF(it->second);
	}
	Py_Finalize();
}

