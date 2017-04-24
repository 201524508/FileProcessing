#include <iostream>
#include <direct.h>
#include <Windows.h>
#include <shellapi.h>
#include <atlstr.h>
#include <string>
#include <tchar.h>
#include <io.h>

using namespace std;

int CopyDir(TCHAR* src, TCHAR* dest);	//directory ����
int updir(TCHAR* src, TCHAR* dest);	//������Ʈ - ����(���ѷ���)
int UpDataTime(TCHAR* aaa);	//���� �ð�

int _tmain(int argc, TCHAR *argv[]) {
	if (argc != 3) {
		cout << "error!\n";
		return 1;
	}

	TCHAR* src = argv[1];	//source path
	TCHAR* dest = argv[2];	//destination path

	if (!CreateDirectory((LPCWSTR)dest, NULL)) {	//directory ����
		int n = GetLastError();
		if (n == ERROR_ALREADY_EXISTS) {	//�����ϴ� directory -> ������Ʈ
			cout << "exist directory\n";
			updir(src, dest);
		}
		else if (n == ERROR_PATH_NOT_FOUND) {	//�߸��� path
			cout << "error for path\n";
			return 1;
		}
		else {
			cout << "error for create directory\n";
			return 1;
		}
	}
	else {	//directory�� �����Ǹ�
		int nD = 0;
		nD = CopyDir(src, dest);	//directory ����
		if (nD == 0) {
			cout << "complete copy directory\n";
		}
		else {
			cout << "failed copy\n";
			return 1;
		}
	}

	return 0;
}

int CopyDir(TCHAR* src, TCHAR* dest) {	//directory ����
	int result = 1;

	TCHAR* src_path = new TCHAR[_tcslen(src) + 2]();	//source path
	TCHAR* dest_path = new TCHAR[_tcslen(dest) + 2]();	//destination path
	/*path �� 2�ڸ��� NULL ����*/

	_tcscat(src_path, src);
	_tcscat(dest_path, dest);

	/*source directory�� ��°�� destination path�� ����*/
	SHFILEOPSTRUCT sf;
	sf.hwnd = NULL;
	sf.wFunc = FO_COPY;
	sf.pFrom = (PCZZWSTR)src_path;
	sf.pTo = (PCZZWSTR)dest_path;
	sf.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	sf.fAnyOperationsAborted = false;
	sf.hNameMappings = NULL;
	sf.lpszProgressTitle = NULL;

	result = SHFileOperation(&sf);	//success : 0, non-success : !0

	cout << result << endl;

	return result;
}

int updir(TCHAR* src, TCHAR* dest) {	//������ �κ� ������Ʈ - ����(���ѷ���)
	cout << src<<endl;
	cout << dest << endl;

	int result = 1;
	WIN32_FIND_DATA fd_s;
	WIN32_FIND_DATA fd_d;
	HANDLE hf_s = FindFirstFile(src, &fd_s);
	HANDLE hf_d = FindFirstFile(dest, &fd_d);

	if (hf_s == INVALID_HANDLE_VALUE || hf_d == INVALID_HANDLE_VALUE) {
		//_tprintf(TEXT("1 %s\n"), fd_s.cFileName);
		//_tprintf(TEXT("2 %s\n"), fd_d.cFileName);

		cout << "FindFirstFile failed "<< GetLastError()<<endl;
		return result;
	}

	if (GetFileAttributes((LPCWSTR)src) == FILE_ATTRIBUTE_DIRECTORY && GetFileAttributes((LPCWSTR)dest) == FILE_ATTRIBUTE_DIRECTORY) {	//src, dest�� ����
		TCHAR* n_src = new TCHAR[MAX_PATH];
		TCHAR* n_dest = new TCHAR[MAX_PATH];

		_tcscpy(n_src, src);
		_tcscpy(n_dest, dest);
		//_tprintf(TEXT("5 %s\n"), n_src);
		//_tprintf(TEXT("6 %s\n"), n_dest);
		updir(n_src, n_dest);
	}
	else if (GetFileAttributes((LPCWSTR)src) == FILE_ATTRIBUTE_ARCHIVE && GetFileAttributes((LPCWSTR)dest) == FILE_ATTRIBUTE_ARCHIVE) {	//src, dest�� �Ϲ� ����
		if ((LPCWSTR)fd_s.cFileName == (LPCWSTR)fd_d.cFileName) {
			int updatefile = UpDataTime(src);
			int backupfile = UpDataTime(dest);

			if (updatefile > backupfile) {
				if (!CopyFile((LPCWSTR)src, (LPCWSTR)dest, NULL)) {
					cout << "copy file error\n";
					return result;
				}
				else {
					cout << "copy file success\n";
					result = 0;
				}
			}
			else { //������Ʈ �ʿ�x
				cout << "No need to update" << endl;
			}

		}

		if (FindNextFile(hf_s, &fd_s) && FindNextFile(hf_d, &fd_d)) {
			TCHAR* n_src = new TCHAR[MAX_PATH];
			TCHAR* n_dest = new TCHAR[MAX_PATH];
			
			_tcscpy(n_src, src);
			_tcscpy(n_dest, dest);
			_tcscat(n_src, TEXT("\\*"));
			_tcscat(n_dest, TEXT("\\*"));
			_tcscat(n_src, fd_s.cFileName);
			_tcscat(n_dest, fd_d.cFileName);

			updir(n_src, n_dest);
			
			delete[] n_src;
			delete[] n_dest;
		}
		else {
			return result;
		}

		FindClose(hf_s);
		FindClose(hf_d);
	}
	return result;
}

int UpDataTime(TCHAR *aaa) {
	//filename
	TCHAR* fname = aaa;

	//temporary storage for file sizes
	DWORD dwFileSize;
	DWORD dwFileType;

	//the files handle
	HANDLE hFile1;
	FILETIME ftCreate, ftAccess, ftWrite;
	SYSTEMTIME stUTC, stLocal, stUTC1, stLocal1, stUTC2, stLocal2;

	//Opening the existing file
	hFile1 = CreateFile((LPCTSTR)aaa,           //file to open 
		GENERIC_READ,         //open for reading 
		FILE_SHARE_READ,      //share for reading 
		NULL,                 //default security 
		OPEN_EXISTING,        //existing file only 
		FILE_FLAG_BACKUP_SEMANTICS, //for directory
		NULL);                 //no attribute template 

	if (hFile1 == INVALID_HANDLE_VALUE) {
		printf("Could not open %s file, error %d\n", aaa/*fname1*/, GetLastError());
		return 4;
	}

	dwFileType = GetFileType(hFile1);
	dwFileSize = GetFileSize(hFile1, NULL);
	printf("%s size is %d bytes and file type is %d\n", aaa/*fname1*/, dwFileSize, dwFileType);

	//Retrieve the file times for the file.
	if (!GetFileTime(hFile1, &ftCreate, &ftAccess, &ftWrite)) {
		printf("Something wrong lol!\n");
		return FALSE;
	}

	//Convert the last-write time to local time.
	FileTimeToSystemTime(&ftWrite, &stUTC2);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC2, &stLocal2);

	//Build a string showing the date and time.
	printf("Last written: %02d/%02d/%d %02d:%02d\n", stLocal2.wMonth, stLocal2.wDay,
		stLocal2.wYear, stLocal2.wHour, stLocal2.wMinute);

	FILE *fout = fopen("lastdata.txt", "w");

	fprintf(fout, "%02d%02d%d%02d%02d\n", stLocal2.wMonth, stLocal2.wDay,
		stLocal2.wYear, stLocal2.wHour, stLocal2.wMinute);
	fclose(fout);

	FILE *fin = fopen("lastdata.txt", "r");
	char lastdata[100];

	if (fin == NULL) {//������Ʈ �� ������ ������
		cout << "no update" << endl;
	}
	else { //������Ʈ ������ ������
		if (fgets(lastdata, sizeof(lastdata), fin) != NULL) {
			puts(lastdata);//�ð��� �����´�
		}
		//printf("%s", lastdata[0]); // mm dd yy hh:mm
		fclose(fin);
	}

	CloseHandle(hFile1);

	return lastdata[0];
}