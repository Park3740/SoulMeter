#include "pch.h"
#include ".\UI\Option.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Damage Meter\History.h"
#include ".\UI\PlayerTable.h"
#include ".\Damage Meter\MySQLite.h"
#include ".\UI\UiWindow.h"

PlayerTable::PlayerTable() : _tableResize(0), _globalFontScale(0), _columnFontScale(0), _tableFontScale(0), _curWindowSize(0) {

}

PlayerTable::~PlayerTable() {
	ClearTable();
}

VOID PlayerTable::ClearTable() {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		delete (*itr)->_specificInfo;
	}

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		delete (*itr);
	}

	_selectInfo.clear();
	_curWindowSize = 0;
}

VOID PlayerTable::SetupFontScale() {

	ImFont* font = ImGui::GetFont();

	_globalFontScale = font->Scale;
	_columnFontScale = _globalFontScale * UIOPTION.GetColumnFontScale();
	_tableFontScale = _globalFontScale * UIOPTION.GetTableFontScale();
}

VOID PlayerTable::ResizeTalbe() {
	_tableResize = TRUE;
}

VOID PlayerTable::Update() {

	DAMAGEMETER.GetLock();
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 prevWindowPadding = style.WindowPadding;
		style.WindowPadding.x = 0;
		style.WindowPadding.y = 0;

		ImVec4 prevInActiveColor = style.Colors[10];
		ImVec4 prevActiveColor = style.Colors[11];

		if (DAMAGEMETER.isRun()) {
			style.Colors[10] = UIOPTION.GetActiveColor();
			style.Colors[11] = UIOPTION.GetActiveColor();
		}
		else {
			style.Colors[10] = UIOPTION.GetInActiveColor();
			style.Colors[11] = UIOPTION.GetInActiveColor();
		}

		SetupFontScale();

		ImGuiWindowFlags windowFlag = ImGuiWindowFlags_None;
		windowFlag |= (ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

		if (!UIOPTION.isOption())
			windowFlag |= ImGuiWindowFlags_NoResize;

		CHAR title[128] = { 0 };
		sprintf_s(title, 128, "%s : %02d:%02d###DamageMeter", DAMAGEMETER.GetWorldName(), (UINT)DAMAGEMETER.GetTime() / 60, (UINT)DAMAGEMETER.GetTime() % 60);
		ImGui::Begin(title, 0, windowFlag);
		{
			if(!UIOPTION.isOption() || _tableResize)
				SetWindowSize();

			if(UIOPTION.isOption())
				StoreWindowWidth();

			SetMainWindowSize();

			BeginPopupMenu();

			ImGui::OutlineText::PushOutlineText(ImGui::IMGUIOUTLINETEXT(UIOPTION.GetOutlineColor(), 1));
			ImGui::TextAlignCenter::SetTextAlignCenter();
			{
				SetupTable();
			}
			ImGui::TextAlignCenter::UnSetTextAlignCenter();
			ImGui::OutlineText::PopOutlineText();
		}
		ImGui::End();

		ShowSelectedTable();

		style.WindowPadding.x = prevWindowPadding.x;
		style.WindowPadding.y = prevWindowPadding.y;

		style.Colors[10] = prevInActiveColor;
		style.Colors[11] = prevActiveColor;
	}
	DAMAGEMETER.FreeLock();
}

VOID PlayerTable::SetWindowSize() {

	_tableResize = FALSE;

	ImGuiStyle& style = ImGui::GetStyle();

	if (ImGui::GetScrollMaxY() > 0)
		_curWindowSize += ImGui::GetScrollMaxY();

	ImGui::SetWindowSize(ImVec2(UIOPTION.GetWindowWidth(), FLOOR(_curWindowSize)));
}

VOID PlayerTable::SetMainWindowSize() {

	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	SetWindowPos(UIWINDOW.GetHWND(), HWND_TOPMOST, pos.x, pos.y, size.x + 1, size.y + 1, SWP_NOACTIVATE);
}

VOID PlayerTable::StoreWindowWidth() {
	UIOPTION.SetWindowWidth(ImGui::GetWindowSize().x);
}

VOID PlayerTable::BeginPopupMenu() {

	if(ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem(u8"시작/정지")) {
			DAMAGEMETER.Toggle();
		}

		if (ImGui::MenuItem(u8"초기화")) {
			DAMAGEMETER.Clear();
			PLAYERTABLE.ClearTable();
		}

		
		bool history_open = false;
		if (HISTORY.size() > 0)
			history_open = true;

		if (ImGui::BeginMenu(u8"전투내역", history_open)) {

			for (INT i = 0; i < HISTORY.size(); i++) {
				
				auto history = HISTORY[i];
				CHAR label[128] = { 0 };

				CHAR mapName[MAX_MAP_LEN] = { 0 };
				SWDB.GetMapName(history._worldID, mapName, MAX_MAP_LEN);

				sprintf_s(label, 128, u8"%s %d:%d:%d", mapName, history._saveTime.wHour, history._saveTime.wMinute, history._saveTime.wSecond);
				if (ImGui::MenuItem(label)) {
					DAMAGEMETER.SetHistory(i);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem(u8"옵션")) {
			UIOPTION.OpenOption();
		}

		if (ImGui::MenuItem(u8"종료"))
			exit(1);

		ImGui::EndPopup();
	}
}

VOID PlayerTable::SetupTable() {

	ImGuiTableFlags tableFlags = ImGuiTableFlags_None;
	tableFlags |= (ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable);

	if(ImGui::BeginTable("Player Table", 8, tableFlags)) {
	
		ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_None;
		columnFlags |= ImGuiTableColumnFlags_NoSort;

		ImGui::SetWindowFontScale(_columnFontScale);

		ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoClip | ImGuiTableColumnFlags_WidthFixed | columnFlags, -1);
		ImGui::TableSetupColumn("DPS", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("D%", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("DAMAGE", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("HIT", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("CRIT", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("HIT/s", columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("MAXC", columnFlags | ImGuiTableColumnFlags_WidthStretch, -1);
		ImGui::TableHeadersRow();

		FLOAT window_width = ImGui::GetWindowWidth();

		ImGui::SetWindowFontScale(_tableFontScale);

		UpdateTable(window_width);

		ImGui::SetWindowFontScale(_globalFontScale);

		ImGui::EndTable();
	}

}

VOID PlayerTable::UpdateTable(FLOAT windowWidth) {

	UINT64 max_Damage = 1;
	CHAR comma[128] = { 0 }; CHAR label[128] = { 0 };

	for (auto itr = DAMAGEMETER.begin(); itr != DAMAGEMETER.end(); itr++) {

		if (itr == DAMAGEMETER.begin())
			max_Damage = (*itr)->GetDamage();

		FLOAT damage_percent = (DOUBLE)(*itr)->GetDamage() / (DOUBLE)max_Damage;

		if (damage_percent > 1)
			damage_percent = 1;
		else if (damage_percent < 0)
			damage_percent = 0;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		DrawBar(windowWidth, damage_percent, UIOPTION.GetJobColor(DAMAGEMETER.GetPlayerJob((*itr)->GetID())));

		// NAME
		if (ImGui::Selectable(DAMAGEMETER.GetPlayerName((*itr)->GetID()), false, ImGuiSelectableFlags_SpanAllColumns))
			ToggleSelectInfo((*itr)->GetID());

		ImGui::TableNextColumn();

		// DPS
		DOUBLE dps = ((DOUBLE)(*itr)->GetDamage()) / DAMAGEMETER.GetTime();
		if (UIOPTION.is1K())
			dps /= 1000;
		else if (UIOPTION.is1M())
			dps /= 1000000;
		sprintf_s(label, 128, "%.0lf", dps);
		TextCommma(label, comma);
		if (UIOPTION.is1K())
			strcat_s(comma, 128, "K");
		else if (UIOPTION.is1M())
			strcat_s(comma, 128, "M");
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// D%
		sprintf_s(label, 128, "%.0lf", ((DOUBLE)(*itr)->GetDamage() / (DOUBLE)DAMAGEMETER.GetPlayerTotalDamage()) * 100);
		ImGui::Text(label);

		ImGui::TableNextColumn();

		// DAMAGE
		UINT64 damage = (*itr)->GetDamage();
		if (UIOPTION.is1K())
			damage /= 1000;
		else if (UIOPTION.is1M())
			damage /= 1000000;
		sprintf_s(label, 128, "%llu", damage);
		TextCommma(label, comma);
		if (UIOPTION.is1K())
			strcat_s(comma, 128, "K");
		else if (UIOPTION.is1M())
			strcat_s(comma, 128, "M");
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// HIT
		sprintf_s(label, 128, "%d", (*itr)->GetHitCount());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// CRIT
		FLOAT crit = 0;

		if ((*itr)->GetHitCount() != 0)
			crit = (FLOAT)(*itr)->GetCritHitCount() / (FLOAT)(*itr)->GetHitCount() * 100;

		sprintf_s(label, 128, "%.0f", crit);
		ImGui::Text(label);

		ImGui::TableNextColumn();

		// HIT/S
		sprintf_s(label, 128, "%.2lf", (DOUBLE)(*itr)->GetHitCount() / DAMAGEMETER.GetTime());
		ImGui::Text(label);

		ImGui::TableNextColumn();

		// MAXC
		sprintf_s(label, 128, "%d", (*itr)->GetMaxCombo());
		TextCommma(label, comma);
		ImGui::Text(comma);
	}
}

VOID PlayerTable::DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color) {

	auto draw_list = ImGui::GetWindowDrawList();

	FLOAT result_width = window_Width * percent;
	FLOAT height = ImGui::GetFontSize();
	ImVec2 line = ImGui::GetCursorScreenPos();

	line.x = FLOOR(line.x);	line.y = line.y;
	height = height;

	draw_list->AddRectFilled(ImVec2(line.x, line.y), ImVec2(line.x + result_width, line.y + height), color, 0, 0);
}

BOOL PlayerTable::ToggleSelectInfo(UINT32 id) {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		if ((*itr)->_playerID == id) {
			(*itr)->_isSelected = !(*itr)->_isSelected;

			return (*itr)->_isSelected;
		}
	}

	SELECTED_PLAYER* selectinfo = new SELECTED_PLAYER(id, TRUE, new SpecificInformation(id));
	_selectInfo.push_back(selectinfo);

	return selectinfo->_isSelected;
}

VOID PlayerTable::ShowSelectedTable() {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		if ((*itr)->_isSelected == TRUE) {
			(*itr)->_specificInfo->Update(&(*itr)->_isSelected);
		}
	}
}