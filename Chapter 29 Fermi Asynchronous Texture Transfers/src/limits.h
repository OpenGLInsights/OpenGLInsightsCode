#ifndef LIMITS_H
#define LIMITS_H

/// Global limits
class limits
{
public:
    static const unsigned int MAX_STREAMS = 64;     ///< that many streams are supported
    static const unsigned int MAX_STAGES = 3;       ///< maximum stage count
};

#endif // LIMITS_H