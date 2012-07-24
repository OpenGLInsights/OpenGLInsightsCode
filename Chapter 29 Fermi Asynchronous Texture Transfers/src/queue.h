#ifndef QUEUE_H
#define QUEUE_H

/// This is an OpenGL multi-threaded object queue.
class queue
{
public:
    /// An element in the queue
    class element
    {
        friend class queue;
    private:
        element *_next;
        GLsync _fence;

    public:
        element();
        ~element();

        GLuint _id;         ///< the id of the OpenGL object
    };

private:
    HANDLE _event;
    HANDLE _mutex;
    element *_root;

public:
    queue();
    ~queue();

    element *dequeue();
    void enqueue(element *e);
};

#endif // QUEUE_H