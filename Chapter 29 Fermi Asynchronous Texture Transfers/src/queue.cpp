#include "stdafx.h"

#include "queue.h"

/// Constructor
queue::queue()
{
    _event = CreateEvent(NULL, FALSE, FALSE, NULL);
    _mutex = CreateMutex(NULL, false, NULL);
    _root = NULL;
}

/// Destructor
queue::~queue()
{
    element *cur = _root, *next;

    while(cur)
    {
        next = cur->_next;
        delete cur;
        cur = next;
    }

    if(_mutex)
        CloseHandle(_mutex);
    if(_event)
        CloseHandle(_event);
}

/// Constructor
queue::element::element()
{
    _next = NULL;
    _fence = 0;
    _id = 0;
}

/// Destructor
queue::element::~element()
{
}

/// Get an element form the queue. Wait if there is no element.
///
/// @return a new element
queue::element *queue::dequeue()
{
    element *e = NULL;

    while(!e)
    {
        WaitForSingleObject(_mutex, INFINITE);
        if(_root)
        {
            e = _root;
            _root = e->_next;
        }
        ReleaseMutex(_mutex);
        if(!e)
            WaitForSingleObject(_event, INFINITE);
    }

    glWaitSync(e->_fence, 0, GL_TIMEOUT_IGNORED);
    glDeleteSync(e->_fence);
    e->_fence = 0;

    return e;
}

/// Add an element to the queue
///
/// @param e [in] element
void queue::enqueue(element *e)
{
    element *cur;

    e->_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    WaitForSingleObject(_mutex, INFINITE);
    if(_root)
    {
        cur = _root;
        while(cur->_next)
            cur = cur->_next;
        cur->_next = e;
    }
    else
    {
        _root = e;
    }
    e->_next = NULL;
    ReleaseMutex(_mutex);

    SetEvent(_event);
}
