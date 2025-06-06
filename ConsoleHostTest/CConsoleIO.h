#pragma once

#include <functional>

class CConsoleIO
{
public:
    // 输出回调函数类型
    using OutputCallback = std::function<void(const CString&)>;

    // 构造/析构
    CConsoleIO();
    ~CConsoleIO();

    // 启动子进程（传入程序路径和回调）
    BOOL StartProcess(LPCTSTR szExePath, OutputCallback callback);

    // 向子进程发送输入
    void WriteInput(const CString& strInput);

    // 终止子进程
    void TerminateProcess();

private:
    // 读取输出线程函数
    static DWORD WINAPI _ReadOutputThread(LPVOID lpParam);
    void CloseAllPipes();

    // 管道句柄
    HANDLE m_hInputWr = NULL;
    HANDLE m_hInputRd = NULL;
    HANDLE m_hOutputRd = NULL;
    HANDLE m_hOutputWr = NULL;

    // 进程信息
    PROCESS_INFORMATION m_piProcInfo = { 0 };

    // 回调函数
    OutputCallback m_OutputCallback;
};