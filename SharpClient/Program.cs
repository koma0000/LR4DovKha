using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace Seleznev_sharp_client
{
    enum Address
    {
        BROKER = 0,
        ALL_USER = -1,
        STORAGE = 666,
    };

    enum MessageTypes
    {
        INIT,
        EXIT,
        CONFIRM,
        GETDATA,
        TEXT,
        NODATA,
        GETALLDATA,
    };

    struct MsgHeader
    {
        public int msgFrom;
        public int msgTo;
        public int msgType;
        public int msgSize;
    };

    class Program
    {
        private static int myID;

        static void SendMessage(Socket s, int To, int From, MessageTypes Type = MessageTypes.TEXT, string Data = "")
        {
            Message m = new Message(To, From, Type, Data);
            m.Send(s);
        }

        static void connect(Socket s, IPEndPoint endPoint)
        {
            s.Connect(endPoint);
        }

        static void disconnect(Socket s)
        {
            s.Shutdown(SocketShutdown.Both);
            s.Close();
        }

        public static void GetData()
        {
            while (true)
            {
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Message m = new Message();

                connect(s, endPoint);
                SendMessage(s, 0, myID, MessageTypes.GETDATA);
                if (m.Receive(s) == MessageTypes.TEXT)
                {
                    Console.WriteLine("Сообщение от клиента: " + m.getMsgHeader().msgFrom + ": " + m.getMsgData());
                }
                disconnect(s);
                Thread.Sleep(1000);
            }
        }


        public static void Main(string[] args)
        {
            // Спим секунду, иначе одинаковые ID будут при одновременном запуске
            Thread.Sleep(1000);

            Message m = new Message();
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            //Подключение к серверу
            connect(s, endPoint);
            if (!s.Connected)
                return;

            SendMessage(s, (int)Address.BROKER, 0, MessageTypes.INIT);
            if (m.Receive(s) == MessageTypes.CONFIRM)
            {
                myID = m.getMsgHeader().msgTo;
                Console.WriteLine("Клиент подключен");
                Console.WriteLine("ID:" + myID);

                Thread t = new Thread(GetData);
                t.Start();
            }
            disconnect(s);

            //Отправке сообщений
            while (true)
            {
                // IPEndPointКласс содержит сведения об узле и локальном или удаленном порте, необходимые приложению для подключения к службе на узле.
                // Объединяя IP-адрес узла и номер порта службы, IPEndPoint класс формирует точку подключения к службе.

                endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 12345);
                s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Console.WriteLine("[1] Отправить сообщение\n[2] Получить все сообщения\n[3] Выйти\n");
                int choice = Convert.ToInt32(Console.ReadLine());
                switch (choice)
                {
                    case 1:
                        {
                            int ClientID = (int)Address.ALL_USER;
                            Console.WriteLine("[1] Только для одного клиента\n[2] Для всех клиентов\n");
                            int choice2 = Convert.ToInt32(Console.ReadLine());
                            if (choice2 == 1)
                            {
                                Console.WriteLine("\nВведите ID клиента\n");
                                ClientID = Convert.ToInt32(Console.ReadLine());
                            }

                            Console.WriteLine("\nВведите сообщение\n");
                            string str = Console.ReadLine().ToString();
                            connect(s, endPoint);
                            SendMessage(s, ClientID, myID, MessageTypes.TEXT, str);
                            if (m.Receive(s) == MessageTypes.CONFIRM)
                            {
                                Console.WriteLine("\nУспешно\n");
                            }
                            else
                            {
                                Console.WriteLine("\nОшибка\n");
                            }
                            disconnect(s);
                            break;

                        }
                    case 2:
                        {
                            connect(s, endPoint);

                            SendMessage(s, myID, myID, MessageTypes.GETALLDATA);

                            if (m.Receive(s) == MessageTypes.NODATA)
                                Console.WriteLine("\nАрхив пуст\n");
                            else
                            {
                                int messagesLen = Int32.Parse(m.getMsgData());

                                Console.WriteLine("\nИстория сообщений:\n");

                                for (int i = 0; i < messagesLen; i++)
                                {
                                    m.Receive(s);


                                    if ((m.getMsgHeader().msgTo) / 256 == myID)
                                    {
                                        Console.WriteLine("Сообщение от клиента " + ((m.getMsgHeader().msgFrom)) / 256 + " : " + m.getMsgData());
                                    }
                                    else
                                    {
                                        Console.WriteLine("Мой ответ клиенту " + (m.getMsgHeader().msgTo) + " : " + m.getMsgData());
                                    }

                                }
                            }

                            disconnect(s);
                            break;
                        }
                    case 3:
                        {
                            connect(s, endPoint);
                            SendMessage(s, (int)Address.BROKER, myID, MessageTypes.EXIT);
                            if (m.Receive(s) == MessageTypes.CONFIRM)
                            {
                                Console.WriteLine("\nУспешно\n");
                            }
                            else
                            {
                                Console.WriteLine("\nОшибка\n");
                            }

                            disconnect(s);
                            Environment.Exit(0);
                            break;
                        }

                    default:
                        break;
                }
            }
        }
    }
}

