// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "MsgQueueTypes.h"
#include "HardwareComponent.h"
#include "RingBuffer.h"

class MsgQueue : public HardwareComponent {
        
    // Ring buffer storing all pending messages
    util::RingBuffer<Message, 64> queue;
            
    // List of registered listeners
    std::vector<std::pair <const void *, Callback *> > listeners;

    
    //
    // Methods from HardwareComponent
    //
    
public:
        
    const char *getDescription() const override { return "MessageQueue"; }

private:
    
    void _reset() override { };
    
    usize _size() override { return 0; }
    usize _load(const u8 *buffer) override { return 0; }
    usize _save(u8 *buffer) override { return 0; }
    
    
    //
    // Managing the queue
    //
    
public:
    
    // Registers a listener together with it's callback function
    void addListener(const void *listener, Callback *func);
    
    // Unregisters a listener
    void removeListener(const void *listener);
    
    // Returns the next pending message, or nullptr if the queue is empty
    Message get();
    
    // Writes a message into the queue and propagates it to all listeners
    void put(MsgType type, long data = 0);
    
private:
    
    // Used by 'put' to propagates a single message to all registered listeners
    void propagate(const Message &msg) const;
};
