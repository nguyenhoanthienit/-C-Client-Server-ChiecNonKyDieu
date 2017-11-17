// Server.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"	// HỖ TRỢ LỚP CSOCKET
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX 1024

// The one and only application object

CWinApp theApp;

using namespace std;

// struct LƯU THÔNG TIN NGƯỜI CHƠI 
struct Client
{
	string ID;
	int diem;
	Client(){
		diem = 0;
	}
};
//CÁC HÀM GỬI NHẬN CLIENT-SERVER
void Receive(CSocket &c, const char receive[], int length);		// Hàm gửi
void Send(CSocket &c, const char send[], int length);			// Hàm nhận
void SendToAll(CSocket c[], const char send[], int n);			// Gửi đến toàn bộ người chơi
void guiKetQua(Client arr[], CSocket ds[], int n, string str);	// Gửi kết quả hiện tại đến người chơi

// CÁC HÀM XỬ LÝ CHUỖI
int demSoKhoangTrang(string str);	// đếm số khoảng trắng
string xoaBoKhoangTrangThua(string str);	// xoá khoảng trắng thừa trong chuỗi (hỗ trợ nhập)
int kiemTraID(string ID, Client* arr, int soLuongClient);	// kiểm tra username bị trùng
int kiemTraKiTuDung(char c, string str);	// kiểm tra kí tự đúng
string newUnrevealedString(string str, string unreavealed, char c);	// chuyển dạng *->kí tự nếu đoán đúng
string diemSoHienTai(Client arr[], int n);	// điểm số hiện tại
Client Max(Client arr[], int n);	// tìm người chơi lớn điểm nhất


// =========================CÁC HÀM GỬI NHẬN CLIENT-SERVER================================
// ============ HÀM GỬI ============
void Receive(CSocket &c, const char receive[], int length){
	c.Receive((char*)&length, sizeof(int), 0);
	c.Receive((char*)receive, length + 1, 0);
}

// ============ HÀM NHẬN ============
void Send(CSocket &c, const char send[], int length){
	c.Send(&length, sizeof(int), 0);
	c.Send(send, length + 1, 0);
}

// ============ HÀM GỬI ĐẾN TOÀN BỘ NGƯỜI CHƠI ============
void SendToAll(CSocket c[], const char send[], int n){
	int length = strlen(send);
	for (int i = 0; i < n; i++){
		Send(c[i], send, length);
	}
}
// ============ GỬI KẾT QUẢ ĐẾN NGƯỜI CHƠI ============
void guiKetQua(Client arr[], CSocket ds[], int n, string str){
	str += "\n" + diemSoHienTai(arr, n);
	SendToAll(ds, str.c_str(), n);
}
// =========================================================================================


// =========================CÁC HÀM XỬ LÝ CHUỖI================================
//============ HÀM ĐẾM KHOẢNG TRẮNG ============
int demSoKhoangTrang(string str){
	int dem = 0;
	for (int i = 0; i < str.length(); i++){
		if (str[i] == ' ')
			dem++;
	}
	return dem;
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
// ============ KIỂM TRA USERNAME BỊ TRÙNG ============
int kiemTraID(string ID, Client* arr, int soLuongClient){
	for (int i = 0; i < soLuongClient; i++){
		if (ID == arr[i].ID)	// trùng nhau
			return 0;
	}
	return 1;
}

// ============ KIỂM TRA CÓ BAO NHIÊU KÍ TỰ ĐÚNG TRONG WORD VỚI KÍ TỰ ĐOÁN CỦA NGƯỜI CHƠI ============
int kiemTraKiTuDung(char c, string str){
	int len = str.length();
	int dem = 0;
	c = toupper(c);
	transform(str.begin(), str.end(), str.begin(), ::toupper);	// chuyển chuỗi thành hoa
	for (int i = 0; i < len; i++){
		if (str[i] != ' ' && c == str[i]){
			dem++;
		}
	}
	return dem;
}

// ============ HIỂN THỊ CÁC KÍ TỰ NGƯỜI CHƠI ĐOÁN TRÙNG VỚI KÍ TỰ TRONG WORD 
// CÁC KÍ TỰ CÒN LẠI VẪN ĐỂ DẠNG * ============
// str : chuỗi ban đầu (WORD), unrevealed : chuỗi sau khi chuyển WORD thành *
string newUnrevealedString(string str, string unreavealed, char c){
	for (int i = 0; i < str.length(); i++){
		if (str[i] == ' ')
			continue;
		else if (c == str[i] && unreavealed[i] == '*')
			unreavealed[i] = c;
	}
	return unreavealed;
}

// ============ IN RA TOÀN BỘ ĐIỂM SỐ HIỆN TẠI NGƯỜI CHƠI ============
string diemSoHienTai(Client arr[], int n){
	string tmp = "";
	for (int i = 0; i < n; i++){
		tmp += "Nguoi choi " + arr[i].ID + " - Diem so : " + to_string(arr[i].diem) + "\n";
	}
	return tmp;
}

// ============ TÌM NGƯỜI CHƠI CÓ ĐIỂM SỐ LỚN NHẤT ============
Client Max(Client arr[], int n){
	Client c;
	c = arr[0];
	for (int i = 1; i < n; i++){
		if (arr[i].diem > c.diem)
			c = arr[i];
	}
	return c;
}
// =====================================================================================


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
			// ============ KHAI BÁO VÀ TẠO PORT KẾT NỐI ============
			AfxSocketInit(NULL);
			CSocket server;
			int port = 1703;	
			if (server.Create(port, SOCK_STREAM, NULL) == 0){
				cout << "Khong the ket noi toi server !!!" << endl;
				cout << server.GetLastError();
				return FALSE;
			}
			else{
				cout << "Ket noi thanh cong !!!" << endl << endl;
				cout << "============ CHIEC NON KI DIEU ============" << endl << endl;
			}
			if (server.Listen() == false){
				cout << "Khong the ket noi den port" << endl;
				server.Close();
				return false;
			}

			// ============NHẬP SỐ LƯỢNG NGƯỜI CHƠI============
			int soLuongNguoiChoi;	// số lượng người chơi
			cout << "Nhap so luong nguoi choi : ";
			cin >> soLuongNguoiChoi;

			//============ TẠO CÁC DANH SÁCH QUẢN LÝ CLIENT ============
			Client* arr = new Client[soLuongNguoiChoi];	// hỗ trợ việc lưu điểm và thông tin username
			CSocket* DsClient = new CSocket[soLuongNguoiChoi];	// hỗ trợ việc send, receive với client

			cout << "Doi nguoi choi ket noi ..." << endl;

			// ============ KIỂM TRA USERNAME NGƯỜI CHƠI CÓ BỊ TRÙNG ============
			char mess[MAX];	// lưu tin nhăn gửi và nhận
			int length;	// độ dài tin nhắn
			for (int i = 0; i < soLuongNguoiChoi; i++){
				if (server.Accept(DsClient[i])){
					//gửi lệnh nhập id đến người chơi
					strcpy(mess, "Nhap ID : ");
					length = strlen(mess);
					Send(DsClient[i], mess, length);

					//nhận id từ người chơi
					Receive(DsClient[i], mess, length);
					string id = mess;
					int ktra = kiemTraID(id, arr, i);	//kiểm tra id có bị trùng nhau không
					while (ktra != 1){
						strcpy(mess, "ID da co nguoi su dung");
						length = strlen(mess);
						Send(DsClient[i], mess, length);	//gửi thông báo id bị trùng

						Receive(DsClient[i], mess, length);	//nhận id mới từ người chơi
						ktra = kiemTraID(mess, arr, i);
					}

					arr[i].ID = mess;	//lưu id người chơi

					//gửi thông báo tạo id thành công
					strcpy(mess, "Dang ki username thanh cong!");
					length = strlen(mess);
					Send(DsClient[i], mess, length);

					cout << "Nguoi choi " << arr[i].ID << " vua ket noi" << endl;
				}
			}
			cout << endl << "Du so luong nguoi choi" << endl << endl;

			// ============ SERVER NHẬP VÀO WORD ============
			cout << "Nhap vao 1 tu hoac 1 cau co nghia : ";
			string WORD;	// server nhập WORD vào
			fflush(stdin);
			getline(cin, WORD);
			WORD = xoaBoKhoangTrangThua(WORD);
			transform(WORD.begin(), WORD.end(), WORD.begin(), ::toupper);	// chuyển toàn bộ WORD thành hoa

			cout << "Goi y (neu khong co thi ENTER) : ";	// nhập gợi ý 
			string goiY;
			fflush(stdin);
			getline(cin, goiY);
			goiY = xoaBoKhoangTrangThua(goiY);
			
			//============ CHUYỂN WORD THÀNH DẠNG * VÀ GỬI ĐẾN NGƯỜI CHƠI ============
			string unrevealed = WORD;	// chuyển WORD thành dạng *
			for (int i = 0; i < WORD.length(); i++){
				if (WORD[i] != ' ')
					unrevealed[i] = '*';
			}

			// ============ CÁC BIẾN KHỞI TẠO TRONG GAME ============
			int MARK;	// điểm số (random)
			bool flag;	// kiểm tra tới lượt người chơi nào
			bool ketThucTroChoi = false;	//kiểm tra trò chơi kết thúc
			int soKiTuDung = 0;	//số kí tự đúng
			int soKiTuConLai = WORD.length();	//số kí tự còn lại
			string tmp = "";	// chuỗi tạm lưu giá trị bất kì
			string oChu = "O chu can phai doan la : ";	// hỗ trợ print ra console

			// ============ BẮT ĐẦU GAME ============
			cout << endl << "Bat dau game" << endl;
			while (!ketThucTroChoi){
				for (int i = 0; i < soLuongNguoiChoi; i++){
					tmp = "====== LUOT CHOI CUA NGUOI CHOI " + arr[i].ID + " ======\n";
					tmp += oChu + unrevealed + "\n" + "Goi y : " + goiY + "\n";
					SendToAll(DsClient, tmp.c_str(), soLuongNguoiChoi);	// gửi thông tin đề đến toàn bộ người chơi
					do{
						// gửi flag đến người chơi, flag = true thì tới lượt người đó chơi
						for (int j = 0; j < soLuongNguoiChoi; j++){
							if (j == i)
								flag = true;
							else
								flag = false;
							DsClient[j].Send(&flag, sizeof(bool), 0);
						}
						// Lấy kết quả đoán kí tự của người chơi
						memset(mess, 0, sizeof(mess));	// xoá vùng nhớ của mess
						Receive(DsClient[i], mess, length);
						length = strlen(mess);
						srand(time(NULL));
						MARK = rand() % 10 + 1;	// random điểm

						if (length == 1){
							soKiTuDung = kiemTraKiTuDung(mess[0], WORD);
							//Thông báo các người chơi bằng hàm guiKetQua
							if (soKiTuDung > 0 && unrevealed.find(mess[0]) == -1){
								arr[i].diem += MARK * soKiTuDung;
								tmp = "== Nguoi choi " + arr[i].ID + " doan dung ki tu " + mess[0] + "\n";
								tmp += "Co " + to_string(soKiTuDung) + " chu " + mess[0] + "\n";
								soKiTuConLai -= soKiTuDung;
								unrevealed = newUnrevealedString(WORD, unrevealed, mess[0]);
								if (unrevealed == WORD){
									ketThucTroChoi = true;
									tmp += oChu + unrevealed + "\n\n";
									tmp += "=== TAT CA O CHU DA DUOC LAT RA ===";
									guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
									break;
								}
								guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
								i--;	// giảm i để khi i++ thì i sẽ trờ lại ban đầu, server có thể tiếp tục gửi flag=true cho người choi đó đoán tiếp
							}
							else{
								if (unrevealed.find(mess[0]) != -1){
									tmp = "== Nguoi choi " + arr[i].ID + " vua doan ki tu " + mess[0] + " da duoc hien thi trong WORD dang duoc lat" + "\n== Mat luot !" + "\n";
								}
								else{
									tmp = "== Nguoi choi " + arr[i].ID + " vua doan sai ki tu " + mess[0] + "\n== KHONG CO CHU " + mess[0] + "\n";
								}
								guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
							}
							break;
						}
						else{
							if (strcmp(mess, WORD.c_str()) == 0){
								arr[i].diem += 20 * (soKiTuConLai - demSoKhoangTrang(WORD));
								tmp = "== Nguoi choi " + arr[i].ID + " doan WORD la " + mess + "\n"; 
								tmp += "=== Nguoi choi " + arr[i].ID + " doan dung WORD! ===" + "\n";
								guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
								ketThucTroChoi = true;
							}
							else{
								arr[i].diem += 0;
								tmp = "== Nguoi choi " + arr[i].ID + " doan WORD la " + mess + " . Sai WORD, Mat luot !" + "\n";
								guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
							}
							break;
						}
					} while (soKiTuDung > 0);
					if (ketThucTroChoi == true)	// thông báo kết quả trên server
					{
						Client winner = Max(arr, soLuongNguoiChoi);
						tmp = "TAT CA O CHU DUOC LAT . WORD : " + WORD + "\nTRO CHOI KET THUC ! \nNguoi chien thang la nguoi choi : " + winner.ID + " voi diem so " + to_string(winner.diem) + "\n";
						cout << tmp << endl;
						guiKetQua(arr, DsClient, soLuongNguoiChoi, tmp);
						break;
					}
				}
			}
			server.Close();
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
