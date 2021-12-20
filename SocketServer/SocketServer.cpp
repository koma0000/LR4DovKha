// Melnicov_Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "Message.h"
#include "Session.h"

map<int, shared_ptr<Session>> sessions;
int ClientID = 100;

// Отключение неактивных пользователей
void TimeOut() {
    while (true)
    {
        for (auto i = sessions.begin(); i != sessions.end();) {
            if (sessions.find(i->first) != sessions.end()) {
                if (double(clock() - i->second->getTime()) > 100000) {
                    cout << "Клиент " << i->first << " был отключен" << endl;
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

// Работа с клиентом
void ProcessClient(SOCKET hSOCK) {
    CSocket s;
    s.Attach(hSOCK);

    Message m;

    switch (m.receive(s)) {
    case INIT: {
        int ID = createID();
        sessions[ID] = make_shared<Session>(ID, clock());
        cout << "Клиент " << ID << " подключен\n";
        Message::send(s, ID, BROKER, CONFIRM);
        // Отправляем копию в хранилище
        Message::SendToStorage(STORAGE, ID, INIT);
        break;
    }
    case EXIT: {
        sessions.erase(m.getHeader().msgFrom);
        cout << "Клиент " << m.getHeader().msgFrom << " отключен\n";
        Message::send(s, m.getHeader().msgFrom, BROKER, CONFIRM);
        // Отправляем копию в хранилище
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
            throw runtime_error("Ошибка подключения");
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
                        // Отправляем копию в хранилище
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
    std::cout << "Сервер запущен\n" << endl;
    createID();

    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            start();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
