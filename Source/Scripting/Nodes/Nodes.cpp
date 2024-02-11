#include "Nodes.h"

static std::shared_mutex mutex;
static std::vector<Nodes::Node*> nodes;

void Nodes::Lock()
{
    mutex.lock();
}

void Nodes::Unlock()
{
    mutex.unlock();
}

std::vector<Nodes::Node*> Nodes::GetNodes()
{
    return nodes;
}

void Nodes::InsertNode(Vector3 _position, int _ownerServerId)
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    Node* node = new Node;
    node->SetOwnerServerId(_ownerServerId);
    node->SetPosition(_position);
    nodes.push_back(node);
}


