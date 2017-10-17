#include "stdafx.h"
#include "YXPFileIO.h"

using namespace std;

YXPFileIO::YXPFileIO()
{
}


YXPFileIO::~YXPFileIO()
{
}


//����ָ��Ŀ¼���丸Ŀ¼�������
//������A�汾����W�汾��Ҳ��֧�����ĵ�
bool YXPFileIO::FindOrMkDir(const std::string& pszPath)
{
	USES_CONVERSION;
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(pszPath.c_str(), &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}

	if (!::CreateDirectoryA(pszPath.c_str(), NULL))
	{
		::AfxMessageBox(_T("����Ŀ¼ʧ��"));
		return false;
	}
	else
		return true;
}



// �ж�Ŀ¼�Ƿ����(/��\\�����ԣ����󲻴���\\������)  
bool YXPFileIO::FolderExists(const std::string& s)
{
	DWORD attr;
	attr = GetFileAttributesA(s.c_str());
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_DIRECTORY);
}


bool YXPFileIO::FileExists(const std::string& s)
{
	DWORD attr;
	attr = GetFileAttributesA(s.c_str());
	return (attr != (DWORD)(-1)) &&
		(attr & FILE_ATTRIBUTE_ARCHIVE); //���������صĻ���ֻ���Ķ��ܼ�⵽
}

// �ݹ鴴��Ŀ¼������ʹ��/��ֻ��\\�����󲻴���\\������)
// ���Ŀ¼�Ѿ����ڻ��ߴ����ɹ�����TRUE  
bool YXPFileIO::RecurMkDir(const std::string& path)
{
	string p(path);
	int len = p.length();
	if (len < 2) return false;

	if ('\\' == p[len - 1]|| '/' == p[len - 1])
	{
		p = p.substr(0, len - 1);
		len = p.length();
	}
	if (len <= 0) return false;

	if (len <= 3)
	{
		if (FolderExists(p)) return true;
		else return false;
	}

	if (FolderExists(p))return true;

	string parDir = p.substr(0, p.find_last_of('\\/'));

	if (parDir.length() <= 0) return false;

	bool Ret = RecurMkDir(parDir);

	if (Ret)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		Ret = (CreateDirectoryA(p.c_str(), &sa) == TRUE);
		return Ret;
	}
	else
		return false;
}

//TODO:������CString��mfc�������CString����Ҫ����ô��
//ȡ��ָ���ļ����µ��ļ����ļ������ƣ����ݹ飬ֻȡһ��Ŀ¼����ȫ����\\������/��������Դ�\\(�޸���),
// ���ص����ļ���ȫ·��
//������/��Ŀ¼��
void YXPFileIO::GetDirectoryFiles(const string &strFolder,
	vector<string> &strVecFileNames,
	bool do_sort,
	bool OnlyFiles,
	bool OnlyDirectories,
	const string &extInclude, //ָ��ֻ�����ĺ�׺������.
	const string &extExclude) //ָ��Ҫ�ų��ĺ�׺������.
{
	strVecFileNames = GetDirectoryFiles(strFolder, do_sort, OnlyFiles, OnlyDirectories, extInclude, extExclude);
}


std::vector<std::string> YXPFileIO::GetDirectoryFiles(const string &strFolder,
	bool do_sort,
	bool OnlyFiles,
	bool OnlyDirectories,
	const string &extInclude, //ָ��ֻ�����ĺ�׺������.
	const string &extExclude) //ָ��Ҫ�ų��ĺ�׺������.
{
	USES_CONVERSION;
	if (OnlyFiles&&OnlyDirectories)  //���������ѡ�У���ô���������ļ���ֱ��Ĭ�ϲ�ѡ��
	{
		OnlyFiles = false;
		OnlyDirectories = false;
	}
	vector<std::string> strVecFileNames;
	struct _finddata_t filefind;
	string  curr = strFolder + "\\*.*";
	int  done = 0;
	int  handle;
	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)
		return strVecFileNames;

	string tempfolder = strFolder;
	if (strFolder[strFolder.size() - 1] != '\\')
	{
		tempfolder += "\\";
	}

	while (!(done = _findnext(handle, &filefind)))
	{
		if (!strcmp(filefind.name, ".."))  //�ô˷�����һ���ҵ����ļ�����Զ��".."��������Ҫ�����ж�
			continue;
		if (OnlyFiles)
		{
			if (FileExists(tempfolder + filefind.name))
			{
				strVecFileNames.push_back(tempfolder + filefind.name);
			}
		}
		else if (OnlyDirectories)
		{
			CString temp = A2W((tempfolder + filefind.name).c_str());
			if (FileExists(tempfolder + filefind.name))
			{
				strVecFileNames.push_back(tempfolder + filefind.name);
			}
		}
		else
		{
			strVecFileNames.push_back(tempfolder + filefind.name);
		}
	}
	_findclose(handle);
	if (do_sort) //�򵥵�����汾��ֱ�ӵ��õ�string.operator<
		std::sort(strVecFileNames.begin(), strVecFileNames.end());

	if (OnlyDirectories)
		return strVecFileNames; //ֻ����Ŀ¼�Ļ��ͷ�����

				//ȥ������Ҫ�ĺ�׺���ļ�
	if (!extInclude.empty())
	{
		for (auto path = strVecFileNames.begin(); path != strVecFileNames.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //������Ŀ¼
			}

			if (!CheckFileExt(*path, extInclude))
				path = strVecFileNames.erase(path);
			else
				++path;
		}
	}

	//ȥ������Ҫ�ĺ�׺���ļ�
	if (!extExclude.empty())
	{
		for (auto path = strVecFileNames.begin(); path != strVecFileNames.end();)
		{
			if (!FileExists(path->c_str()))
			{
				++path;
				continue; //������Ŀ¼
			}

			if (CheckFileExt(*path, extExclude))
				path = strVecFileNames.erase(path);
			else
				++path;
		}
	}

	return strVecFileNames;
}


//����һ��Ҫ�ټ�鷵��ֵ�Ƿ�Ϊ��!,�������ǻ������
std::string YXPFileIO::BrowseFolder(const std::string & title, const HWND owner)
{
	//TCHAR wchPath[MAX_PATH];     //���ѡ���Ŀ¼·�� 
	char path[MAX_PATH];
	ZeroMemory(path, sizeof(path));
	BROWSEINFOA bi;
	bi.hwndOwner = owner;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;
	bi.lpszTitle = title.c_str();
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolderA(&bi);
	CString cStrPath;
	if (lp && SHGetPathFromIDListA(lp, path))
	{
		return path;
	}
	return "";

}

//ɾ����ǰ�㼶Ŀ¼�������ļ�������ɾ����Ŀ¼
void YXPFileIO::DeleteDirectory(const std::string path, bool delFolder)
{
	if (path.empty() || !FolderExists(path.c_str())) return;
	vector<string> files;
	GetDirectoryFiles(path, files);
	if (!files.empty())
		for (int i = 0; i != files.size(); ++i)
			DeleteFileA(files[i].c_str());
	if (delFolder)
		RemoveDirectoryA(path.c_str());
}

std::string YXPFileIO::GetFileNameNoPath(const std::string &filename) //֧��\\��/��Ŀ¼
{
	int pos = filename.find_last_of("\\/");
	return filename.substr(pos + 1, filename.length() - pos);
}
std::string YXPFileIO::GetFileNameNoExt(const std::string &filename) //ֻ�ж�.��λ�ã�����ȥ��·��
{
	int pos = filename.rfind(".");
	return filename.substr(0, pos);
}
std::string YXPFileIO::GetFileNameExt(const std::string &filename) //���ش�.��
{
	int pos = filename.rfind(".");
	return filename.substr(pos, filename.length() - pos);
}

bool YXPFileIO::CheckFileExt(const std::string& Path, const std::string &ext)
{
	if (Path.empty() || ext.empty()) return false;
	string ext_src = GetFileNameExt(Path);

	string ext_dst(ext);

	std::transform(ext_src.begin(), ext_src.end(), ext_src.begin(), ::tolower); //ת����Сд
	std::transform(ext_dst.begin(), ext_dst.end(), ext_dst.begin(), ::tolower); //ת����Сд

	if (ext_dst.compare(ext_src)) return false;
	return true;

}


//������Ŀ¼���ļ���IsExistReplaceָ������ʱ����滻
bool YXPFileIO::Rename(const std::string &src, const std::string &dst, bool IsExistReplace)
{
	//�ļ�������ֱ�ӷ���false
	if (!FileExists(src) && !FolderExists(src)) return false;
	if ((FileExists(dst) || FolderExists(dst)) && !IsExistReplace)
		return false;

	if (FileExists(dst) && IsExistReplace)
	{
		DeleteFileA(dst.c_str());
	}

	if (FolderExists(dst) && IsExistReplace)
	{
		DeleteDirectory(dst, true); //���ܵݹ�ɾ��Ŀ¼��ֻ��ɾ�������ļ���Ŀ¼
	}


	//win sdk��rename����˵��:
	//1.Ŀ���ļ����Ѿ����ڻ᷵��-1ʧ��
	//2.����Ӧ���Ŀ��Կ�Ŀ¼
	//3.����ֱ��������Ŀ¼���������ļ�Ҳ����,��Ȼͬ��Ŀ��Ŀ¼�Ѵ���(�������ļ���)�ᱨ��
	return	rename(src.c_str(), dst.c_str()) < 0 ? false : true;
}

std::string YXPFileIO::GetAppStdStringPath()
{
	char exeFullPath[MAX_PATH];

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	string pathName(exeFullPath);

	//����ֵ��󲻴�'\\'
	int index = pathName.rfind('\\');
	return pathName.substr(0, index);
}

//��ʵ��Щ�������Է���ֵ����Ϊenum,��ֱ�ۣ����Ƿ���false��̫��������
//Ҫ��dstĿ¼�����Ѿ�����,ֻ�����ļ�
bool YXPFileIO::CopyDirFiles(const std::string & src, const std::string & dst, bool IsExistReplace)
{
	string dstPath(dst);
	if (!FolderExists(src) || dstPath.empty()) return false;

	if (dstPath[dstPath.length() - 1] == '\\')
		dstPath = dstPath.substr(0, dstPath.length() - 1);

	vector<std::string> srcFiles;

	GetDirectoryFiles(src, srcFiles,true,true);
	if (!IsExistReplace) //�������滻������£��ȼ�����ظ��ļ���,��ֹ���ֲ��ֿ���
	{
		vector<std::string> dstFiles;
		GetDirectoryFiles(dstPath, dstFiles, true, true);
		for (auto srcFile : srcFiles)
		{
			for (auto dstFile : dstFiles)
			{
				if (_stricmp(GetFileNameNoPath(srcFile).c_str(),
								GetFileNameNoPath(dstFile).c_str()) == 0)
					return false;
			}
		}
	}
	//CopyFile:
	//If the function succeeds, the return value is nonzero.
	//If the function fails, the return value is zero.To get extended error information, call GetLastError.
	for (auto it=srcFiles.begin();it!=srcFiles.end();++it)
	{
		if (!CopyFileA(it->c_str(), (dst +"\\"+ GetFileNameNoPath(*it)).c_str(), !IsExistReplace))
			return false;
	}
	return true;
}


std::string YXPFileIO::BrowseFile(const std::string strFilter, bool isOpen)
{
	char Buffer[MAX_PATH];
	OPENFILENAMEA   ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = Buffer;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = strFilter.c_str();
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (isOpen) 
	{
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		GetOpenFileNameA(&ofn);
		return Buffer;
	}

	GetSaveFileNameA(&ofn);
	return string(Buffer);

}

CString YXPFileIO::GetAppCStringPath() //������������ȡ�ó�������·��������ֵ��'\\'
{
	//ȡ��Ӧ�ó���·��	
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString pathName(exeFullPath);

	//����ֵ����Դ�'\\'
	int index = pathName.ReverseFind('\\');
	return pathName.Left(index + 1);
}

//���ļ����뵽string��  
bool YXPFileIO::readAllText(const std::string & filename, std::string & txt)
{
	ifstream ifs(filename);
	if (!ifs.is_open()) return false;

	//���ļ����뵽ostringstream����buf��  
	ostringstream buf;
	char ch;
	while (buf&&ifs.get(ch))
		buf.put(ch);
	//������������buf�������ַ���  
	txt = buf.str();
	return true;
}

bool YXPFileIO::writeToText(const std::string & filename, const std::string & txt)
{
	ofstream ofs(filename);
	if (!ofs.is_open()) return false;
	ofs.write(txt.c_str(), txt.length());
	ofs.close();
	return true;
}

