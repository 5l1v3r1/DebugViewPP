// DebugViewCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace fusion;
using namespace fusion::debugviewpp;

void Method1()
{
	std::vector<std::unique_ptr<LogSource>> sources;
	
	sources.push_back(make_unique<DBWinReader>(false));
	if (HasGlobalDBWinReaderRights())
	{
		sources.push_back(make_unique<DBWinReader>(true));
	}

	auto pred = [](const Line& a, const Line& b) { return a.time < b.time; };
	while (true)
	{
		Lines lines;
		for (auto it = sources.begin(); it != sources.end(); )
		{
			Lines pipeLines((*it)->GetLines());
			Lines lines2;
			lines2.reserve(lines.size() + pipeLines.size());
			std::merge(lines.begin(), lines.end(), pipeLines.begin(), pipeLines.end(), std::back_inserter(lines2), pred);
			lines.swap(lines2);

			if ((*it)->AtEnd())
				it = sources.erase(it);
			else
				++it;
		}

		for (auto i=lines.begin(); i != lines.end(); ++i)
		{
			std::cout << i->pid << "\t" << i->processName.c_str() << "\t" << i->message.c_str() << "\n";
		}
		Sleep(100);
	}
}

void OnMessage(double time, FILETIME systemTime, DWORD processId, HANDLE processHandle, const char* message)
{
	static boost::mutex mutex;
	boost::mutex::scoped_lock lock(mutex);
	std::cout << time << "\t" << processId << "\t" << message << "\n";
}

void Method2()
{
	boost::signals2::connection connection1;
	boost::signals2::connection connection2;

	LogSources sources(false);

	auto dbwinlistener = make_unique<DBWinReader>(false, false);
	connection1 = dbwinlistener->Connect(&OnMessage);
	sources.Add(std::move(dbwinlistener));

	if (HasGlobalDBWinReaderRights())
	{
		auto globalDBbwinlistener = make_unique<DBWinReader>(true, false);
		connection2 = globalDBbwinlistener->Connect(&OnMessage);
		sources.Add(std::move(globalDBbwinlistener));
	}
	sources.Listen();
}

int _tmain(int argc, _TCHAR* argv[])
{
	try {
		Method1();
	}
	catch (std::exception& e)
	{
		MessageBoxA(0, e.what(), "DebugViewCmd caught an exception!", MB_OK);
	}
	return 0;
}
