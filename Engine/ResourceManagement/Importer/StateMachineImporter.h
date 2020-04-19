#ifndef _STATEMACHINEIMPORTER_H_
#define _STATEMACHINEIMPORTER_H_

#include "Importer.h"

class StateMachineImporter : public Importer
{
public:
	StateMachineImporter() : Importer(ResourceType::STATE_MACHINE) {};
	~StateMachineImporter() = default;
	FileData ExtractData(Path& assets_file_path, const Metafile& metafile) const override;
};

#endif // !_STATEMACHINEIMPORTER_H_


