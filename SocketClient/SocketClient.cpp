// Melnicov_Client_C++.cpp : ���� ���� �������� ������� "main". ����� ���������� � ������������� ���������� ���������.
//

#include "pch.h"
#include "Message.h"

int MyID;

void connect(CSocket& S) {
    S.Create();
    S.Connect(_T("127.0.0.1"), 12345);
}

void disconnect(CSocket& S) {
    S.Close();
}

void GetData() {
    while (true) {
        CSocket s;
        connect(s);
        Message m;
        Message::send(s, BROKER, MyID, GETDATA);
        if (m.receive(s) == TEXT) {
            cout << "��������� �� " << m.getHeader().msgFrom << ": " << m.getData() << endl;
        }
        disconnect(s);
        Sleep(1000);
    }
}

void Client() {
    // ������������� ������� � �����������
    AfxSocketInit();
    CSocket socket;
    connect(socket);
    Message::send(socket, 0, 0, INIT);
    Message m;
    if (m.receive(socket) == CONFIRM) {
        MyID = m.getHeader().msgTo;
        cout << "ID: " << MyID << endl;
        thread t(GetData);
        t.detach();
    }
    else {
        cout << "������" << endl;
        return;
    }
    disconnect(socket);

    // �������� � ����� ���������
    while (true) {
        cout << "1)��������� ���������\n2) �������� ��� ���������\n3) �����\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int ClientID = ALL_USER;
            cout << "1) ������ ��� ������ �������\n2) ��� ���� ��������\n" << endl;
            int choice2;
            cin >> choice2;

            if (choice2 == 1) {
                cout << "\n������� ID �������\n";
                cin >> ClientID;
            }


            cout << "\n������� ���������\n";
            string str;
            cin.ignore();
            getline(cin, str);
            connect(socket);
            Message::send(socket, ClientID, MyID, TEXT, str);
            if (m.receive(socket) == CONFIRM) {
                cout << "\n�������\n" << endl;
            }
            else {
                cout << "\n������\n" << endl;
            }
            disconnect(socket);

            break;
        }
        case 2: {
            CSocket s;
            s.Create();

            if (!s.Connect("127.0.0.1", 12345))
            {
                DWORD dwError = GetLastError();
                throw runtime_error("������ �����������");
            }

            m.send(s, MyID, MyID, GETALLDATA);

            m.receive(s);

            if (m.getHeader().msgType == NODATA) {
                cout << "\n����� ����\n" << endl;
            }
            else
            {
                cout << "\n\n������� ���������\n" << endl;
                size_t pos = 0;
                string hist_idfrom, hist_idto;
                string history_message;

                string data = m.getData();

                while ((pos = data.find("#")) != std::string::npos) {
                    hist_idto = data.substr(0, pos);
                    data.erase(0, pos + 1);
                    hist_idfrom = data.substr(0, pos);
                    data.erase(0, pos + 1);

                    pos = data.find("#");

                    history_message = data.substr(0, pos);
                    data.erase(0, pos + 1);
                    if (MyID == stoi(hist_idto.c_str())) {
                        cout << "��������� �� ������� " << hist_idfrom << " : " << history_message << endl;
                    }
                    else {
                        cout << "��� ����� ������� " << hist_idto << " : " << history_message << endl;
                    }
                }

                /*for (int i = 0; i < length; i++) {

                    m.receive(s);

                    if (m.getHeader().msgTo == MyID) {
                        cout << "��������� �� ������� " << m.getHeader().msgFrom << " : " << m.getData() << endl;
                    }
                    else {
                        cout << "��� ����� ������� " << m.getHeader().msgTo << " : " << m.getData() << endl;
                    }

                }*/

                cout << "\n" << endl;
            }

            disconnect(s);

            s.Close();

            break;
        }
        case 3: {
            connect(socket);
            Message::send(socket, BROKER, MyID, EXIT);
            if (m.receive(socket) == CONFIRM) {
                cout << "\n�������\n" << endl;
            }
            else {
                cout << "\n������\n" << endl;
            }
            disconnect(socket);
            return;
        }
        default:
            break;
        }
    }

}

int main()
{
    setlocale(LC_ALL, "");
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // ���������������� MFC, � ����� ������ � ��������� �� ������� ��� ����
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: �������� ���� ��� ��� ����������.
            wprintf(L"����������� ������: ���� ��� ������������� MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: �������� ���� ��� ��� ����������.
            std::cout << "������ ���������\n" << endl;
            Client();
        }
    }
    else
    {
        // TODO: �������� ��� ������ � ������������ � �������������
        wprintf(L"����������� ������: ���� GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}

// ������ ���������: CTRL+F5 ��� ���� "�������" > "������ ��� �������"
// ������� ���������: F5 ��� ���� "�������" > "��������� �������"

// ������ �� ������ ������ 
//   1. � ���� ������������ ������� ����� ��������� ����� � ��������� ���.
//   2. � ���� Team Explorer ����� ������������ � ������� ���������� ��������.
//   3. � ���� "�������� ������" ����� ������������� �������� ������ ������ � ������ ���������.
//   4. � ���� "������ ������" ����� ������������� ������.
//   5. ��������������� �������� ������ ���� "������" > "�������� ����� �������", ����� ������� ����� ����, ��� "������" > "�������� ������������ �������", ����� �������� � ������ ������������ ����� ����.
//   6. ����� ����� ������� ���� ������ �����, �������� ������ ���� "����" > "�������" > "������" � �������� SLN-����.
