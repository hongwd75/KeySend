#pragma once
#include "DirectoryChanges.h"
#include <map>
#include <vector>
#include <string>

/////////////////////////////////////////////////////////////////////////////////
//
struct fileAction
{
	std::string actionString;
	int keyIndex;
	bool alt,ctrl,shift;
};

/////////////////////////////////////////////////////////////////////////////////
//
struct fileCheckAction
{
	DWORD fileReadByte;
	std::string filename;
	std::vector<fileAction*> list;
};


/////////////////////////////////////////////////////////////////////////////////
//
class myDirectoryChangeHandler : public CDirectoryChangeHandler
{
public:
	myDirectoryChangeHandler();
	virtual ~myDirectoryChangeHandler();

public:
	void loadJson(TCHAR *filename);


protected:
	void On_FileModified(const CString & strFileName);

protected:
	std::vector<fileAction*> m_list;
};

