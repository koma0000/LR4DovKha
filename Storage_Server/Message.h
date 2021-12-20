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
public:
	// ����������� ��� ����������
	Message() {
		msgHeader = { 0 };
		msgData = "";
	}

	// ����������� � ����������
	Message(int to, int from, int type = TEXT, const string& data = "") {
		msgHeader.msgFrom = from;
		msgHeader.msgTo = to;
		msgHeader.msgType = type;
		msgHeader.msgSize = data.length();
		msgData = data;
	}

	// ������ ���������
	MsgHeader getHeader() {
		return msgHeader;
	}

	// ������ ������ ���������
	string getData() {
		return msgData;
	}

	// ��������� ���������
	int receive(CSocket& s) {
		if (!s.Receive(&msgHeader, sizeof(MsgHeader))) {
			return NODATA;
		}

		if (msgHeader.msgSize != 0) {
			char* pBuff = new char[msgHeader.msgSize + 1];

			s.Receive(pBuff, msgHeader.msgSize + 1);
			pBuff[msgHeader.msgSize] = '\0';
			msgData = pBuff;
			delete[] pBuff;
		}

		return msgHeader.msgType;
	}

	// �������� ���������
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

	// ������� ��� �������� ������� �� ���������
	static Message send(unsigned int type = TEXT, const string& data = "") {
		CSocket s;
		s.Create();

		if (!s.Connect(_T("127.0.0.1"), 12345))
		{
			DWORD dwError = GetLastError();
			throw runtime_error("������ �����������");
		}

		Message m(BROKER, STORAGE, type, data);
		m.send(s);

		return m;
	}
};

