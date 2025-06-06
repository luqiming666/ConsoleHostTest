
// ConsoleHostTestDlg.h: 头文件
//

#pragma once

#include "CConsoleIO.h"

// CConsoleHostTestDlg 对话框
class CConsoleHostTestDlg : public CDialogEx
{
// 构造
public:
	CConsoleHostTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONSOLEHOSTTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	CConsoleIO m_ConsoleIO;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUserSend();
	LRESULT OnMsgConsoleOutput(WPARAM wParam, LPARAM lParam);

	// 回调函数：接收子进程输出
	void OnConsoleOutput(const CString& strOutput);

private:
	CEdit m_edtOutput;
	CEdit m_edtInput;
public:
	afx_msg void OnDestroy();
};
