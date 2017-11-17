// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"	// HỖ TRỢ LỚP CSOCKET
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX 1024
// The one and only application object

CWinApp theApp;

using namespace std;

void Receive(CSocket &c, const char receive[], int length);		// Hàm gửi
void Send(CSocket &c, const char send[], int length);			// Hàm nhận
string xoaBoKhoangTrangThua(string str);	// xoá khoảng trắng thừa trong chuỗi (hỗ trợ nhập)


// ============ HÀM NHẬN ============
void Receive(CSocket &c, const char receive[], int length){
	c.Receive((char*)&length, sizeof(int), 0);
	c.Receive((char*)receive, length + 1, 0);
}

// ============ HÀM GỬI ============
void Send(CSocket &c, const char send[], int length){
	c.Send(&length, sizeof(int), 0);
	c.Send(send, length + 1, 0);
}

// =========== HÀM XOÁ BỎ KHOẢNG TRẮNG THỪA =============
string xoaBoKhoangTrangThua(string str){
	// xoá \t
	for (int i = 0; i < str.length(); i++){
		if (str[i] == '\t'){
			str[i] = ' ';
		}
	}
	// xoá khoảng trắng cuối
	while (str[str.length() - 1] == ' '){
		str.erase(str.length() - 1, 1);
	}
	// xoá khoảng trắng đầu
	while (str[0] == ' ')
		str.erase(0, 1);
	//xoá khoảng trắng giữa
	for (int i = 0; i < str.length(); i++){
		if (str[i] == ' ' && str[i + 1] == ' '){
			while (str[i + 1] == ' '){
				str.erase(i + 1, 1);
			}
		}
	}
	return str;
}

// =========================================================================

// ============ MAIN ============
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// ============ KHAI BÁO VÀ KẾT NỐI SERVER ============
			AfxSocketInit(NULL);
			CSocket client;
			client.Create();

			char mess[MAX];	// lưu tin nhăn gửi và nhận
			int length;	// độ dài tin nhắn
			if (client.Connect(CA2W("127.0.0.1"), 1703)){
				cout << "Da ket noi toi server !!!" << endl << endl;
				cout << "CHIEC NON KY DIEU" << endl;

				//nhận thông báo đăng kí username
				Receive(client, mess, length);
				cout << mess;

				// nhập và gửi ID user đến server
				cin.getline(mess, MAX);
				length = strlen(mess);
				Send(client, mess, length);

				// nhận phản hổi đăng kí id
				Receive(client, mess, length);

				// đăng kí username không thành công phải nhập lại
				while (strcmp(mess, "Dang ki username thanh cong!") != 0){
					cout << mess << endl;
					cout << "Nhap lai username : ";
					fflush(stdin);
					cin.getline(mess, MAX);
					// tiếp tục gửi và nhận phản hổi đăng kí id
					length = strlen(mess);
					Send(client, mess, length);
					Receive(client, mess, length);
				}
				cout << mess << endl;
				cout << "Da ket noi thanh cong den server" << endl << endl;
				

				// ============ TỚI LƯỢT CHƠI ============
				bool flag;	// kiểm tra tới lượt người chơi nào
				strcpy(mess, "");
				string tmp = "";
				do{
					Receive(client, mess, length);	// nhận thông báo tới lượt người chơi nào
					cout << mess << endl;
					client.Receive((char*)&flag, sizeof(bool), 0);	// nhận flag, flag = true thì tới lượt chơi
					if (flag){	// flag = true
						cout << "==Xin moi nguoi choi doan==" << endl;
						cout << "Moi ban doan 1 ki tu hoac doan toan bo WORD : ";
						fflush(stdin);
						cin.getline(mess, MAX);
						strupr(mess);
						tmp = mess;
						tmp = xoaBoKhoangTrangThua(tmp);
						Send(client, tmp.c_str(), tmp.length());
					}
					//thông báo kết quả và điểm hiện tại
					Receive(client, mess, length);
					cout << mess << endl << endl;
				} while (true);
			}
			else{
				cout << "Khong co ket noi ..." << endl << "Kiem tra lai server ..." << endl << "Hay chac chan server da khoi dong ..." << endl;
			}
			client.Close();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	system("pause");
	return nRetCode;
}
