#pragma once
class EditorAction
{
public:
	EditorAction();
	virtual ~EditorAction();

	virtual void Undo() = 0;

	virtual void Redo() = 0;

};

