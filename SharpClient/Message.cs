using System;
using System.Text;
using System.Net.Sockets;

namespace Seleznev_sharp_client
{
    class Message
    {
        private MsgHeader msgHeader;
        private string msgData;

        public static int GetSize()
        {
            return sizeof(int) * 4;
        }

        public MsgHeader getMsgHeader()
        {
            return msgHeader;
        }

        public void setMsgData(string msgData)
        {
            this.msgData = msgData;
        }

        public string getMsgData()
        {
            return msgData;
        }

        public Message()
        {
            msgHeader.msgTo = 0;
            msgHeader.msgFrom = 0;
            msgHeader.msgSize = 0;
            msgHeader.msgType = 0;
        }

        public Message(int to, int from, MessageTypes type = MessageTypes.TEXT, string data = "")
        {
            msgHeader.msgTo = to;
            msgHeader.msgFrom = from;
            msgHeader.msgType = (int)type;
            msgHeader.msgSize = data.Length;
            msgData = data;
        }

        public void Send(Socket s)
        {
            // Байт определяется как 8-битовое целое число без знака.
            // BitConverterКласс содержит статические методы для преобразования всех типов-примитивов в массив байтов и из него.

            s.Send(BitConverter.GetBytes(msgHeader.msgFrom), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(msgHeader.msgTo), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes((int)msgHeader.msgType), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(msgHeader.msgSize), sizeof(int), SocketFlags.None);

            if (msgHeader.msgSize != 0)
            {
                s.Send(Encoding.UTF8.GetBytes(msgData), msgHeader.msgSize, SocketFlags.None);
            }

        }

        public MessageTypes Receive(Socket s)
        {
            byte[] buffer = new byte[sizeof(int)];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgFrom = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[sizeof(int)];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgTo = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[sizeof(int)];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgType = (int)(MessageTypes)BitConverter.ToInt32(buffer, 0);

            buffer = new byte[sizeof(int)];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgSize = BitConverter.ToInt32(buffer, 0);

            if (msgHeader.msgSize != 0)
            {
                buffer = new byte[msgHeader.msgSize];
                s.Receive(buffer, msgHeader.msgSize, SocketFlags.None);
                msgData = Encoding.ASCII.GetString(buffer, 0, msgHeader.msgSize);
            }

            return (MessageTypes)(int)msgHeader.msgType;
        }
    }
}
