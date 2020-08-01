#include "NotificationWindow.h"

#include "Core/Settings.h"
#include "Overlay/imgui_utils.h"

#include <sstream>

void NotificationWindow::SetNotification(const std::string & text, float timeToShowInSec)
{
	if (!Settings::settingsIni.notificationpopups)
	{
		return;
	}

	m_notificationText = text;
	m_notificationTimer = timeToShowInSec;
	Open();
}

void NotificationWindow::BeforeDraw()
{
	ImGui::SetNextWindowPosCenter(ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, 50), ImVec2(500, 500));

	SetNotificationTextOnMainWindowTitle();
	UpdateNotificationTimer();
}

void NotificationWindow::Draw()
{
	//ImGui::Begin("Notification", NO_CLOSE_FLAG, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text(m_notificationText.c_str());

	std::ostringstream stringBuf;
	stringBuf << "OK (" << round(ceil(m_notificationTimer)) << ")";
	std::string timeLeft(stringBuf.str());

	const ImVec2 buttonSize = ImVec2(100, 30);
	AlignItemHorizontalCenter(buttonSize.x);

	if (ImGui::Button(timeLeft.c_str(), buttonSize))
	{
		m_notificationTimer = 0.0f;
		Close();
	}
}

void NotificationWindow::AfterDraw()
{
	
}

void NotificationWindow::SetNotificationTextOnMainWindowTitle()
{
	std::ostringstream stringBuf;
	stringBuf << m_notificationText << " (" << round(ceil(m_notificationTimer)) << ")";
	//SetMainWindowTitle(stringBuf.str().c_str());
}

void NotificationWindow::UpdateNotificationTimer()
{
	m_notificationTimer -= ImGui::GetIO().DeltaTime;

	if (m_notificationTimer < 0.0f)
	{
		SetNotificationTextOnMainWindowTitle(); // reset title to default
		Close();
	}
}
