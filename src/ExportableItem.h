#pragma once

class ExportableItem
{
public:
	ExportableItem();
	virtual ~ExportableItem() = 0;
	virtual void sample() {}
private:
	DISALLOW_COPY_AND_ASSIGN(ExportableItem);
};
