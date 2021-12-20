#pragma once
#include "pch.h"
#include "Message.h"

class Session
{
private:
	// Идентификатор сессии (задается сервером)
	int session_id;
	// !!! Тут список, а не очередь !!!
	// Список  сообщений
	vector<Message> messages;
	// Критическая секция для защиты очереди от одновременного использования
	CRITICAL_SECTION criricalSection;
public:
	// Конструктор по умолчанию
	Session() {
		InitializeCriticalSection(&criricalSection);
	}

	// Конструктор с параметрами
	Session(int ID)
		:session_id(ID) {
		InitializeCriticalSection(&criricalSection);
	}

	// Деструктор следящий за уничтожением критической секции 
	~Session() {
		DeleteCriticalSection(&criricalSection);
	}

	// Геттер идентификатора сессии
	int getId() {
		return session_id;
	}

	// Сеттер идентификатора сессии
	void setId(int i) {
		session_id = i;
	}

	// Добавление сообщения в хранилище
	void Add(Message& m) {
		EnterCriticalSection(&criricalSection);

		messages.push_back(m);

		LeaveCriticalSection(&criricalSection);
	}

	// Отправка списка сообщений
	void send(CSocket& s) {
		EnterCriticalSection(&criricalSection);

		if (messages.empty())
		{
			Message::send(s, session_id, BROKER, NODATA);
		}
		else
		{
			Message::send(s, session_id, STORAGE, TEXT, to_string((int)messages.size()));
			for (Message m : messages)
				Message::send(s, m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, m.getData());
		}

		LeaveCriticalSection(&criricalSection);
	}

	void senthist(CSocket& s)
	{
		if (messages.empty())
		{
			Message::send(s, session_id, STORAGE, NODATA);
		}
		else
		{

			string data = "";
			int pref_id = 0, pref_to = 0;

			for (Message m : messages) {
				if (!((m.getHeader().msgFrom == pref_id) && (m.getHeader().msgTo != pref_to))) {
					data.append(to_string(m.getHeader().msgTo));
					data.append("#");
					data.append(to_string(m.getHeader().msgFrom));
					data.append("#");
					data.append(m.getData());
					data.append("#");

					pref_id = m.getHeader().msgFrom;
					pref_to = m.getHeader().msgTo;
				}
			}

			Message m = messages[0];

			Message::send(s, m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, data);
		}
	}
};



