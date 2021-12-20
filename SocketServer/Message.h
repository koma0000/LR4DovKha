#pragma once
#include "pch.h"

enum Address
{
	BROKER = 0,
	ALL_USER = -1,
	STORAGE = 666,
};

enum MessageTypes {
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
	int msgFrom;
	int msgTo;
	int msgType;
	int msgSize;
};

class Message
{
private:
	MsgHeader msgHeader;
	string msgData;
	static int myID;
public:
	// Конструктор без параметров
	Message() {
		msgHeader = { 0 };
		msgData = "";
	}

	// Конструктор с парметрами
	Message(int to, int from, int type = TEXT, const string& data = "") {
		msgHeader.msgFrom = from;
		msgHeader.msgTo = to;
		msgHeader.msgType = type;
		msgHeader.msgSize = data.length();
		msgData = data;
	}

	// Геттер заголовка
	MsgHeader getHeader() {
		return msgHeader;
	}

	// Геттер текста сообщения
	string getData() {
		return msgData;
	}

	// Геттер ID
	int getMyID() {
		return myID;
	}

	// Получение сообщения
	int receive(CSocket& s) {
		if (!s.Receive(&msgHeader, sizeof(MsgHeader))) {
			return NODATA;
		}

		if (msgHeader.msgSize != 0) {
			char* pBuff = new char[msgHeader.msgSize + 1];

			s.Receive(pBuff, msgHeader.msgSize + 1);
			pBuff[msgHeader.msgSize] = '\0';
			msgData = pBuff;

			// Мы против утечек памяти
			delete[] pBuff;
		}

		return msgHeader.msgType;
	}

	// Отправка сообщения
	void send(CSocket& s) {
		s.Send(&msgHeader, sizeof(MsgHeader));
		if (msgHeader.msgSize != 0) {
			s.Send(msgData.c_str(), msgHeader.msgSize + 1);
		}
	}

	static void send(CSocket& s, int to, int from, int type = TEXT, const string& data = "") {
		Message m(to, from, type, data);
		m.send(s);
	}

	//// Функции для работы с хранилищем
	//static Message send(unsigned int to, unsigned int type = TEXT, const string& data = "") {
	//	CSocket s;
	//	s.Create();

	//	if (!s.Connect("127.0.0.1", 12345))
	//	{
	//		DWORD dwError = GetLastError();
	//		throw runtime_error("Ошибка подключения");
	//	}

	//	Message m(to, myID, type, data);
	//	m.send(s);
	//	m.receive(s);
	//	if (m.getHeader().msgType == INIT)
	//	{
	//		myID = m.getHeader().msgTo;
	//	}
	//	return m;
	//}

	// Функция для отправки служебной информации в хранилище
	static Message SendToStorage(unsigned int to, unsigned int from, unsigned int type, const string& data = "")
	{
		CSocket s;
		s.Create();
		if (!s.Connect("127.0.0.1", 66666))
		{
			DWORD dwError = GetLastError();
			throw runtime_error("Ошибка подключения");
		}
		Message m(to, from, type, data);
		m.send(s);
		m.receive(s);

		s.Close();
		return m;
	}
};

int Message::myID = 0;