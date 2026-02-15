#include "NetContext.h"

namespace atlas::net {

void NetContext::Init(NetMode mode) {
    m_mode = mode;
    m_peers.clear();
    m_snapshots.clear();
}

void NetContext::Shutdown() {
    m_peers.clear();
    m_snapshots.clear();
    m_mode = NetMode::Standalone;
}

void NetContext::Poll() {
    // Stub: poll network events, receive packets
}

void NetContext::Send(uint32_t /*peerID*/, const Packet& /*pkt*/) {
    // Stub: send packet to specific peer
}

void NetContext::Broadcast(const Packet& /*pkt*/) {
    // Stub: send packet to all peers
}

void NetContext::Flush() {
    // Stub: flush outgoing packets
}

NetMode NetContext::Mode() const {
    return m_mode;
}

const std::vector<NetPeer>& NetContext::Peers() const {
    return m_peers;
}

bool NetContext::IsAuthority() const {
    return m_mode == NetMode::Server || m_mode == NetMode::P2P_Host;
}

void NetContext::SaveSnapshot(uint32_t tick) {
    WorldSnapshot snap;
    snap.tick = tick;
    // Stub: serialize ECS state
    m_snapshots.push_back(std::move(snap));
}

void NetContext::RollbackTo(uint32_t tick) {
    // Stub: restore ECS state from snapshot at given tick
    while (!m_snapshots.empty() && m_snapshots.back().tick > tick) {
        m_snapshots.pop_back();
    }
}

void NetContext::ReplayFrom(uint32_t /*tick*/) {
    // Stub: replay input frames from given tick to current tick
}

}
