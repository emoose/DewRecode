#pragma once
#include <ElDorito/ElDorito.hpp>
#include <map>
#include "Utils/Utils.hpp"
#include <ElDorito/CustomPackets.hpp>
#include <unordered_map>

// handles game events and callbacks for different modules/plugins
// if you make any changes to this class make sure to update the exported interface (create a new interface + inherit from it if the interface already shipped)
class Engine : public IEngine
{
public:
	bool OnTick(TickCallback callback);
	bool OnEndScene(EventCallback callback);
	bool OnWndProc(WndProcCallback callback);
	bool OnEvent(const std::string& eventNamespace, const std::string& eventName, EventCallback callback);

	bool RemoveOnTick(TickCallback callback);
	bool RemoveOnWndProc(WndProcCallback callback);
	bool RemoveOnEvent(const std::string& eventNamespace, const std::string& eventName, EventCallback callback);

	void Event(const std::string& eventNamespace, const std::string& eventName, void* param = 0);

	bool RegisterInterface(const std::string& interfaceName, void* ptrToInterface);
	void* CreateInterface(const std::string& interfaceName, int* returnCode);

	bool HasMainMenuShown() { return hasMainMenuShown; }
	bool IsDedicated();

	HWND GetGameHWND() { return Pointer(0x199C014).Read<HWND>(); }
	Pointer GetMainTls(size_t offset = 0);
	Blam::ArrayGlobal* GetArrayGlobal(size_t offset);

	std::string GetDoritoVersionString() { return Utils::Version::GetVersionString(); }
	DWORD GetDoritoVersionInt() { return Utils::Version::GetVersionInt(); }

	std::pair<int, int> GetGameResolution()
	{
		return std::pair<int, int>(Pointer(0x2301D08).Read<int>(), Pointer(0x2301D0C).Read<int>());
	}

	uint32_t GetServerIP();
	std::string GetPlayerName();

	Blam::Network::Session* GetActiveNetworkSession();
	int GetNumPlayers();
	Blam::Network::PacketTable* GetPacketTable();
	void SetPacketTable(const Blam::Network::PacketTable* newTable);

	uint8_t GetKeyTicks(Blam::Input::KeyCodes key, Blam::Input::InputType type);
	uint16_t GetKeyMs(Blam::Input::KeyCodes key, Blam::Input::InputType type);
	bool ReadKeyEvent(Blam::Input::KeyEvent* result, Blam::Input::InputType type);
	void BlockInput(Blam::Input::InputType type, bool block);
	void PushInputContext(std::shared_ptr<InputContext> context);

	void SendPacket(int targetPeer, const void* packet, int packetSize);

	Packets::PacketGuid RegisterPacketImpl(const std::string &name, std::shared_ptr<Packets::RawPacketHandler> handler);
	CustomPacket* LookUpPacketType(Packets::PacketGuid guid);

	/* CHAT COMMANDS */
	// Sends a message to every peer. Returns true if successful.
	bool SendChatGlobalMessage(const std::string &body);

	// Sends a message to every player on the local player's team. Returns
	// true if successful.
	bool SendChatTeamMessage(const std::string &body);

	// Sends a server message to specific peers. Only works if you are
	// host. Returns true if successful.
	bool SendChatServerMessage(const std::string &body, Chat::PeerBitSet peers);

	bool SendChatDirectedServerMessage(const std::string& body, int peer);

	// Registers a chat handler object.
	void AddChatHandler(std::shared_ptr<Chat::ChatHandler> handler);

	bool SetModEnabled(const std::string& guid, bool enabled);
	bool GetModEnabled(const std::string& guid);

	// functions that aren't exposed over IEngine interface
	void Tick(const std::chrono::duration<double>& deltaTime);
	void EndScene(void* d3dDevice);
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Engine();
	~Engine();
private:
	bool hasMainMenuShown = false;
	bool hasFirstTickTocked = false;
	std::vector<TickCallback> tickCallbacks;
	std::vector<EventCallback> endSceneCallbacks;
	std::vector<WndProcCallback> wndProcCallbacks;
	std::map<std::string, std::vector<EventCallback>> eventCallbacks;
	std::map<std::string, void*> interfaces;
	std::unordered_map<Packets::PacketGuid, CustomPacket> customPackets;

	PatchSet* enginePatchSet;
};
