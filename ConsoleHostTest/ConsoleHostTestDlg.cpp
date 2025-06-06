
// ConsoleHostTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ConsoleHostTest.h"
#include "ConsoleHostTestDlg.h"
#include "afxdialogex.h"
#include "UMiscUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_CONSOLE_OUTPUT    (WM_USER+100)

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CConsoleHostTestDlg 对话框



CConsoleHostTestDlg::CConsoleHostTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONSOLEHOSTTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConsoleHostTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHATBOX, m_edtOutput);
	DDX_Control(pDX, IDC_EDIT_USER_INPUT, m_edtInput);
}

BEGIN_MESSAGE_MAP(CConsoleHostTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_USER_SEND, &CConsoleHostTestDlg::OnBnClickedButtonUserSend)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_CONSOLE_OUTPUT, OnMsgConsoleOutput)
END_MESSAGE_MAP()


// CConsoleHostTestDlg 消息处理程序

BOOL CConsoleHostTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString strCmd(L"D:\\Dev\\ConsoleHostTest\\Debug\\ConsoleApp.exe");
	// 启动子进程（传入程序路径和回调）
	m_ConsoleIO.StartProcess(strCmd,
		std::bind(&CConsoleHostTestDlg::OnConsoleOutput, this, std::placeholders::_1));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CConsoleHostTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CConsoleHostTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CConsoleHostTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConsoleHostTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	//m_ConsoleIO.TerminateProcess(); // 终止子进程
}

static CString gMsg[10]; // 循环使用10个字符串
static int gBufIndex = 0;
void CConsoleHostTestDlg::OnConsoleOutput(const CString& strOutput) 
{
	// 在UI线程更新文本框（使用PostMessage避免跨线程冲突）
	gMsg[gBufIndex] = strOutput;
	PostMessage(WM_CONSOLE_OUTPUT, 0, (LPARAM)gBufIndex);
	if (++gBufIndex >= 10) gBufIndex = 0;
}

LRESULT CConsoleHostTestDlg::OnMsgConsoleOutput(WPARAM wParam, LPARAM lParam)
{
	int bufIndex = lParam;
	m_edtOutput.SetSel(-1, -1); // 滚动到末尾
	m_edtOutput.ReplaceSel(gMsg[bufIndex]); // 添加输出
	return 0;
}

void CConsoleHostTestDlg::OnBnClickedButtonUserSend()
{
	CString strInput;
	m_edtInput.GetWindowText(strInput);
	if (!strInput.IsEmpty()) {
		// 显示输入命令
		m_edtOutput.SetSel(-1, -1);
		m_edtOutput.ReplaceSel(_T("> ") + strInput + _T("\r\n"));
		// 发送输入到子进程
		m_ConsoleIO.WriteInput(strInput);
		m_edtInput.SetWindowText(_T(""));
	}
}
