

#include "pch.h"
#include "Message.h"
#include "Session.h"

map<int, shared_ptr<Session>> sessions;

std::map<int, string> typesNames = {
    { INIT, "INIT"},
    { EXIT, "EXIT"},
    { CONFIRM, "CONFIRM"},
    { GETDATA, "GETDATA"},
    { TEXT, "TEXT"},
    { NODATA, "NODATA"},
    { GETALLDATA, "GETALLDATA"}
};

void ProcessClient(SOCKET hSock)
{
    CSocket s;
    s.Attach(hSock);

    Message m;
    int typeCode = m.receive(s);
    cout << m.getHeader().msgFrom << ": " << typesNames[typeCode] << endl;

    switch (typeCode)
    {
    case INIT:
    {
        auto pSession = make_shared<Session>(m.getHeader().msgFrom);
        sessions[pSession->getId()] = pSession;
        Message::send(CONFIRM);
        break;
    }
    case EXIT:
    {
        sessions.erase(m.getHeader().msgTo);
        Message::send(CONFIRM);
        return;
    }
    case GETALLDATA:
    {
        if (sessions.find(m.getHeader().msgTo) != sessions.end())
        {
            sessions[m.getHeader().msgTo]->send(s);
        }
        break;
    }
    default:
        if (sessions.find(m.getHeader().msgFrom) != sessions.end())
        {
            if (sessions.find(m.getHeader().msgTo) != sessions.end())
            {
                sessions[m.getHeader().msgTo]->Add(m);
            }
            else if (m.getHeader().msgTo == BROADCAST)
            {
                for (auto& [id, Session] : sessions)
                {
                    if (id != m.getHeader().msgFrom)
                    {
                        Session->Add(m);
                    }
                }
            }
        }
    }
}

// Стартуем сервер 
// Обслуживаем каждого клиента в отдельном потоке
void start()
{
    AfxSocketInit();

    CSocket Server;
    Server.Create(54321);

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
