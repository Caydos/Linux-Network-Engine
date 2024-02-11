#ifndef PEDS_H
#define PEDS_H
#include "../../Header.h"
#include "../Types.h"
#include "../../Scripting/Nodes/Nodes.h"
#include "../../Events/Events.h"

#define REFRESH_TIME 1000

namespace Peds
{
    class Ped
    {
        private:
        std::vector<Nodes::Node*> nodes;
        
        int serverId;
        int ownerServerId;
        int bucketId;

        Vector3 position;
        std::string model;

        public:
        std::shared_mutex mutex;
        Clock refresh;
        
        void TriggerDeletion()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            for (size_t i = 0; i < nodes.size(); i++)
            {
                json object;
                object["serverId"] = this->serverId;
                object["deletion"] = true;
                nodes[i]->InsertPed(object);
            }
        }

        void AddNode(Nodes::Node* _node)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            json object;
            object["serverId"] = this->serverId;
            object["ownerServerId"] = this->ownerServerId;
            object["position"] = { this->position.x, this->position.y, this->position.z };
            object["model"] = this->model;

            _node->InsertPed(object);
            nodes.push_back(_node);
        }

        void OwnerRemoval(int _ownerServerId)
        {
            // std::unique_lock<std::shared_mutex> lock(mutex); // deadlock
            for (size_t nodeId = 0; nodeId < nodes.size(); nodeId++)
            {
                if (_ownerServerId != nodes[nodeId]->GetOwnerServerId())
                {
                    nodes.erase(nodes.begin() + nodeId);
                    break;
                }
            }
            for (size_t nodeId = 0; nodeId < nodes.size(); nodeId++)
            {
                if (_ownerServerId != nodes[nodeId]->GetOwnerServerId())
                {
                    SetOwnerServerId(nodes[nodeId]->GetOwnerServerId());
                    return;
                }
            }
            TriggerDeletion(); // Triggered if no one is in it's node
        }

        int GetServerId()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->serverId;
        }
        void SetServerId(int _serverId)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->serverId = _serverId;
        }

        int GetOwnerServerId()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->ownerServerId;
        }
        void SetOwnerServerId(int _ownerServerId)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            for (size_t i = 0; i < nodes.size(); i++)
            {
                json object;
                object["serverId"] = this->serverId;
                object["ownerServerId"] = _ownerServerId;
                nodes[i]->InsertPed(object);
            }
            this->ownerServerId = _ownerServerId;
        }

        int GetBucketId() { return this->bucketId; }
        void SetBucketId(int _bucketId) { this->bucketId = _bucketId; }

        Vector3 GetPosition()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->position;
        }
        void SetPosition(Vector3 _position)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->position = _position;
            for (size_t i = 0; i < nodes.size(); i++)
            {
                json object;
                object["serverId"] = this->serverId;
                object["ownerServerId"] = this->ownerServerId;
                object["position"] = {_position.x, _position.y, _position.z};
                nodes[i]->InsertPed(object);
            }
        }

        std::string GetModel()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->model;
        }
        void SetModel(std::string _model)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            for (size_t i = 0; i < nodes.size(); i++)
            {
                json object;
                object["serverId"] = this->serverId;
                object["model"] = _model;
                nodes[i]->InsertPed(object);
            }
            this->model = _model;
        }

    };

    void Insert(Ped* _pedPtr);
    void Remove(int _serverId);
    void UpdateFromJson(json _content);
    void OwnerRemoval(int _ownerServerId);
} // namespace Peds


#endif //PEDS_H