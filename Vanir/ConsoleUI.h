#pragma once
#include "UI.h"

/** Logging to console logic from ImGui::ShowDemoWIndow() */
class ConsoleUI final : public UI
{
	public:
		ConsoleUI();
		~ConsoleUI();

		ConsoleUI(const ConsoleUI&) = delete;
		ConsoleUI& operator=(const ConsoleUI&) = delete;
		ConsoleUI(ConsoleUI&&) = delete;
		ConsoleUI& operator=(ConsoleUI&&) = delete;

		void Initialize(const Vanir& vEditor) override;
		void Update() override;
		void FixedUpdate() override;
		void ShowWindow() override;

	protected:

	private:
		char                  m_InputBuf[256];
		ImVector<char*>       m_Items;
		ImVector<const char*> m_Commands;
		ImVector<char*>       m_History;
		int                   m_HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImGuiTextFilter       m_Filter;
		bool                  m_AutoScroll;
		bool                  m_ScrollToBottom;

        // Portable helpers
        static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
        static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
        static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
        static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

		void AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			// FIXME-OPT
			char buf[1024];
			va_list args;
			va_start(args, fmt);
			vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
			buf[IM_ARRAYSIZE(buf) - 1] = 0;
			va_end(args);
			m_Items.push_back(Strdup(buf));
		}

        void ClearLog();
        void Draw();
};