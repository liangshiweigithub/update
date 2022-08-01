#include "stdafx.h"

#include <fstream>

#include "FileIO.h"
#include "VectorMathTypes.h"

bool FileIO::LoadFromFile(const std::string& filename, std::string& dest)
{
	bool success = false;

	FILE* fp;
	if(!fopen_s(&fp, filename.c_str(), "rt"))
	{
		fseek(fp, 0, SEEK_END);
		unsigned int filelength = ftell(fp);
		rewind(fp);

		if (filelength > 0)
		{
			success = true;

			char* buffer = new char[filelength + 1];
			filelength = static_cast<unsigned int>(fread(buffer, sizeof(char), filelength, fp));
			buffer[filelength] = '\0';

			dest = buffer;
			delete[] buffer;
		}
		fclose(fp);
	}
	return success;
}

unsigned int FileIO::GetNbrOfLines(const std::string& str)
{
	if (str.empty())
		return 0;

	unsigned int lineCount = 1;
	std::string::const_iterator it = str.begin();

	while (it != str.end())
    {
    	if (*it == '\n')
    		lineCount++;
    	*it++;
    }
    
	return lineCount;
}

const std::string FileIO::GetLine(const std::string& str, unsigned int nbr)
{
	if (str.empty())
		return std::string();

	unsigned int lineCount = 1;
	std::string::const_iterator it = str.begin();
	
	unsigned int i;
	for (i = 0; i < str.length() && lineCount < nbr; ++i)
	{
		if (str[i] == '\n')
			lineCount++;
	}
	
	unsigned int last = str.find('\n', i);
	
	return str.substr(i, last - i);
}

bool FileIO::Write(const std::string& filename, const std::string& input)
{
	std::ofstream file;

	file.open(filename.c_str(), std::ios::out | std::ios::trunc);

	if (!file.is_open())
		return false;

	file << input.c_str();

	file.close();

	return true;
}

bool FileIO::Load(const std::string& filename, std::string& output)
{
	std::ifstream file;

	file.open(filename.c_str(), std::ifstream::in);

	if (!file.is_open())
		return false;

	while (!file.eof())
	{
		char line[256];
		file.getline(line, 256);

		if (strlen(line) > 0)
			output += line;
	}

	file.close();

	return true;
}


bool FileIO::LoadValue(const std::string& filename, const std::string& id, int& value)
{
	std::ifstream file;

	file.open(filename.c_str(), std::ifstream::in);

	if (!file.is_open())
		return false;

	std::string startDelim = std::string("<") + id + std::string(">");
	std::string endDelim = std::string("</") + id + std::string(">");

	unsigned int startlen = startDelim.length();
	
	bool found = false;

	while (!file.eof() && !found)
	{
		char line[2000];
		file.getline(line, 2000);

		std::string current(line);

		if (strlen(line) > 0 && current[0] != '#')
		{
			std::string::size_type first = current.find(startDelim);
			std::string::size_type last = current.find(endDelim);

			if (first == std::string::npos || last == std::string::npos || first > last)
				continue;

			std::string strVal(current.substr(first + startlen, last - (first + startlen)));

			found |= (sscanf_s(strVal.c_str(), "%d", &value) != 0);

		}
	}

	file.close();

	return found;
}
