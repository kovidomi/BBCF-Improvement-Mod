#include "DebugWindow.h"

#include "Core/interfaces.h"

void DebugWindow::Draw()
{
	if (ImGui::CollapsingHeader("Gameval addresses"))
	{
	//	if (!g_interfaces.player1.GetChar1().IsCharDataNullPtr())
	//		ImGui::Text("pP1Char1_Data 0x%p", g_interfaces.player1.GetChar1().GetData());

	//	if (!g_interfaces.player1.GetChar2().IsCharDataNullPtr())
	//		ImGui::Text("pP1Char2_Data 0x%p", g_interfaces.player1.GetChar2().GetData());

	//	if (!g_interfaces.player2.GetChar1().IsCharDataNullPtr())
	//		ImGui::Text("pP2Char1_Data 0x%p", g_interfaces.player2.GetChar1().GetData());

	//	if (!g_interfaces.player2.GetChar2().IsCharDataNullPtr())
	//		ImGui::Text("pP2Char2_Data 0x%p", g_interfaces.player2.GetChar2().GetData());

	//	ImGui::Separator();
	//	ImGui::Text("pP1Meters 0x%p", g_interfaces.player1.GetMeters());
	//	ImGui::Text("pP2Meters 0x%p", g_interfaces.player2.GetMeters());

	//	ImGui::Separator();
	//	if (!g_interfaces.player1.GetChar1().IsCharDataNullPtr())
	//		ImGui::Text("P1Char1 meter: %d / %d",
	//			g_interfaces.player1.GetChar1().GetData()->unique_meter_cur_val,
	//			g_interfaces.player1.GetChar1().GetData()->unique_meter_max_val);

	//	if (!g_interfaces.player1.GetChar2().IsCharDataNullPtr())
	//		ImGui::Text("P1Char2 meter: %d / %d",
	//			g_interfaces.player1.GetChar2().GetData()->unique_meter_cur_val,
	//			g_interfaces.player1.GetChar2().GetData()->unique_meter_max_val);

	//	if (!g_interfaces.player2.GetChar1().IsCharDataNullPtr())
	//		ImGui::Text("P2Char1 meter: %d / %d",
	//			g_interfaces.player2.GetChar1().GetData()->unique_meter_cur_val,
	//			g_interfaces.player2.GetChar1().GetData()->unique_meter_max_val);

	//	if (!g_interfaces.player2.GetChar2().IsCharDataNullPtr())
	//		ImGui::Text("P2Char2 meter: %d / %d",
	//			g_interfaces.player2.GetChar2().GetData()->unique_meter_cur_val,
	//			g_interfaces.player2.GetChar2().GetData()->unique_meter_max_val);

	//	ImGui::Separator();
	//	ImGui::Text("pPalIndex_P1Char1 0x%p", &(g_interfaces.player1.GetChar1().GetPalHandle().GetPalIndexRef()));
	//	ImGui::Text("pPalIndex_P1Char2 0x%p", &(g_interfaces.player1.GetChar2().GetPalHandle().GetPalIndexRef()));
	//	ImGui::Text("pPalIndex_P2Char1 0x%p", &(g_interfaces.player2.GetChar1().GetPalHandle().GetPalIndexRef()));
	//	ImGui::Text("pPalIndex_P2Char2 0x%p", &(g_interfaces.player2.GetChar2().GetPalHandle().GetPalIndexRef()));

	//	const int PAL_INDEX_MIN = 0;
	//	const int PAL_INDEX_MAX = 19;
	//	if (!g_interfaces.player1.GetChar1().GetPalHandle().IsNullPointerPalIndex())
	//		ImGui::SliderInt("PalIndex_P1Char1", &g_interfaces.player1.GetChar1().GetPalHandle().GetPalIndexRef(), PAL_INDEX_MIN, PAL_INDEX_MAX);

	//	if (!g_interfaces.player1.GetChar2().GetPalHandle().IsNullPointerPalIndex())
	//		ImGui::SliderInt("PalIndex_P1Char2", &g_interfaces.player1.GetChar2().GetPalHandle().GetPalIndexRef(), PAL_INDEX_MIN, PAL_INDEX_MAX);

	//	if (!g_interfaces.player2.GetChar1().GetPalHandle().IsNullPointerPalIndex())
	//		ImGui::SliderInt("PalIndex_P2Char1", &g_interfaces.player2.GetChar1().GetPalHandle().GetPalIndexRef(), PAL_INDEX_MIN, PAL_INDEX_MAX);

	//	if (!g_interfaces.player2.GetChar2().GetPalHandle().IsNullPointerPalIndex())
	//		ImGui::SliderInt("PalIndex_P2Char2", &g_interfaces.player2.GetChar2().GetPalHandle().GetPalIndexRef(), PAL_INDEX_MIN, PAL_INDEX_MAX);

	//	//ImGui::Separator();
	//	//ImGui::Text("PalBaseAddrP1Char1 0x%p", g_gameVals.PalBaseAddrP1Char1);
	//	//ImGui::Text("PalBaseAddrP1Char2 0x%p", g_gameVals.PalBaseAddrP1Char2);
	//	//ImGui::Text("PalBaseAddrP2Char1 0x%p", g_gameVals.PalBaseAddrP2Char1);
	//	//ImGui::Text("PalBaseAddrP2Char2 0x%p", g_gameVals.PalBaseAddrP2Char2);

	//	ImGui::Separator();
	//	ImGui::Text("pGameState: 0x%p : %d", g_gameVals.pGameState, *g_gameVals.pGameState);
	//	ImGui::Text("pGameMode: 0x%p : %d", g_gameVals.pGameMode, *g_gameVals.pGameMode);
	//	ImGui::Text("pIsHUDHidden: 0x%p", g_gameVals.pIsHUDHidden);
	//	if (g_gameVals.pIsHUDHidden)
	//		ImGui::Checkbox("pIsHUDHidden", (bool*)g_gameVals.pIsHUDHidden);
	//}

	//if (ImGui::CollapsingHeader("Color tester"))
	//{
	//	static float col[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	//	ImVec4 color(col[0], col[1], col[2], col[3]);
	//	ImGui::TextColored(color, "ABCD 0123");
	//	ImGui::ColoredProgressBar(1.0f, ImVec2(-1.0f, 0.0f), color, NULL, NULL, NULL);
	//	ImGui::ColorEdit4("ColEdit", col);
	}

	static bool showDemoWindow = false;
	if (ImGui::Button("Demo"))
	{
		showDemoWindow = !showDemoWindow;
	}

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
