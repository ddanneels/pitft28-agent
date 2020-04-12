#ifndef PITFT_AGENT_FRAMEBUFFER_H
#define PITFT_AGENT_FRAMEBUFFER_H

class Framebuffer {
public:
    static int find_framebuffer();

protected:
    Framebuffer( char* device );
    ~Framebuffer();
};


#endif // PITFT_AGENT_FRAMEBUFFER_H