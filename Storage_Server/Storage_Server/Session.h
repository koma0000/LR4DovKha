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
			Message::send(s, session_id, MIDDLEWARE, NODATA);
		}
		else
		{
			Message::send(s, session_id, STORAGE, TEXT, to_string((int)messages.size()));
			for (Message m : messages)
				Message::send(s, m.getHeader().msgTo, m.getHeader().msgFrom, TEXT, m.getData());
		}

		LeaveCriticalSection(&criricalSection);
	}
};

