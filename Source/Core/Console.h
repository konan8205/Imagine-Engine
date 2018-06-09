
#include "Core/CoreHeader.h"

class Console
{
public:
	Console();
	~Console();
	Console(const Console& copy);
	Console(const Console&& copy);
	Console& operator=(const Console& copy);
	Console& operator=(const Console&& copy);

public:
	void Error(const char* _Message, ...);
	void Error(const string& _Message, ...);

private:
	/*
	inline char* ReplaceChar(const char* ch, const char* findCh, const char* replaceCh) {
	}

	inline string& ReplaceString(const string& str, const string& findStr, const string& replaceStr,
		const uint32_t pos)
	{
		string strTemp(str);
		return strTemp.replace(str.find(findStr, pos), findStr.length(), replaceStr);
	}
	*/
};