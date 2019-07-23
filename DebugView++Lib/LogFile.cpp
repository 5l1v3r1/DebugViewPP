// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#include "stdafx.h"
#include <vector>
#include "CobaltFusion/Str.h"
#include "Win32/Utilities.h"
#include "DebugView++Lib/LogFile.h"

namespace fusion {
namespace debugviewpp {

Message::Message(double time, FILETIME systemTime, DWORD pid, const std::string& processName, const std::string& msg, COLORREF color) :
	time(time),
	systemTime(systemTime),
	processId(pid),
	processName(processName),
	text(msg),
	color(color)
{
}

LogFile::LogFile() :
	m_historySize(0)
{
}

bool LogFile::Empty() const
{
	return m_messages.empty();
}

void LogFile::Clear()
{
	m_messages.clear();
	m_messages.shrink_to_fit();
	m_storage.Clear();
	m_storage.shrink_to_fit();
	m_processInfo.Clear();
}

void LogFile::Add(const Message& msg)
{
	auto props = m_processInfo.GetProcessProperties(msg.processId, WStr(msg.processName).str());
	m_messages.emplace_back(InternalMessage(msg.time, msg.systemTime, props.uid));
	m_storage.Add(msg.text);
}

size_t LogFile::BeginIndex() const
{
	return 0;
}

size_t LogFile::EndIndex() const
{
	return m_messages.size();
}

size_t LogFile::Count() const
{
	return m_messages.size();
}

Message LogFile::operator[](size_t i) const
{
	auto& msg = m_messages[i];
	auto props = m_processInfo.GetProcessProperties(msg.uid);
	return Message(msg.time, msg.systemTime, props.pid, Str(props.name).str(), m_storage[i], props.color);
}

size_t LogFile::GetHistorySize() const
{
	return m_historySize;
}

void LogFile::SetHistorySize(size_t size)
{
	m_historySize = size;
}

void LogFile::Copy(size_t beginIndex, size_t endIndex, LogFile& logfile)
{
	for (size_t i = beginIndex; i <= endIndex; ++i)
		logfile.Add((*this)[i]);
}

void LogFile::Swap(LogFile& logfile)
{
	std::swap(m_messages, logfile.m_messages);
	std::swap(m_processInfo, logfile.m_processInfo);
	std::swap(m_storage, logfile.m_storage);
	std::swap(m_historySize, logfile.m_historySize);
}

} // namespace debugviewpp 
} // namespace fusion
