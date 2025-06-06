#pragma once

#include <functional>

class CConsoleIO
{
public:
    // ����ص���������
    using OutputCallback = std::function<void(const CString&)>;

    // ����/����
    CConsoleIO();
    ~CConsoleIO();

    // �����ӽ��̣��������·���ͻص���
    BOOL StartProcess(LPCTSTR szExePath, OutputCallback callback);

    // ���ӽ��̷�������
    void WriteInput(const CString& strInput);

    // ��ֹ�ӽ���
    void TerminateProcess();

private:
    // ��ȡ����̺߳���
    static DWORD WINAPI _ReadOutputThread(LPVOID lpParam);
    void CloseAllPipes();

    // �ܵ����
    HANDLE m_hInputWr = NULL;
    HANDLE m_hInputRd = NULL;
    HANDLE m_hOutputRd = NULL;
    HANDLE m_hOutputWr = NULL;

    // ������Ϣ
    PROCESS_INFORMATION m_piProcInfo = { 0 };

    // �ص�����
    OutputCallback m_OutputCallback;
};