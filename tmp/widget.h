#include "point.h"

class IWidget{
public:
	virtual Point GetPos() = default;
	virtual Resolution GetResolution() = default;
	virtual void SetOnClicked() = default;
	virtual void DoOnClicked() = default;
};
