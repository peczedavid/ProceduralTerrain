#pragma once

#include "Core/Core.h"
#include <imgui.h>

class Log
{
private:
	struct ImGuiLog
	{
		ImGuiTextBuffer     Buf;
		ImVector<int>       LineOffsets;
		bool                AutoScroll;

		ImGuiLog()
		{
			AutoScroll = true;
			Clear();
		}

		void Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void AddTrace(const char* fmt, ...) IM_FMTARGS(2)
		{
			const std::time_t t = std::time(0);
			const std::tm* now = std::localtime(&t);
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendf("[%02d:%02d:%02d] T - ", now->tm_hour, now->tm_min, now->tm_sec);
			Buf.appendfv(fmt, args);
			Buf.appendf("\n");
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void AddInfo(const char* fmt, ...) IM_FMTARGS(2)
		{
			const std::time_t t = std::time(0);
			const std::tm* now = std::localtime(&t);
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendf("[%02d:%02d:%02d] I - ", now->tm_hour, now->tm_min, now->tm_sec);
			Buf.appendfv(fmt, args);
			Buf.appendf("\n");
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void AddWarn(const char* fmt, ...) IM_FMTARGS(2)
		{
			const std::time_t t = std::time(0);
			const std::tm* now = std::localtime(&t);
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendf("[%02d:%02d:%02d] W - ", now->tm_hour, now->tm_min, now->tm_sec);
			Buf.appendfv(fmt, args);
			Buf.appendf("\n");
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void AddError(const char* fmt, ...) IM_FMTARGS(2)
		{
			const std::time_t t = std::time(0);
			const std::tm* now = std::localtime(&t);
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendf("[%02d:%02d:%02d] E - ", now->tm_hour, now->tm_min, now->tm_sec);
			Buf.appendfv(fmt, args);
			Buf.appendf("\n");
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void Draw(const char* title)
		{
			if (!ImGui::Begin(title))
			{
				ImGui::End();
				return;
			}

			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			ImGui::Separator();

			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				Clear();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					const int line_len = strlen(line_start) - strlen(line_end);
					char line[1024];
					memcpy(line, line_start, line_len);
					line[1023] = '\0';
					const char* trace = strstr(line, "] T -");
					const char* info = strstr(line, "] I -");
					const char* warn = strstr(line, "] W -");
					const char* error = strstr(line, "] E -");
					if (trace != nullptr)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
					else if (info != nullptr)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
					else if (warn != nullptr)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
					else if (error != nullptr)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));

					ImGui::TextUnformatted(line_start, line_end);
					
					ImGui::PopStyleColor();
				}
			}
			clipper.End();
			ImGui::PopStyleVar();

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
			ImGui::End();
		}
	};
public:
	static void Initialize();

	static void Draw();

	inline static Ref<ImGuiLog>& GetLogger() { return s_Logger; }
private:
	static Ref<ImGuiLog> s_Logger;
};

#define TRACE(...) Log::GetLogger()->AddTrace(__VA_ARGS__)
#define INFO(...) Log::GetLogger()->AddInfo(__VA_ARGS__)
#define WARN(...) Log::GetLogger()->AddWarn(__VA_ARGS__)
#define ERROR(...) Log::GetLogger()->AddError(__VA_ARGS__)