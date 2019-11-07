#include <QApplication>
#include <qsplashscreen.h>
#include "mainwindow.h"
#include <Windows.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qstringlist.h>
using namespace std;

size_t ExecuteProcess(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait);

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//qApp->addLibraryPath("./Lib");
	//qApp->setLibraryPaths(paths);

	//ExecuteProcess(L"C:\Program Files (x86)\Adobe\Acrobat 9.0\Acrobat",NULL,60);

	/*
	const QString program = "C:\\Program Files (x86)\\CambridgeSoft\\ChemOffice2006\\ChemDraw\\ChemDraw.exe";
	// create a qprocess for the matlab process
	QProcess chemDraw;
	chemDraw.startDetached(program,QStringList());
	QStringList env = chemDraw.environment();
	QString temp = env.join("\n");
	QMessageBox::information(0,"ELISA Bioo",temp);*/
	
	/*
	PROCESS_INFORMATION pi;
	STARTUPINFO si = {sizeof(si)};
	TCHAR cCommandLine[_MAX_PATH];
	//_stprintf(cCommandLine, _T("notepad.exe \"%s\""),m_filePath);
	//LPCWSTR name = L"notepad.exe \"%s\"";
	//LPCWSTR name = L"C:\\Program Files (x86)\\Adobe\\Acrobat 9.0\\Acrobat\\Acrobat.exe";
	LPCWSTR name = L"C:\\Program Files (x86)\\CambridgeSoft\\ChemOffice2006\\ChemDraw\\ChemDraw.exe";
	if (CreateProcess(name,NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		//QMessageBox::information(0,"ELISA Bioo","ChemDraw opened.");
		WaitForSingleObject(pi.hProcess, 10000);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else {
		 int iReturnVal = GetLastError();
		 QMessageBox::information(0,"ELISA Bioo",QString("Error: %1").arg(iReturnVal));
	}*/


	/*
	STARTUPINFO sinfo;
	PROCESS_INFORMATION pinfo;
	memset(&sinfo,0,sizeof(sinfo));
	memset(&pinfo,0,sizeof(pinfo));
	sinfo.cb = sizeof(sinfo);
	//LPWSTR programName = L"\"D:\\Program Files\\Diablo II\\game.exe\"";
	LPWSTR programName = L"\"C:\\Program Files (x86)\\Adobe\\Acrobat 9.0\\Acrobat\\Acrobat.exe\"";

	if( CreateProcess(programName,0, 0,0,0,0,0,0,0,&pinfo) == 0)
	{ 
		//::WaitForSingleObject(pinfo.hProcess, 100000); 
		//CloseHandle(pinfo.hProcess); 
		//CloseHandle(pinfo.hThread); 
		QMessageBox::information(0,"ELISA Bioo","This function will be implemented soon.");
	//QWebView *view = new QWebView(this);
		//printf("Create Process failed\n");
	}*/




	MainWindow *mainWin = new MainWindow;
	mainWin->show();
	return app.exec();
}

size_t ExecuteProcess(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait) 
{ 
    size_t iMyCounter = 0, iReturnVal = 0, iPos = 0; 
    DWORD dwExitCode = 0; 
    std::wstring sTempStr = L""; 

    /* - NOTE - You should check here to see if the exe even exists */

     /* Add a space to the beginning of the Parameters */
     if (Parameters.size() != 0) 
    { 
        if (Parameters[0] != L' ') 
        { 
            Parameters.insert(0,L" "); 
        } 
    } 

    /* The first parameter needs to be the exe itself */
     sTempStr = FullPathToExe; 
    iPos = sTempStr.find_last_of(L"\\"); 
    sTempStr.erase(0, iPos +1); 
    Parameters = sTempStr.append(Parameters); 

     /* CreateProcessW can modify Parameters thus we allocate needed memory */
     wchar_t * pwszParam = new wchar_t[Parameters.size() + 1]; 
    if (pwszParam == 0) 
    { 
        return 1; 
    } 
    const wchar_t* pchrTemp = Parameters.c_str(); 
    wcscpy_s(pwszParam, Parameters.size() + 1, pchrTemp); 

    /* CreateProcess API initialization */
     STARTUPINFOW siStartupInfo; 
    PROCESS_INFORMATION piProcessInfo; 
    memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
    memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
    siStartupInfo.cb = sizeof(siStartupInfo); 

    if (CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()), 
                            pwszParam, 0, 0, false, 
                            CREATE_DEFAULT_ERROR_MODE, 0, 0, 
                            &siStartupInfo, &piProcessInfo) != false) 
    { 
         /* Watch the process. */
         dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000)); 
    } 
    else
     { 
        /* CreateProcess failed */
         iReturnVal = GetLastError(); 
    } 

    /* Free memory */
     delete[]pwszParam; 
    pwszParam = 0; 

    /* Release handles */
     CloseHandle(piProcessInfo.hProcess); 
    CloseHandle(piProcessInfo.hThread); 

    return iReturnVal; 
} 