#include "InputFocusManager.h"
#include "widget/U_edit.h"
#include <stdio.h>
#include <stdarg.h>

static void LogInputFocus(const char* fmt, ...)
{
	(void)fmt;
}

InputFocusManager::InputFocusManager()
	: m_focusedEditor(NULL)
{
}

InputFocusManager& InputFocusManager::GetInstance()
{
	static InputFocusManager instance;
	return instance;
}

// Global accessor for DXLibBackendSDL (cannot include headers)
InputFocusManager& g_GetInputFocusManager()
{
	return InputFocusManager::GetInstance();
}

void InputFocusManager::SetFocusedEditor(LineEditorVisual* editor)
{
	m_focusedEditor = editor;

	// Debug logging
	if (editor) {
		printf("InputFocusManager: Set focused editor to %p\n", (void*)editor);
		LogInputFocus("SetFocusedEditor editor=%p", (void*)editor);
	} else {
		printf("InputFocusManager: Cleared focused editor\n");
		LogInputFocus("SetFocusedEditor editor=null");
	}
}

LineEditorVisual* InputFocusManager::GetFocusedEditor() const
{
	return m_focusedEditor;
}

void InputFocusManager::HandleTextInput(const char* text)
{
	if (!m_focusedEditor) {
		// No editor has focus, ignore input
		LogInputFocus("HandleTextInput ignored text='%s' no_focus", text ? text : "(null)");
		return;
	}

	// Route text input to focused editor
	LogInputFocus("HandleTextInput text='%s' editor=%p", text ? text : "(null)", (void*)m_focusedEditor);
	m_focusedEditor->m_Editor.HandleTextInput(text);
}

void InputFocusManager::HandleTextEditing(const char* text, int start, int length)
{
	if (!m_focusedEditor) {
		// No editor has focus, ignore editing events
		LogInputFocus("HandleTextEditing ignored start=%d length=%d no_focus", start, length);
		return;
	}

	// Route text editing to focused editor
	LogInputFocus("HandleTextEditing text='%s' start=%d length=%d editor=%p",
		text ? text : "(null)", start, length, (void*)m_focusedEditor);
	m_focusedEditor->m_Editor.HandleTextEditing(text, start, length);
}

void InputFocusManager::HandleKeyDown(unsigned int vk_code)
{
	if (!m_focusedEditor) {
		// No editor has focus, ignore key events
		LogInputFocus("HandleKeyDown ignored vk=%u no_focus", vk_code);
		return;
	}

	// Route key down to focused editor
	// WM_KEYDOWN = 0x0100 (Windows message value)
	LogInputFocus("HandleKeyDown vk=%u editor=%p", vk_code, (void*)m_focusedEditor);
	m_focusedEditor->m_Editor.KeyboardControl(0x0100, vk_code, 0);
}
