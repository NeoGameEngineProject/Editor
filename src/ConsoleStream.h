#ifndef NEOEDITOR_CONSOLESTREAM_H
#define NEOEDITOR_CONSOLESTREAM_H

#include <streambuf>
#include <ostream>

#include <QScrollBar>
#include <QPlainTextEdit>
#include <iostream>

class ConsoleBuffer : public std::basic_streambuf<char, std::char_traits<char>>
{
	typedef std::char_traits<char> Traits;

	static const size_t BUF_SIZE = 512;
	char m_buffer[BUF_SIZE];
	QPlainTextEdit* m_output = nullptr;

public:
	ConsoleBuffer()
	{
		setp(m_buffer, m_buffer + BUF_SIZE);
	}

	void setOutput(QPlainTextEdit* data)
	{
		m_output = data;
	}

protected:
	int overflow(int c = Traits::eof()) override
	{
		putChars(pbase(), pptr());
		if (c != Traits::eof())
		{
			char c2 = c;
			putChars(&c2, &c2 + 1);
		}

		setp(m_buffer, m_buffer + BUF_SIZE);
		return c;
	}

	int sync() override
	{
		putChars(pbase(), pptr());
		setp(m_buffer, m_buffer + BUF_SIZE);
		return 0;
	}

	void putChars(const char* begin, const char* end)
	{
		QString data;
		for(const char* c = begin; c < end - 1; c++)
		{
			data += *c;
		}
		m_output->appendPlainText(data);
		m_output->verticalScrollBar()->setValue(m_output->verticalScrollBar()->maximum());

		std::cout << data.toStdString() << std::endl;
	}
};

class ConsoleStream : public std::basic_ostream<char, std::char_traits<char>>
{
public:
	ConsoleStream():
		std::basic_ostream< char, std::char_traits<char>>(&m_buffer)
	{}

	void setOutput(QPlainTextEdit* data)
	{
		m_buffer.setOutput(data);
	}

private:
	ConsoleBuffer m_buffer;
};


#endif //NEOEDITOR_CONSOLESTREAM_H
