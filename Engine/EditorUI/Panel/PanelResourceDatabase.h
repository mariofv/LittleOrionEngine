#ifndef _PANELRESOURCEDATABASE_H_
#define _PANELRESOURCEDATABASE_H_

#include "Panel.h"
#include "ResourceManagement/Resources/Resource.h"

class PanelResourceDatabase : public Panel
{
public:
	PanelResourceDatabase();
	~PanelResourceDatabase() = default;

	void Render() override;

private:
	std::string GetResourceTypeName(ResourceType resource_type) const;

};

#endif //_PANELRESOURCEDATABASE_H_

