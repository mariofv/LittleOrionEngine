#ifndef _EDITORACTION_H_
#define _EDITORACTION_H_
class EditorAction
{
public:
	EditorAction() = default;
	virtual ~EditorAction() = default;

	virtual void Undo() = 0;

	virtual void Redo() = 0;

};

#endif _EDITORACTION_H