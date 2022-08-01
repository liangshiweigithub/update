#include "stdafx.h"
#include "Shader.h"

Shader::Shader() :
	_programId(0),
	_filenames(SourceCOUNT),
	_sources(SourceCOUNT),
	_path(SourceCOUNT),
	_maxEmitVert(0)
{
}

Shader::Shader(const std::string& name) :
	_programId(0),
	_filenames(SourceCOUNT),
	_sources(SourceCOUNT),
	_path(SourceCOUNT),
	_maxEmitVert(0)
{
	SetName(name);
}

Shader::~Shader()
{
}

const std::string Shader::GetVertexSource() const
{
	return _sources[VertexSource];
}

const std::string Shader::GetGeometrySource() const
{
	return _sources[GeometrySource];
}

const std::string Shader::GetFragmentSource() const
{
	return _sources[FragmentSource];
}

const std::string Shader::GetCompleteSource() const
{
	return _sources[CompleteSource];
}
	
const std::string Shader::GetSource(SourceType type) const
{
	return _sources[type];
}

void Shader::SetSource(const std::string source, SourceType type)
{
	if (source.empty())
		return;

	_sources[type] = source;

	if (!_filenames[type].empty())
		_filenames[type].clear();

}

const std::string Shader::GetPath(SourceType type) const
{
	return _path[type];
}

const std::string Shader::GetFilename(SourceType type) const
{
	return _filenames[type];
}

void Shader::SetFilename(const std::string path, SourceType type)
{
	if (path.empty())
		return;

	std::string source;

	if(FileIO::LoadFromFile(path, source))
	{
		_path[type] = path;

		SetSource(source, type);
		
		std::basic_string<char>::size_type index;
		
		if ((index = path.find_last_of("/\\")) != std::string::npos)
			_filenames[type] = path.substr(index + 1);
		else
			_filenames[type] = path;
	}
}

int Shader::GetMaxEmittedVertices() const
{
	return _maxEmitVert;
}

void Shader::SetMaxEmittedVertices(unsigned int value)
{
	_maxEmitVert = value;
}

unsigned int Shader::GetProgramBindId() const
{
	return _programId;
}

void Shader::SetProgramBindId(unsigned int id)
{
	_programId = id;
}


