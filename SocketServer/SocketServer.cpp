// Melnicov_Server.cpp : ���� ���� �������� ������� "main". ����� ���������� � ������������� ���������� ���������.
//

#include "pch.h"
#include "Message.h"
#include "Session.h"

map<int, shared_ptr<Session>> sessions;
int ClientID = 100;

// ���������� ���������� �������������
void TimeOut() {
    while (true)
    {
        for (auto i = sessions.begin(); i != sessions.end();) {
            if (sessions.find(i->first) != sessions.end()) {
                if (double(clock() - i->second->getTime()) > 100000) {
                    cout << "������ " << i->first << " ��� ��������" << endl;
                    i = sessions.erase(i);
                }
                else
                    i++;
            }
        }
        Sleep(1000);
    }
}


int createID() {
    return ClientID++;
}

// ������ � ��������
void ProcessClient(SOCKET hSOCK) {
    CSocket s;
    s.Attach(hSOCK);

    Message m;

    switch (m.receive(s)) {
    case INIT: {
        int ID = createID();
        sessions[ID] = make_shared<Session>(ID, clock());
        cout << "������ " << ID << " ���������\n";
        Message::send(s, ID, BROKER, CONFIRM);
        // ���������� ����� � ���������
        Message::SendToStorage(STORAGE, ID, INIT);
        break;
    }
    case EXIT: {
        sessions.erase(m.getHeader().msgFrom);
        cout << "������ " << m.getHeader().msgFrom << " ��������\n";
        Message::send(s, m.getHeader().msgFrom, BROKER, CONFIRM);
        // ���������� ����� � ���������
        Message::SendToStorage(STORAGE, m.getHeader().msgFrom, EXIT);
        break;
    }
    case GETDATA: {
        if (sessions.find(m.getHeader().msgFrom) != sessions.end()) {
            sessions[m.getHeader().msgFrom]->send(s);
            sessions[m.getHeader().msgFrom]->setTime(clock());
        }
        break;
    }
    case GETALLDATA:
    {
        CSocket sock;
        sock.Create();

        if (!sock.Connect("127.0.0.1", 66666))
        {
            DWORD dwError = GetLastError();
            throw runtime_error("������ �����������");
        }

        m.send(sock);
        m.receive(sock);

        if (m.getHeader().msgType == NODATA) {
            m.send(s, m.getHeader().msgFrom, m.getHeader().msgTo, NODATA);
        }
        else
        {
            m.send(s, m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, m.getData());
        }

        sock.Close();
        break;
    }
    default: {
        if (sessions.find(m.getHeader().msgFrom) != sessions.end()) {
            if (sessions.find(m.getHeader().msgTo) != sessions.end()) {
                Message::SendToStorage(m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, m.getData());
                sessions[m.getHeader().msgTo]->Add(m);
            }
            else if (m.getHeader().msgTo == ALL_USER) {
                for (auto& [id, Session] : sessions)
                {
                    if (id != m.getHeader().msgFrom)
                    {
                        // ���������� ����� � ���������
                        Message::SendToStorage(Session->getId(), m.getHeader().msgFrom, TEXT, m.getData());
                        Session->Add(m);
                    }
                }
            }
            Message::send(s, m.getHeader().msgFrom, BROKER, CONFIRM);
        }
        break;
    }
    }
}


void start() {
    AfxSocketInit();
    CSocket Server;
    Server.Create(12345);

    thread tt(TimeOut);
    tt.detach();

    while (true)
    {
        if (!Server.Listen())
            break;
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}

int main()
{
    setlocale(LC_ALL, "");
    std::cout << "������ �������\n" << endl;
    createID();

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
            start();
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
