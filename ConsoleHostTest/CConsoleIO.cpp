#include "pch.h"
#include "CConsoleIO.h"

CConsoleIO::CConsoleIO() 
{
}

CConsoleIO::~CConsoleIO() 
{
    TerminateProcess();

    CloseAllPipes();
}

void CConsoleIO::CloseAllPipes()
{
    if (m_hInputWr) {
        CloseHandle(m_hInputWr);
        m_hInputWr = NULL;
    }
    if (m_hInputRd) {
        CloseHandle(m_hInputRd);
        m_hInputRd = NULL;
    }
    if (m_hOutputRd) {
        CloseHandle(m_hOutputRd);
        m_hOutputRd = NULL;
    }
    if (m_hOutputWr) {
        CloseHandle(m_hOutputWr);
        m_hOutputWr = NULL;
    }
}

BOOL CConsoleIO::StartProcess(LPCTSTR szExePath, OutputCallback callback) 
{
    // 初始化安全属性（允许句柄继承）
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // 创建输出管道（子进程输出 → 主进程读取）
    if (!CreatePipe(&m_hOutputRd, &m_hOutputWr, &saAttr, 0)) return FALSE;

    // 创建输入管道（主进程写入 → 子进程输入）
    if (!CreatePipe(&m_hInputRd, &m_hInputWr, &saAttr, 0)) {
        CloseAllPipes();
        return FALSE;
    }

    // 配置启动信息（重定向标准输入/输出）
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_NORMAL; // 隐藏子进程控制台窗口
    si.hStdInput = m_hInputRd;
    si.hStdOutput = m_hOutputWr;
    si.hStdError = m_hOutputWr; // 错误输出重定向到输出管道

    // 启动子进程
    if (!::CreateProcess(szExePath, NULL, NULL, NULL, TRUE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &m_piProcInfo)) {
        CloseAllPipes();
        return FALSE;
    }

    // 保存回调并启动读取线程
    m_OutputCallback = callback;
    CreateThread(NULL, 0, _ReadOutputThread, this, 0, NULL);
    return TRUE;
}

void CConsoleIO::WriteInput(const CString& strInput) 
{
    if (!m_hInputWr) return;

    CString str = strInput + _T("\n"); // 添加换行符模拟键盘输入
    DWORD bytesToWrite = str.GetLength() * sizeof(TCHAR);
    BOOL ret = WriteFile(m_hInputWr, (LPCTSTR)str, bytesToWrite, NULL, NULL);
}

void CConsoleIO::TerminateProcess() 
{
    if (m_piProcInfo.hProcess) {
        ::TerminateProcess(m_piProcInfo.hProcess, 0);
        CloseHandle(m_piProcInfo.hProcess);
        CloseHandle(m_piProcInfo.hThread);
        m_piProcInfo = { 0 };
    }
}

DWORD WINAPI CConsoleIO::_ReadOutputThread(LPVOID lpParam) 
{
    CConsoleIO* pThis = (CConsoleIO*)lpParam;
    CHAR chBuf[4096] = { 0 };
    DWORD dwRead = 0;

    while (pThis->m_hOutputRd &&
        ReadFile(pThis->m_hOutputRd, chBuf, sizeof(chBuf) - 1, &dwRead, NULL) &&
        dwRead > 0) {
        chBuf[dwRead] = '\0';
        pThis->m_OutputCallback(CString(chBuf)); // 调用回调传递数据
    }
    return 0;
}