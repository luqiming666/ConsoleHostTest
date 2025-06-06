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
    // ��ʼ����ȫ���ԣ��������̳У�
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // ��������ܵ����ӽ������ �� �����̶�ȡ��
    if (!CreatePipe(&m_hOutputRd, &m_hOutputWr, &saAttr, 0)) return FALSE;

    // ��������ܵ���������д�� �� �ӽ������룩
    if (!CreatePipe(&m_hInputRd, &m_hInputWr, &saAttr, 0)) {
        CloseAllPipes();
        return FALSE;
    }

    // ����������Ϣ���ض����׼����/�����
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_NORMAL; // �����ӽ��̿���̨����
    si.hStdInput = m_hInputRd;
    si.hStdOutput = m_hOutputWr;
    si.hStdError = m_hOutputWr; // ��������ض�������ܵ�

    // �����ӽ���
    if (!::CreateProcess(szExePath, NULL, NULL, NULL, TRUE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &m_piProcInfo)) {
        CloseAllPipes();
        return FALSE;
    }

    // ����ص���������ȡ�߳�
    m_OutputCallback = callback;
    CreateThread(NULL, 0, _ReadOutputThread, this, 0, NULL);
    return TRUE;
}

void CConsoleIO::WriteInput(const CString& strInput) 
{
    if (!m_hInputWr) return;

    CString str = strInput + _T("\n"); // ��ӻ��з�ģ���������
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
        pThis->m_OutputCallback(CString(chBuf)); // ���ûص���������
    }
    return 0;
}