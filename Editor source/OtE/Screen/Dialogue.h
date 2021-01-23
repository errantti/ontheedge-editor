// On the Edge Editor
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef __CDIALOGUE_H__
#define __CDIALOGUE_H__

#include <vector>
#include <fstream>
#include "..\\Edgewalker\\DirectX9\\Texture.h"
#include "..\\Edgewalker\\EWScript\\Script.h"
#include "..\\Edgewalker\\Input\\IInputReceiver.h"

#define MAX_SPEAKER_NAME_LENGTH		32
#define MAX_COMMENT_LENGTH			1024

#define MAX_MISSION_OBJECTIVE_LENGTH	256

#define DIALOGUE_FADE_TIME			0.5f

enum DIALOGUE_STATE 
{
	DIALOGUE_STATE_FADE_IN=0,
	DIALOGUE_STATE_DIALOGUE,
	DIALOGUE_STATE_FADE_OUT
};

class CDialogueWindow;

/**
 * CDialogue contains a single dialogue between any number
 * of targets
 */
class CDialogue
{
public:
	struct SComment
	{
		char speaker[MAX_SPEAKER_NAME_LENGTH];
		char comment[MAX_COMMENT_LENGTH];

		void FillDefaults()
		{
			speaker[0] = '\0';
			comment[0] = '\0';
		}
	};

	CDialogue();
	~CDialogue();

	inline const int GetID() const {return m_ID;}
	inline void SetID(int id) {m_ID = id;}

	inline std::string &GetName() {return m_name;}
	inline void SetName(std::string name) {m_name = name;}

	/// Opens the dialogue
	void OpenDialogue();
	/// Opens the dialogue regardless of functional state
	void ForceOpenDialogue();

	/// Signals the dialogue to close itself as soon as possible and execute on close script
	void CloseDialogue();
	/// Immediately starts closing the dialogue
	void ForceCloseDialogue();

	void NextComment();

	inline vector<SComment> *GetComments() {return &m_comments;}
	inline void AddComment(SComment s) {m_comments.push_back(s);}
	inline void ClearComments() {m_comments.clear();}

	inline bool GetUpdatesMissionObjective() {return m_bUpdateMissionObjective;}
	inline int GetMissionPriority() {return m_missionPriority;}
	inline std::string GetNewMissionObjective() {return m_newMissionObjective;}
	inline char *GetOnDialogueEndScriptName() {return m_dialogueEndScriptName;}

	inline void SetUpdatesMissionObjective(bool value) {m_bUpdateMissionObjective = value;}
	inline void SetMissionPriority(int value) {m_missionPriority = value;}
	inline void SetNewMissionObjective(std::string str) {m_newMissionObjective = str;}
	inline void SetOnDialogueEndScriptName(const char *str) {strcpy_s(m_dialogueEndScriptName, 31, str);}

	inline bool IsAlreadyShown() {return m_bAlreadyShown;}
	inline void SetIsAlreadyShown(bool value) {m_bAlreadyShown = value;}

	void SetOnDialogueEndScript(CScript *script) {m_onDialogueEnd = script;}

	void ProcessInput();

	bool ReadDialogueInfoFromStream(std::fstream &f);
	void WriteDialogueInfoIntoStream(std::fstream &f);

	void UpdateOnEndScriptReference();

	void SetDialogueWindowHandler(CDialogueWindow *handler) {m_dialogueWnd = handler;}
	void ReportDialogueWindowClosed();

private:
	vector<SComment> m_comments;
	std::string m_name;
	long m_ID, m_currentComment;
	float m_fadeTimer;
	DIALOGUE_STATE m_state;

	// m_bAlreadyShown = has the dialogue already been shown in this level
	bool m_bNextButtonDown, m_bAlreadyShown;
	bool m_bSkippable;
	bool m_bOpen;

	bool m_bUpdateMissionObjective;
	int m_missionPriority;
	std::string m_newMissionObjective;

	char m_dialogueEndScriptName[32];
	CScript *m_onDialogueEnd;

	CDialogueWindow *m_dialogueWnd;
};

#endif // #ifndef __CDIALOGUE_H__
