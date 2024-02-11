#ifndef NODES_H
#define NODES_H
#include "../../Header.h"
#include "../../Events/Events.h"



namespace Nodes
{
    class Node
    {
    private:
        std::shared_mutex mutex;
        int ownerServerId;
        Vector3 position;
        json peds;
        json vehicles;
        json objects;
        json pickups;

        float radius;

    public:
        Node()
        {
            this->radius = 300.0f;
        }
        ~Node();

        float GetRadius()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->radius;
        }
        void SetRadius(float _radius)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->radius = _radius;
        }

        void InsertPed(json _pedData)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            peds.push_back(_pedData);
        }
        void InsertVehicle(json _vehicleData)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            vehicles.push_back(_vehicleData);
        }
        void InsertObject(json _objectData)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            objects.push_back(_objectData);
        }
        void InsertPickup(json _pickupData)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            pickups.push_back(_pickupData);
        }

        void TriggerCallback()
        {
            {
                std::shared_lock<std::shared_mutex> lock(this->mutex);
                json nextSyncEvent;
                nextSyncEvent["peds"] = this->peds;

                Events::TriggerClient(this->ownerServerId, "Nodes::Callback", nextSyncEvent.dump());
            }
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->peds.clear();
        }

        int GetOwnerServerId()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->ownerServerId; 
        }
        void SetOwnerServerId(int _ownerServerId)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->ownerServerId = _ownerServerId;
        }

        Vector3 GetPosition()
        {
            std::shared_lock<std::shared_mutex> lock(this->mutex);
            return this->position;
        }
        void SetPosition(Vector3 _position)
        {
            std::unique_lock<std::shared_mutex> lock(this->mutex);
            this->position = _position;
        }
    };


    void Lock();
    void Unlock();
    std::vector<Node*> GetNodes();
    void InsertNode(Vector3 _position, int _ownerServerId);
}

#endif //NODES_H