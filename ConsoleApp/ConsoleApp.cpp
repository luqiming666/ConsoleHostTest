// ConsoleApp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <conio.h>
#include <string>
#include <cctype>
#include <algorithm>

bool isQ(const std::wstring& str) {
    // 方法1：比较小写形式（忽略大小写）
    //return (str.length() > 0) && (std::tolower(str[0]) == L'q');

    // 方法2：显式检查Q和q（区分大小写）
     return (str.length() > 0) && (str[0] == L'Q' || str[0] == L'q');
}

int main()
{
    // 设置本地化以支持宽字符
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());

    std::wcout << L"输入字符，按Enter确认，按Q退出程序。\n" << std::endl;

    /*char ch;
    while (true) {
        ch = _getch();
        if (ch == 'Q' || ch == 'q') {
            break;
        }
        std::cout << ch;
        if (ch == '\r') {
            std::cout << "\nI got it. Please continue...\n" << std::endl;
        }
    }*/

    std::wstring line;
    while (true) {
        getline(std::wcin, line);  // 读取整行，直到换行符（换行符被丢弃）

        //std::wcout << L"The input has " << line.size() << L" characters. They are: " << line << std::endl;
        //std::wcout.write(line.c_str(), line.length());  // 避免line中的\0 导致输出中断
        //std::wcout << std::endl;  // 手动添加换行符

        if (isQ(line)) {
            break;
        }        

        std::wcout << L"I got it. Please continue..." << std::endl;
    }

    std::wcout << L"\n程序已退出。\n" << std::endl;
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
