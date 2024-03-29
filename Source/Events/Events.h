#ifndef EVENTS_H
#define EVENTS_H
#include "../Header.h"
#include "../Connections/Connections.h"
#include <typeinfo>
#include "Reading/Reading.h"

namespace Events
{
	const char END_CHARACTER = '§';
	const char SEPARATOR_CHARACTER = 'µ';
	const unsigned int ARRAY_SIZE = 1024;
	const unsigned int BUFFER_OVERSIZE = 30000;

	void Listener(int socketfd);
	void Unserialize(char* buffer);
	void Register(const char* _name, void (*function)(char** _args));


	#pragma region Triggers
	class EventCl
	{
	public:
		char* buffer = nullptr;
		unsigned int bufferSize = 0;

		template<typename Arg>
		void SerializeArg(Arg _arg)
		{
			std::stringstream valueString;
			valueString /*<< typeid(_arg).name()*/ /*<< '_'*/ << _arg;
			std::string argStr = valueString.str();


			int argLength = argStr.length();

			int oldBufferSize = bufferSize - 1;
			bufferSize += (argLength + 1); // +1 for the null-terminator

			char* newBuffer = new char[bufferSize + 1];

			if (buffer != nullptr)
			{
				std::memcpy(newBuffer, buffer, oldBufferSize);
				delete[] buffer;
			}

			buffer = newBuffer;

			for (int i = 0; i < argLength; i++)
			{
				buffer[oldBufferSize + i] = argStr[i];
			}
			buffer[bufferSize - 2] = SEPARATOR_CHARACTER;
			buffer[bufferSize - 1] = '\0';
		}
	};
	
	template<class...Arg>
	void TriggerClient(int _target, const char* _name, Arg ..._args)
	{
		EventCl evtcl;
		int nameLength = strlen(_name);

		char* targetString = new char[nameLength + 1];
		strcpy(targetString, _name);

		evtcl.bufferSize = strlen(targetString) + 2;
		evtcl.buffer = new char[evtcl.bufferSize];
		strcpy(evtcl.buffer, targetString);
		evtcl.buffer[evtcl.bufferSize - 2] = SEPARATOR_CHARACTER;

		((evtcl.SerializeArg(_args)), ...);
		evtcl.buffer[evtcl.bufferSize - 2] = END_CHARACTER;

		delete[] targetString;
		Connections::SendData(_target, evtcl.buffer);
		return;
	}
	#pragma endregion
}

#endif // !EVENTS_H
