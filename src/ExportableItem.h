#pragma once
#include "Animation.h"

class ExportableItem
{
public:
	ExportableItem();
	virtual ~ExportableItem() = 0;
	virtual void sample(Animation*) {}
private:
	DISALLOW_COPY_AND_ASSIGN(ExportableItem);
};
