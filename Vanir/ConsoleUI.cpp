#include "VanirPCH.h"
#include "ConsoleUI.h"

#include "Window.h"
#include "Mouse.h"

ConsoleUI::ConsoleUI()
          : UI("Console")
          , m_InputBuf()
          , m_Items(ImVector<char*>())
          , m_Commands(ImVector<const char*>())
          , m_History(ImVector<char*>())
          , m_HistoryPos()
          , m_Filter()
          , m_AutoScroll(false)
          , m_ScrollToBottom(false)
{}
ConsoleUI::~ConsoleUI()
{
    const int itemsSize{ m_Items.Size };
    for (int i{}; i < itemsSize; ++i)
        free(m_Items[i]);
    m_Items.clear();

    const int historySize{ m_History.Size };
    for (int i{}; i < historySize; ++i)
        free(m_History[i]);
    m_History.clear();
}

void ConsoleUI::Initialize(const Vanir& /*vEditor*/)
{}
void ConsoleUI::Update()
{}
void ConsoleUI::FixedUpdate()
{
    std::queue<std::string>& messages = Locator::GetDebugLoggerService()->GetMessages();
    const size_t messagesSize{ messages.size() };
    for (size_t i = 0; i < messagesSize; ++i)
    {
        AddLog((messages.front()).c_str());
        messages.pop();
    }
}
void ConsoleUI::ShowWindow()
{
	if (!m_RenderUI)
		return;

    ImGuiWindowFlags window_flags{};
    BeginWindowBase(window_flags);
	Draw();
	EndWindowBase();
}

void ConsoleUI::ClearLog()
{
	for (int i = 0; i < m_Items.Size; i++)
		free(m_Items[i]);
	m_Items.clear();
}

void ConsoleUI::Draw()
{
    // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
    // So e.g. IsItemHovered() will return true when hovering the title bar.
    // Here we create a context menu only available from the title bar.
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            m_RenderUI = false;
        ImGui::EndPopup();
    }

    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    m_Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) ClearLog();
        ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or add custom widgets.
    // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
    // to only process visible items. The clipper will automatically measure the height of your first item and then
    // "seek" to display only items in the visible area.
    // To use the clipper we can replace your standard loop:
    //      for (int i = 0; i < Items.Size; i++)
    //   With:
    //      ImGuiListClipper clipper;
    //      clipper.Begin(Items.Size);
    //      while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // - That your items are evenly spaced (same height)
    // - That you have cheap random access to your elements (you can access them given their index,
    //   without processing all the ones before)
    // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
    // We would need random-access on the post-filtered list.
    // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
    // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
    // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
    // to improve this example code!
    // If your items are of variable height:
    // - Split them into same height items would be simpler and facilitate random-seeking into your list.
    // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    if (copy_to_clipboard)
        ImGui::LogToClipboard();
    for (int i = 0; i < m_Items.Size; i++)
    {
        const char* item = m_Items[i];
        if (!m_Filter.PassFilter(item))
            continue;

        // Normally you would store more information in your item than just a string.
        // (e.g. make Items[] an array of structure, store color/type etc.)
        ImVec4 color;
        bool has_color = false;
        if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
        else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }
    if (copy_to_clipboard)
        ImGui::LogFinish();

    if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
    m_ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();
}