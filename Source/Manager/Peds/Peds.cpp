#include "Peds.h"
#include "../../Connections/Connections.h"
#include "../../Scripting/Nodes/Nodes.h"


static std::shared_mutex mutex;
static int serverId = 0;

static std::vector<Peds::Ped*> peds;

void Peds::Insert(Ped* _pedPtr)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    _pedPtr->SetServerId(serverId);
    _pedPtr->SetBucketId(0);
    int ownerServerId = Connections::GetOwnerServerId();
    _pedPtr->SetOwnerServerId(ownerServerId);
    
    Nodes::Lock();
    std::vector<Nodes::Node*> nodes = Nodes::GetNodes();
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (ownerServerId != nodes[i]->GetOwnerServerId() && Vdist(nodes[i]->GetPosition(), _pedPtr->GetPosition()) <= nodes[i]->GetRadius())
        {
            std::cout << "found one node in range" << std::endl;
            _pedPtr->AddNode(nodes[i]);
        }
    }
    Nodes::Unlock();
    peds.push_back(_pedPtr);
    serverId++;
}

void Peds::Remove(int _serverId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    for (size_t i = 0; i < peds.size(); i++)
    {
        if (peds[i]->GetServerId() == _serverId)
        {
            delete peds[i];
            peds.erase(peds.begin() + i);
            return;
        }
    }
}

void Peds::UpdateFromJson(json _content)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
   	if (!_content.contains("serverId"))
	{
        return;
	}
    for (size_t i = 0; i < peds.size(); i++)
    {
        if (peds[i]->GetServerId() == _content["serverId"]) // mutex already set in the functions
        {// Update the position
            if (peds[i]->refresh.GetElapsedTime() < REFRESH_TIME)
            {
                std::cout << "Updating ped position" << std::endl;
                if (_content.contains("position"))
                {
                    peds[i]->SetPosition(Vector3(_content["position"][0],_content["position"][1],_content["position"][2]));
                }
            }
            else
            {
                std::cout << "Inserting in nodes" << std::endl;
                Nodes::Lock();
                std::vector<Nodes::Node*> nodes = Nodes::GetNodes();
                for (size_t nodeId = 0; nodeId < nodes.size(); nodeId++)
                {
                    if (nodes[nodeId]->GetOwnerServerId() == peds[i]->GetOwnerServerId()) { continue; }
                    if (Vdist(nodes[nodeId]->GetPosition(), peds[i]->GetPosition()) < nodes[nodeId]->GetRadius())
                    {
                        peds[i]->AddNode(nodes[nodeId]);
                    }
                }
                Nodes::Unlock();
                
                peds[i]->refresh.Restart();
            }
            return;
        }
    }
}


void Peds::OwnerRemoval(int _ownerServerId)
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (size_t i = 0; i < peds.size(); i++)
    {
        if (peds[i]->GetOwnerServerId() == _ownerServerId)
        {
            peds[i]->OwnerRemoval(_ownerServerId);
            return;
        }
    }
}