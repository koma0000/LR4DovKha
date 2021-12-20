// Melnicov_Client_C++.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
            cout << "Сообщение от " << m.getHeader().msgFrom << ": " << m.getData() << endl;
        }
        disconnect(s);
        Sleep(1000);
    }
}

void Client() {
    // Инициализация сокетов и подключения
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
        cout << "Ошибка" << endl;
        return;
    }
    disconnect(socket);

    // Передача и прием сообщений
    while (true) {
        cout << "1)Отправить сообщение\n2) Получить все сообщения\n3) Выйти\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int ClientID = ALL_USER;
            cout << "1) Только для одного клиента\n2) Для всех клиентов\n" << endl;
            int choice2;
            cin >> choice2;

            if (choice2 == 1) {
                cout << "\nВведите ID клиента\n";
                cin >> ClientID;
            }


            cout << "\nВведите сообщение\n";
            string str;
            cin.ignore();
            getline(cin, str);
            connect(socket);
            Message::send(socket, ClientID, MyID, TEXT, str);
            if (m.receive(socket) == CONFIRM) {
                cout << "\nУспешно\n" << endl;
            }
            else {
                cout << "\nОшибка\n" << endl;
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
                throw runtime_error("Ошибка подключения");
            }

            m.send(s, MyID, MyID, GETALLDATA);

            m.receive(s);

            if (m.getHeader().msgType == NODATA) {
                cout << "\nАхрив пуст\n" << endl;
            }
            else
            {
                cout << "\n\nИстория сообщений\n" << endl;
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
                        cout << "Сообщение от клиента " << hist_idfrom << " : " << history_message << endl;
                    }
                    else {
                        cout << "Мой ответ клиенту " << hist_idto << " : " << history_message << endl;
                    }
                }

                /*for (int i = 0; i < length; i++) {

                    m.receive(s);

                    if (m.getHeader().msgTo == MyID) {
                        cout << "Сообщение от клиента " << m.getHeader().msgFrom << " : " << m.getData() << endl;
                    }
                    else {
                        cout << "Мой ответ клиенту " << m.getHeader().msgTo << " : " << m.getData() << endl;
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
                cout << "\nУспешно\n" << endl;
            }
            else {
                cout << "\nОшибка\n" << endl;
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
            std::cout << "Клиент подключен\n" << endl;
            Client();
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
