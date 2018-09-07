#ifndef NEO_SDLINPUTCONTEXT_H
#define NEO_SDLINPUTCONTEXT_H

#include <InputContext.h>
#include <unordered_map>

class QtInputContext : public Neo::InputContext
{
public:
	QtInputContext(): InputContext() {}
	virtual void handleInput();
	virtual void setMouseRelative(bool value);
};

#endif //NEO_SDLINPUTCONTEXT_H
