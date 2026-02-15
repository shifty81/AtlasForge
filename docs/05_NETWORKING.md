# Atlas Engine — Networking

## Unified Networking Model

Atlas supports multiple networking modes through a single abstraction:

| Mode           | Description                    |
|----------------|--------------------------------|
| Standalone     | No networking                  |
| ClientServer   | Classic dedicated server       |
| P2P_Host       | Peer-to-peer (host)            |
| P2P_Peer       | Peer-to-peer (peer)            |
| Hybrid         | Server + peer validation       |

## NetContext API

```cpp
enum class NetMode {
    Standalone,
    Client,
    Server,
    P2P_Host,
    P2P_Peer
};

struct NetPeer {
    uint32_t id;
    float rtt;
    bool connected;
};

class NetContext {
public:
    void Init(NetMode mode);
    void Shutdown();
    void Poll();
    void Send(uint32_t peerID, const Packet& pkt);
    void Broadcast(const Packet& pkt);
    void Flush();

    NetMode Mode() const;
    const std::vector<NetPeer>& Peers() const;
    bool IsAuthority() const;
};
```

## Replication Rules

### Authoritative State
- ECS component state
- Simulation ticks
- Graph events

### Client-Only State
- Camera
- UI
- Prediction

### Data Flow

| Data     | Direction            |
|----------|----------------------|
| Input    | Client → Authority   |
| State    | Authority → Clients  |
| Events   | Authority → All      |
| Assets   | Editor → Disk → Runtime |

## Lockstep + Rollback

- Fixed tick rate (e.g., 30 Hz)
- Inputs buffered by tick
- Graph VM deterministic
- ECS state snapshot per tick

### Rollback

```cpp
void OnLateInput(InputFrame f) {
    if (f.tick < currentTick) {
        RollbackTo(f.tick);
        ReplayFrom(f.tick);
    }
}
```

## Deterministic Sync

Systems opt-in to replication:

```cpp
class INetSyncSystem {
public:
    virtual void Serialize(Packet&) = 0;
    virtual void Deserialize(Packet&) = 0;
};
```
