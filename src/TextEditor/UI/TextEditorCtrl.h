#pragma once

#include <utility>
#include "Archive/ArchiveEntry.h"
#include "TextEditor/Lexer.h"
#include "TextEditor/TextLanguage.h"
#include "TextEditor/TextStyle.h"

class wxButton;
class wxCheckBox;
class wxTextCtrl;
class FindReplacePanel;
class SCallTip;
class wxChoice;

wxDECLARE_EVENT(wxEVT_COMMAND_JTCALCULATOR_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_TEXT_CHANGED, wxCommandEvent);

class JumpToCalculator : public wxThread
{
public:
	JumpToCalculator(wxEvtHandler* handler, const string& text, vector<string> block_names, vector<string> ignore) :
		handler_(handler),
		text_(text),
		block_names_(std::move(block_names)),
		ignore_(std::move(ignore))
	{
	}
	virtual ~JumpToCalculator() = default;

	ExitCode Entry() override;

private:
	wxEvtHandler*  handler_;
	string         text_;
	vector<string> block_names_;
	vector<string> ignore_;
};

class TextEditorCtrl : public wxStyledTextCtrl
{
public:
	TextEditorCtrl(wxWindow* parent, int id);
	~TextEditorCtrl();

	TextLanguage* language() const { return language_; }
	long          lastModified() const { return last_modified_; }

	bool setLanguage(TextLanguage* lang);

	void setup();
	void setupFoldMargin(TextStyle* margin_style = nullptr);
	bool applyStyleSet(StyleSet* style);
	bool loadEntry(ArchiveEntry* entry);
	void getRawText(MemChunk& mc) const;

	// Misc
	void trimWhitespace();

	// Find/Replace
	void setFindReplacePanel(FindReplacePanel* panel) { panel_fr_ = panel; }
	void showFindReplacePanel(bool show = true);
	bool findNext(const string& find, int flags);
	bool findPrev(const string& find, int flags);
	bool replaceCurrent(const string& find, const string& replace, int flags);
	int  replaceAll(const string& find, const string& replace, int flags);

	// Hilight/matching
	void checkBraceMatch();
	void matchWord();
	void clearWordMatch();

	// Calltips
	void showCalltip(int position);
	void hideCalltip();
	bool openCalltip(int pos, int arg = 0, bool dwell = false);
	void updateCalltip();

	// Jump To
	void setJumpToControl(wxChoice* jump_to);
	void updateJumpToList();
	void jumpToLine();

	// Folding
	void foldAll(bool fold = true);
	void setupFolding();

	// Comments
	void lineComment();
	void blockComment();
	void cycleComments() const;

private:
	TextLanguage*          language_ = nullptr;
	FindReplacePanel*      panel_fr_ = nullptr;
	SCallTip*              call_tip_ = nullptr;
	wxChoice*              choice_jump_to_ = nullptr;
	JumpToCalculator*      jump_to_calculator_ = nullptr;
	std::unique_ptr<Lexer> lexer_;
	string                 prev_word_match_;
	string                 autocomp_list_;
	vector<int>            jump_to_lines_;
	long                   last_modified_ = 0;

	// State tracking for updates
	int  prev_cursor_pos_      = -1;
	int  prev_text_length_     = -1;
	int  prev_brace_match_     = -1;
	bool block_comment_closed_ = false;

	// Timed update stuff
	wxTimer timer_update_;
	bool    update_jump_to_ = false;
	bool    update_word_match_ = false;

	// Calltip stuff
	TLFunction* ct_function_ = nullptr;
	int         ct_argset_ = 0;
	int         ct_start_ = 0;
	bool        ct_dwell_ = false;

	// Default comment strings
	const string default_line_comment_  = "//";
	const string default_begin_comment_ = "/*";
	const string default_end_comment_   = "*/";

	// Events
	void onKeyDown(wxKeyEvent& e);
	void onKeyUp(wxKeyEvent& e);
	void onCharAdded(wxStyledTextEvent& e);
	void onUpdateUI(wxStyledTextEvent& e);
	void onCalltipClicked(wxStyledTextEvent& e);
	void onMouseDwellStart(wxStyledTextEvent& e);
	void onMouseDwellEnd(wxStyledTextEvent& e);
	void onMouseDown(wxMouseEvent& e);
	void onFocusLoss(wxFocusEvent& e);
	void onActivate(wxActivateEvent& e);
	void onMarginClick(wxStyledTextEvent& e);
	void onJumpToCalculateComplete(wxThreadEvent& e);
	void onJumpToChoiceSelected(wxCommandEvent& e);
	void onModified(wxStyledTextEvent& e);
	void onUpdateTimer(wxTimerEvent& e);
	void onStyleNeeded(wxStyledTextEvent& e);
};
