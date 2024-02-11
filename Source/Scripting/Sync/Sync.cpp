#include "Sync.h"
#include "../../Events/Events.h"
#include "../Nodes/Nodes.h"
#include "../../Manager/Peds/Peds.h"
#include "../../Connections/Connections.h"

void Sync::Events(void)
{
    Events::Register("Sync::PlayerJoined", &Sync::PlayerJoined);
    Events::Register("Sync::PlayerUpdate", &Sync::PlayerUpdate);
}

void Sync::PlayerJoined(char** _args)
{
    Vector3 position(ToFloat(_args[0]),ToFloat(_args[1]),ToFloat(_args[2]));

    Nodes::InsertNode(position, Connections::GetOwnerServerId());
    Peds::Ped* playerPed = new Peds::Ped;
    playerPed->SetModel(_args[3]);
    
    playerPed->SetPosition(position);

    Peds::Insert(playerPed);
    Events::TriggerClient(Connections::GetOwnerServerId(), "Welcome", playerPed->GetServerId());
}

void Sync::PlayerUpdate(char** _args)
{//Dodu
    std::cout <<_args[3] << std::endl;
    Vector3 position(ToFloat(_args[0]),ToFloat(_args[1]),ToFloat(_args[2]));
    json syncData = json::parse(_args[3]);
    if (syncData.contains("peds") && syncData["peds"].is_array())
	{
		for (auto& pedData : syncData["peds"])
		{
            Peds::UpdateFromJson(pedData);
		}
	}
    int owner = Connections::GetOwnerServerId();
    Nodes::Lock();
    std::vector<Nodes::Node*> nodes = Nodes::GetNodes();
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->GetOwnerServerId() == owner)
        {
            nodes[i]->TriggerCallback();
            break;
        }
    }
    Nodes::Unlock();
}
