/* CDRMonitor.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef SHOWCONSTRUCTDESTRUCT_H
#define SHOWCONSTRUCTDESTRUCT_H
#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "noDefaultMethods.h"
#include "lock.h"
#include "pvType.h"

namespace epics { namespace pvData { 

//! Used to pass around snapshots
struct CDRCount { // default copy and assignment are ok
    size_t cons, dtys;
    long refs;
    CDRCount():cons(0),dtys(0),refs(0){}
    CDRCount& operator+=(const CDRCount& o)
        {cons+=o.cons; dtys+=o.dtys; refs+=o.refs; return *this;}
    CDRCount& operator=(size_t count) // reset counters
        {cons=count; dtys=count; refs=count; return *this;}
};

class CDRNode;

//! @brief Global registrar for CDRNodes
class CDRMonitor : private NoDefaultMethods {
public:
    static CDRMonitor& get();

    CDRNode* addNode(CDRNode& next)
    {
        CDRNode *ret=firstNode;
        firstNode=&next;
        return ret;
    }

    CDRCount current(); //!< current global count

    CDRNode* first(){return firstNode;}

    void show(FILE*);
private:
    // Private ctor for singleton
    CDRMonitor();
    CDRNode *firstNode;

    static CDRMonitor *theone;
    static void init(void*);
};

//! Counters for Construction, Destruction, and References of one class
class CDRNode : private NoDefaultMethods {
public:
    CDRNode(const String& name)
        :nodeName(name)
        ,current()
        ,guard()
        ,nextNode(CDRMonitor::get().addNode(*this))
    {}
    void construct(){Lock x(&guard); current.cons++;}
    void destruct(){Lock x(&guard); current.dtys++;}
    void incRef(){Lock x(&guard); current.refs++;}
    void decRef(){Lock x(&guard); current.refs--;}

    CDRNode* next() const{return nextNode;}

    CDRCount get() const{Lock x(&guard); return current;}

    void show(FILE*);
private:
    const String nodeName;
    CDRCount current;
    mutable Mutex guard;
    CDRNode * const nextNode;
};

#define PVDATA_REFCOUNT_MONITOR_DEFINE(NAME) static CDRNode NAME ## _node(#NAME)

#define PVDATA_REFCOUNT_MONITOR_DESTRUCT(NAME) NAME ## _node.destruct()

#define PVDATA_REFCOUNT_MONITOR_CONSTRUCT(NAME) NAME ## _node.construct()


}}
#endif  /* SHOWCONSTRUCTDESTRUCT_H */