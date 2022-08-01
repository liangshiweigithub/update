#pragma once

#include "SceneGraphExports.h"

#include "View.h"
#include "Perspective.h"

class SCENEGRAPH_API Camera
{
public:
	Camera();
	~Camera();

	const View& GetView() const;
	View& GetView();
	void SetView(const View& view);

	const Perspective& GetPerspective() const;
	void SetPerspective(const Perspective& perspective);

private:
	View _view;
	Perspective _perspective;

};