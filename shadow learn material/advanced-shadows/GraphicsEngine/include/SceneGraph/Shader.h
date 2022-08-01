#pragma once

#include <string>
#include <vector>

#include "SceneGraphExports.h"
#include "Common/Object.h"

class Material;
class Transform;

class SCENEGRAPH_API Shader : public Object
{
public:
	enum SourceType
	{
		VertexSource = 0,
		GeometrySource,
		FragmentSource,
		CompleteSource,
		SourceCOUNT
	};

	Shader();
	Shader(const std::string& name);
	virtual ~Shader();

	virtual const std::string GetVertexSource() const;
	virtual const std::string GetFragmentSource() const;
	virtual const std::string GetGeometrySource() const;
	virtual const std::string GetCompleteSource() const;
	
	const std::string GetSource(SourceType type) const;
	void SetSource(const std::string source, SourceType type);

	const std::string GetPath(SourceType type) const;
	const std::string GetFilename(SourceType type) const;
	void SetFilename(const std::string path, SourceType type);

	int GetMaxEmittedVertices() const;
	void SetMaxEmittedVertices(unsigned int value); 

	unsigned int GetProgramBindId() const;
	void SetProgramBindId(unsigned int id);

private:
	typedef std::vector<std::string> tFilename;
	typedef std::vector<std::string> tSource;
	
private:
	unsigned int _programId;
	int _maxEmitVert;

#pragma warning(push)
#pragma warning(disable : 4251)
	tFilename _path;
	tFilename _filenames;
	tSource _sources;
#pragma warning(pop)
};