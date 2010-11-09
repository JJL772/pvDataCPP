/* linkedListVoid.cpp */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>

#include "lock.h"
#include "pvType.h"
#include "pvIntrospect.h"
#include "linkedListVoid.h"

namespace epics { namespace pvData { 

static volatile int64 totalNodeConstruct = 0;
static volatile int64 totalNodeDestruct = 0;
static volatile int64 totalListConstruct = 0;
static volatile int64 totalListDestruct = 0;
static Mutex *globalMutex = 0;
static String alreadyOnList("already on list");

int64 LinkedListVoidNode::getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalNodeConstruct;
}

int64 LinkedListVoidNode::getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalNodeDestruct;
}

int64 LinkedListVoid::getTotalConstruct()
{
    Lock xx(globalMutex);
    return totalListConstruct;
}

int64 LinkedListVoid::getTotalDestruct()
{
    Lock xx(globalMutex);
    return totalListDestruct;
}


void LinkedListVoid::init() {
     static Mutex mutex = Mutex();
      Lock xx(&mutex);
     if(globalMutex==0) globalMutex = new Mutex();
}

LinkedListVoidNode::LinkedListVoidNode(void *object)
: object(object),before(0),after(0)
{
    LinkedListVoid::init();
    Lock xx(globalMutex);
    totalNodeConstruct++;
}

LinkedListVoidNode::LinkedListVoidNode(bool isHead)
: object(this),before(this),after(this)
{
    LinkedListVoid::init();
    Lock xx(globalMutex);
    totalNodeConstruct++;
}

LinkedListVoidNode::~LinkedListVoidNode()
{
    Lock xx(globalMutex);
    totalNodeDestruct++;
}

void *LinkedListVoidNode::getObject() {
    return object;
}

bool LinkedListVoidNode::isOnList()
{
    if(before==0 && after==0) return false;
    return true;
}

LinkedListVoid::LinkedListVoid()
: head(new LinkedListVoidNode(true)),length(0)
{
    LinkedListVoid::init();
    Lock xx(globalMutex);
    totalListConstruct++;
}

LinkedListVoid::~LinkedListVoid()
{
    Lock xx(globalMutex);
    delete head;
    totalListDestruct++;
}

int LinkedListVoid::getLength()
{
    return length;
}

void LinkedListVoid::addTail(LinkedListVoidNode *node)
{
    if(node->before!=0 || node->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    node->before = head->before;
    node->after = head;
    head->before->after = node;
    head->before = node;
    ++length;
}

void LinkedListVoid::addHead(LinkedListVoidNode *node)
{
    if(node->before!=0 || node->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    node->after = head->after;
    node->before = head;
    head->after->before = node;
    head->after = node;
    ++length;
}

void LinkedListVoid::insertAfter(LinkedListVoidNode *node,
    LinkedListVoidNode *addNode)
{
    LinkedListVoidNode *existingNode = node;
    LinkedListVoidNode *newNode = addNode;
    if(existingNode->after==0 || existingNode->before==0) {
        throw std::logic_error(String("listNode not on list"));
    }
    if(newNode->before!=0 || newNode->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    newNode->after = existingNode->after;
    newNode->before = existingNode;
    existingNode->after->before = newNode;
    existingNode->after = newNode;
    ++length;
}

void LinkedListVoid::insertBefore(LinkedListVoidNode *node,
    LinkedListVoidNode *addNode)
{
    LinkedListVoidNode *existingNode = node;
    LinkedListVoidNode *newNode = addNode;
    if(existingNode->after==0 || existingNode->before==0) {
        throw std::logic_error(String("listNode not on list"));
    }
    if(newNode->before!=0 || newNode->after!=0) {
        throw std::logic_error(alreadyOnList);
    }
    newNode->after = existingNode;
    newNode->before = existingNode->before;
    existingNode->before->after = newNode;
    existingNode->before = newNode;
    ++length;
}

LinkedListVoidNode *LinkedListVoid::removeTail()
{
    if(head->after==head) return 0;
    LinkedListVoidNode *node = head->before;
    remove(head->before);
    return node;
}

LinkedListVoidNode *LinkedListVoid::removeHead()
{
    if(head->after==head) return 0;
    LinkedListVoidNode *node = head->after;
    remove(head->after);
    return node;
}

void LinkedListVoid::remove(LinkedListVoidNode *listNode)
{
    LinkedListVoidNode *node = listNode;
    if(node->before==0 || node->after==0) {
        throw std::logic_error(String("listNode not on list"));
    }
    LinkedListVoidNode *prev = node->before;
    LinkedListVoidNode *next = node->after;
    node->after = node->before = 0;
    prev->after = next;
    next->before = prev;
    length--;
}

void LinkedListVoid::remove(void * object)
{
    LinkedListVoidNode *node = getHead();
    while(node!=0) {
        if(node->getObject()==object) {
            remove(node);
            return;
        }
        node = getNext(node);
    }
}

LinkedListVoidNode *LinkedListVoid::getHead()
{
    if(head->after==head) return 0;
    return head->after;
}

LinkedListVoidNode *LinkedListVoid::getTail()
{
    if(head->after==head) return 0;
    return head->before;
}

LinkedListVoidNode *LinkedListVoid::getNext(LinkedListVoidNode *listNode)
{
    if(listNode->after==head) return 0;
    return listNode->after;
}

LinkedListVoidNode *LinkedListVoid::getPrev(LinkedListVoidNode *listNode)
{
    if(listNode->before==head) return 0;
    return listNode->before;
}

bool LinkedListVoid::isEmpty()
{
    if(head->after==head) return true;
    return false;
}

bool LinkedListVoid::contains(void * object)
{
    LinkedListVoidNode *node = getHead();
    while(node!=0) {
        if(node->getObject()==object) {
            return true;
        }
        node = getNext(node);
    }
    return false;
}

}}
