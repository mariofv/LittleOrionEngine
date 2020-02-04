#pragma once
class EditorAction
{
public:
	EditorAction() = default;
	virtual ~EditorAction() = default;

	virtual void Undo() = 0;

	virtual void Redo() = 0;

};

