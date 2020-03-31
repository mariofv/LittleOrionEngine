#ifndef _STATEMACHINEIMPORTER_H_
#define _STATEMACHINEIMPORTER_H_

#include "Importer.h"

class StateMachineImporter : public Importer
{
public:
	StateMachineImporter() = default;
	~StateMachineImporter() = default;
	ImportResult Import(const File & file, bool force = true) const override;
};

#endif // !_STATEMACHINEIMPORTER_H_


