#include <exception>

using namespace std;

class camException : public exception
{
private:
	const char * info;
public:
	camException(const char * s):info(s)
	{}

	virtual const char * what() const throw()
    { 
    	return info; 
    }

    virtual ~camException() throw()
    {}

};