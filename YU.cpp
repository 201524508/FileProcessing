#include <iostream>
#include <direct.h>
#include <Windows.h>
#include <shellapi.h>
#include <atlstr.h>
#include <string>
#include <tchar.h>
#include <io.h>

using namespace std;

int CopyDir(TCHAR* src, TCHAR* dest);	//directory 복사
int updir(TCHAR* src, TCHAR* dest);	//업데이트 - 에러(무한루프)
int UpDataTime(TCHAR* aaa);	//파일 시간

int _tmain(int argc, TCHAR *argv[]) {
	if (argc != 3) {
		cout << "error!\n";
		return 1;
	}

	TCHAR* src = argv[1];	//source path
	TCHAR* dest = argv[2];	//destination path

	if (!CreateDirectory((LPCWSTR)dest, NULL)) {	//directory 생성
		int n = GetLastError();
		if (n == ERROR_ALREADY_EXISTS) {	//존재하는 directory -> 업데이트
			cout << "exist directory\n";
			updir(src, dest);
		}
		else if (n == ERROR_PATH_NOT_FOUND) {	//잘못된 path
			cout << "error for path\n";
			return 1;
		}
		else {
			cout << "error for create directory\n";
			return 1;
		}
	}
	else {	//directory가 생성되면
		int nD = 0;
		nD = CopyDir(src, dest);	//directory 복사
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

int CopyDir(TCHAR* src, TCHAR* dest) {	//directory 복사
	int result = 1;

	TCHAR* src_path = new TCHAR[_tcslen(src) + 2]();	//source path
	TCHAR* dest_path = new TCHAR[_tcslen(dest) + 2]();	//destination path
	/*path 끝 2자리는 NULL 문자*/

	_tcscat(src_path, src);
	_tcscat(dest_path, dest);

	/*source directory를 통째로 destination path에 복사*/
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

int updir(TCHAR* src, TCHAR* dest) {	//수정된 부분 업데이트 - 에러(무한루프)
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

	if (GetFileAttributes((LPCWSTR)src) == FILE_ATTRIBUTE_DIRECTORY && GetFileAttributes((LPCWSTR)dest) == FILE_ATTRIBUTE_DIRECTORY) {	//src, dest가 폴더
		TCHAR* n_src = new TCHAR[MAX_PATH];
		TCHAR* n_dest = new TCHAR[MAX_PATH];

		_tcscpy(n_src, src);
		_tcscpy(n_dest, dest);
		//_tprintf(TEXT("5 %s\n"), n_src);
		//_tprintf(TEXT("6 %s\n"), n_dest);
		updir(n_src, n_dest);
	}
	else if (GetFileAttributes((LPCWSTR)src) == FILE_ATTRIBUTE_ARCHIVE && GetFileAttributes((LPCWSTR)dest) == FILE_ATTRIBUTE_ARCHIVE) {	//src, dest가 일반 파일
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
			else { //업데이트 필요x
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

	if (fin == NULL) {//업데이트 된 파일이 없으면
		cout << "no update" << endl;
	}
	else { //업데이트 파일이 있으면
		if (fgets(lastdata, sizeof(lastdata), fin) != NULL) {
			puts(lastdata);//시간을 가져온다
		}
		//printf("%s", lastdata[0]); // mm dd yy hh:mm
		fclose(fin);
	}

	CloseHandle(hFile1);

	return lastdata[0];
}